#include "UCP.h"
#include "MCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	// TODO: Add other states...
	ST_ITEM_REQUESTED
};

UCP::UCP(Node *node, uint16_t requestedItemId, const AgentLocation &uccLocation, const AgentLocation &parent_mcp) :
	Agent(node),
	_requestedItemId(requestedItemId),
	_uccLocation(uccLocation),
	_parent_mcp(parent_mcp),
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

	default:;
	}
}

void UCP::finalize()
{
	destroyChildMCP();
	finish();
}

void UCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;

	// TODO: Handle requests
	if (state() == ST_ITEM_REQUESTED && packetType == PacketType::RequestUCPForConstraint)
	{
		PacketRequestUCPForConstraint packetData;
		packetData.Read(stream);
		createChildMCP(packetData.itemId);
		iLog << "UCP looking for constraint: " << packetData.itemId;
	}
}

bool UCP::negotiationFinished() const {
	// TODO
	return false;
}

bool UCP::negotiationAgreement() const {
	// TODO
	return false;
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
	AgentLocation myself;
	myself.hostIP = _uccLocation.hostIP;
	myself.hostPort = _uccLocation.hostPort;
	myself.agentId = id();

	MCP *mcp = new MCP(node(), constraintItemId, &myself);
	AgentPtr agentPtr(mcp);
	g_AgentContainer->addAgent(agentPtr);
}

void UCP::sendConstraint(uint16_t constraintItemId)
{
	// TODO

	//sendPacketToHost(ip, port, ostream);
}

void UCP::destroyChildMCP()
{
	// TODO
}
