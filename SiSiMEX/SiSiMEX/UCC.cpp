#include "UCC.h"



enum State
{
	// TODO: Add some states
	ST_IDLE,
	ST_CONSTRAINT_REQUESTED
};

UCC::UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node),
	_contributedItemId(contributedItemId),
	_constraintItemId(constraintItemId),
	_negotiationAgreement(false)
{
	setState(ST_IDLE);

	iLog << "UCC created with ID: " << id() << " Offering item: " << _contributedItemId;
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
	if (state() == ST_IDLE && packetType == PacketType::RequestUCCForItem)
	{
		PacketRequestUCCForItem data;
		data.Read(stream);
		iLog << "UCP requested itemID: " << data.itemId << "I am an UCC and I am offering: " << _contributedItemId;
		
		if (data.itemId == _contributedItemId)
		{
			
			setState(ST_CONSTRAINT_REQUESTED);

			if (_constraintItemId != NULL_ITEM_ID)
			{
				PacketHeader head;
				head.srcAgentId = id();
				head.dstAgentId = packetHeader.srcAgentId;
				head.packetType = PacketType::RequestUCPForConstraint;
				PacketRequestUCPForConstraint data;
				data.itemId = _constraintItemId;

				OutputMemoryStream stream;
				head.Write(stream);
				data.Write(stream);
				sendPacketToHost(socket->RemoteAddress().GetIPString(), LISTEN_PORT_AGENTS, stream);
			}
		}
		else
			iLog << "You shouldn't be here";
	}
}

bool UCC::negotiationFinished() const {
	// TODO
	return false;
}

bool UCC::negotiationAgreement() const {
	// TODO
	return false;
}
