#include "UCC.h"



enum State
{
	// TODO: Add some states
};

UCC::UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node),
	_contributedItemId(contributedItemId),
	_constraintItemId(constraintItemId),
	_negotiationAgreement(false)
{
	//setState(ST_WHATEVER_INITIAL_STATE ...);
}

UCC::~UCC()
{
}

void UCC::update()
{
	// Nothing to do
}

void UCC::finalize()
{
	finish();
}

void UCC::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;

	// TODO Receive requests and send back responses...
}

bool UCC::negotiationFinished() const {
	// TODO
	return false;
}

bool UCC::negotiationAgreement() const {
	// TODO
	return false;
}
