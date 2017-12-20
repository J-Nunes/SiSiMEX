#pragma once
#include "Agent.h"

class UCC :
	public Agent
{
public:

	// Constructor and destructor
	UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId);
	~UCC();

	void update() override;
	void finalize() override;

	void OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream) override;

	// Whether or not the negotiation finished
	bool negotiationFinished() const;

	// Whether ir not there was a negotiation agreement
	bool negotiationAgreement() const;

private:

	uint16_t _contributedItemId; /**< The contributed item. */
	uint16_t _constraintItemId; /**< The constraint item. */

	bool _negotiationAgreement; /**< Was there a negotiation agreement? */
};

