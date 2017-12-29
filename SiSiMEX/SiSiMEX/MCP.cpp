#include "MCP.h"
#include "UCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	ST_REQUESTING_MCCs,
	// TODO: Add other states
	ST_REQUESTING_NEGOTIATION,
	ST_NEGOTIATING,
	ST_FINISHED
};

MCP::MCP(Node *node, uint16_t itemId) :
	Agent(node),
	_itemId(itemId),
	_negotiationAgreement(false)
{
	setState(ST_INIT);
}

MCP::~MCP()
{
}

void MCP::update()
{
	switch (state())
	{
	case ST_INIT:
		queryMCCsForItem(_itemId);
		setState(ST_REQUESTING_MCCs);
		break;

	// TODO: Handle other states
	case ST_REQUESTING_NEGOTIATION:
		break;

	case ST_NEGOTIATING:
		if (_child_ucp->negotiationFinished())
		{
			setState(ST_FINISHED);
			if (_child_ucp->negotiationAgreement())
				_negotiationAgreement = true;
			else
				_negotiationAgreement = false;
		}
		break;

	default:;
	}
}

void MCP::finalize()
{
	iLog << "MCP finalizing";
	destroyChildUCP();
	finish();
}

void MCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	const PacketType packetType = packetHeader.packetType;
	if (state() == ST_REQUESTING_MCCs && packetType == PacketType::ReturnMCCsForItem)
	{
		//iLog << "OnPacketReceived PacketType::ReturnMCCsForItem " << _itemId;

		// Read the packet
		PacketReturnMCCsForItem packetData;
		packetData.Read(stream);

		for (auto &mccdata : packetData.mccAddresses)
		{
			uint16_t agentId = mccdata.agentId;
			const std::string &hostIp = mccdata.hostIP;
			uint16_t hostPort = mccdata.hostPort;
			//iLog << " - MCC: " << agentId << " - host: " << hostIp << ":" << hostPort;
		}

		// Collect MCC compatible from YP
		_mccRegisters.swap(packetData.mccAddresses);

		// Select MCC to negociate
		_mccRegisterIndex = 0;
		setState(ST_REQUESTING_NEGOTIATION);
		sendNegotiationRequest(_mccRegisters[_mccRegisterIndex]);
		_mccRegisterIndex++;

		socket->Disconnect();
	}

	// TODO: Handle other responses
	else if (state() == ST_REQUESTING_NEGOTIATION && packetType == PacketType::AnswerMCPNegotiation)
	{
		PacketAnswerMCPNegotiation packetData;
		packetData.Read(stream);

		if (packetData.UCC_ID != NULL_AGENT_ID)
		{
			iLog << "MCC with id: " << packetHeader.srcAgentId << " agreeds to start a negotiation";
			setState(ST_NEGOTIATING);

			AgentLocation ucc;
			ucc.hostIP = socket->RemoteAddress().GetIPString();
			ucc.hostPort = LISTEN_PORT_AGENTS;
			ucc.agentId = packetData.UCC_ID;

			createChildUCP(ucc);
		}
		else
		{
			iLog << "MCC doesn't want to negotiate, negotiation has failed";

			if (_mccRegisterIndex < _mccRegisters.size())
			{
				sendNegotiationRequest(_mccRegisters[_mccRegisterIndex]);
				_mccRegisterIndex++;
			}
			else
			{
				setState(ST_FINISHED);
				_negotiationAgreement = false;
			}
		}
	}
}

bool MCP::negotiationFinished() const
{
	// TODO
	return (state() == ST_FINISHED);
}

bool MCP::negotiationAgreement() const
{
	// TODO
	return _negotiationAgreement;
}


bool MCP::queryMCCsForItem(int itemId)
{
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::QueryMCCsForItem;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = -1;
	PacketQueryMCCsForItem packetData;
	packetData.itemId = _itemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	// 1) Ask YP for MCC hosting the item 'itemId'
	return sendPacketToYellowPages(stream);
}

bool MCP::sendNegotiationRequest(const AgentLocation &mccRegister)
{
	// TODO
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::RequestMCCForNegotiation;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = mccRegister.agentId;
	PacketRequestMCCForNegotiation packetData;
	packetData.itemId = _itemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	return sendPacketToHost(mccRegister.hostIP, mccRegister.hostPort, stream);
}

void MCP::createChildUCP(const AgentLocation &uccLoc)
{
	_child_ucp = std::make_shared<UCP>(node(), _itemId, uccLoc);
	g_AgentContainer->addAgent(_child_ucp);
}

void MCP::destroyChildUCP()
{
	// TODO
	if (_child_ucp)
		_child_ucp->finalize();
}
