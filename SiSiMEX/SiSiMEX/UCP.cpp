#include "UCP.h"
#include "MCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	// TODO: Add other states...
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
		//setState(ST_WHATEVER_NEXT_STATE...);
		break;

	// TODO: Handle other states

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
	// TODO

	//sendPacketToHost(ip, port, ostream);
}

void UCP::sendConstraint(uint16_t constraintItemId)
{
	// TODO

	//sendPacketToHost(ip, port, ostream);
}

void UCP::createChildMCP(uint16_t constraintItemId)
{
	// TODO
}

void UCP::destroyChildMCP()
{
	// TODO
}
