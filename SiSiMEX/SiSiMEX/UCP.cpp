#include "UCP.h"
#include "MCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	// TODO: Add other states...
	ST_ITEM_REQUESTED,
	ST_WAITING_CONSTRAINT,
	ST_FINISHED
};

UCP::UCP(Node *node, uint16_t requestedItemId, const AgentLocation &uccLocation) :
	Agent(node),
	_requestedItemId(requestedItemId),
	_uccLocation(uccLocation),
	_negotiationAgreement(false)
{
	setState(ST_INIT);
}

UCP::~UCP()
{
}

void UCP::update()
{
	switch (state())
	{
	case ST_INIT:
		requestItem();
		setState(ST_ITEM_REQUESTED);
		break;

	// TODO: Handle other states
	case ST_ITEM_REQUESTED:
		break;

	case ST_WAITING_CONSTRAINT:
		if (_child_mcp->negotiationFinished())
		{
			if (_child_mcp->negotiationAgreement())
			{
				sendConstraint(_child_mcp->requestedItemId());
				setState(ST_ITEM_REQUESTED);
			}
			else
				_negotiationAgreement = false;
		}
		break;
	case ST_FINISHED:
		break;

	default:;
	}
}

void UCP::finalize()
{
	iLog << "UCP requesting item: " << _requestedItemId << " finalizing";
	destroyChildMCP();
	finish();
}

void UCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;

	// TODO: Handle requests
	if (state() == ST_ITEM_REQUESTED && packetType == PacketType::RequestUCPForConstraint)
	{
		setState(ST_WAITING_CONSTRAINT);
		PacketRequestUCPForConstraint packetData;
		packetData.Read(stream);		
		createChildMCP(packetData.itemId);	

		iLog << "UCP looking for constraint: " << packetData.itemId;
	}

	if (state() == ST_ITEM_REQUESTED && packetType == PacketType::SendItemRequestedUCP)
	{
		iLog << "UCP requesting item: " << _requestedItemId << " negotiation finished successfully";

		_negotiationAgreement = true;
		setState(ST_FINISHED);
	}
}

bool UCP::negotiationFinished() const {
	// TODO
	return (state() == ST_FINISHED);
}

bool UCP::negotiationAgreement() const {
	// TODO
	return _negotiationAgreement;
}


void UCP::requestItem()
{
	iLog << "UCP requesting itemID: " << _requestedItemId;
	// TODO
	PacketHeader head;
	head.srcAgentId = id();
	head.dstAgentId = _uccLocation.agentId;
	head.packetType = PacketType::RequestUCCForItem;
	PacketRequestUCCForItem data;
	data.itemId = _requestedItemId;

	OutputMemoryStream stream;
	head.Write(stream);
	data.Write(stream);

	sendPacketToHost(_uccLocation.hostIP, _uccLocation.hostPort, stream);
}

void UCP::createChildMCP(uint16_t constraintItemId)
{
	// TODO
	_child_mcp = std::make_shared<MCP>(node(), constraintItemId);
	g_AgentContainer->addAgent(_child_mcp);
}

void UCP::sendConstraint(uint16_t constraintItemId)
{
	// TODO
	iLog << "UCP requestin item: " << _requestedItemId << " sending constraint: " << constraintItemId << " to UCC";

	PacketHeader head;
	head.srcAgentId = id();
	head.dstAgentId = _uccLocation.agentId;
	head.packetType = PacketType::SendConstraintRequestedUCC;
	PacketSendConstraintRequestedUCC data;
	data.itemId = constraintItemId;

	OutputMemoryStream stream;
	head.Write(stream);
	data.Write(stream);

	sendPacketToHost(_uccLocation.hostIP, _uccLocation.hostPort, stream);
}

void UCP::destroyChildMCP()
{
	// TODO 
	if (_child_mcp)
		_child_mcp->finalize();
}
