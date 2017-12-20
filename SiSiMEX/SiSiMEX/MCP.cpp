#include "MCP.h"
#include "UCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	ST_REQUESTING_MCCs,
	// TODO: Add other states
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

	default:;
	}
}

void MCP::finalize()
{
	destroyChildUCP();
	finish();
}

void MCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	const PacketType packetType = packetHeader.packetType;
	if (state() == ST_REQUESTING_MCCs && packetType == PacketType::ReturnMCCsForItem)
	{
		iLog << "OnPacketReceived PacketType::ReturnMCCsForItem " << _itemId;

		// Read the packet
		PacketReturnMCCsForItem packetData;
		packetData.Read(stream);

		for (auto &mccdata : packetData.mccAddresses)
		{
			uint16_t agentId = mccdata.agentId;
			const std::string &hostIp = mccdata.hostIP;
			uint16_t hostPort = mccdata.hostPort;
			iLog << " - MCC: " << agentId << " - host: " << hostIp << ":" << hostPort;
		}

		// Collect MCC compatible from YP
		_mccRegisters.swap(packetData.mccAddresses);

		// Select MCC to negociate
		//_mccRegisterIndex = 0;
		///setState(ST_SOME_NEXT_STATE...);

		socket->Disconnect();
	}

	// TODO: Handle other responses
}

bool MCP::negotiationFinished() const
{
	// TODO
	return false;
}

bool MCP::negotiationAgreement() const
{
	// TODO
	return false;
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

	//return sendPacketToHost(hostIP, hostPort, stream);
	return false;
}

void MCP::createChildUCP(const AgentLocation &uccLoc)
{
	// TODO
}

void MCP::destroyChildUCP()
{
	// TODO
}
