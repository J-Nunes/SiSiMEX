#pragma once
#include "Globals.h"
#include "AgentLocation.h"

/**
 * Enumerated type for packets.
 * There must be a value for each kind of packet
 * containing extra data besides the Header.
 */
enum class PacketType
{
	// MCC <-> YP
	RegisterMCC,
	RegisterMCCAck,
	UnregisterMCC,
	UnregisterMCCAck,
	// MCP <-> YP
	QueryMCCsForItem,
	ReturnMCCsForItem,
	// MCP <-> MCC
	// TODO: Add message types
	RequestMCCForNegotiation,
	AnswerMCPNegotiation,
	// UCP <-> UCC
	// TODO: Add message types
	RequestUCCForItem,
	RequestUCPForConstraint,
	SendItemRequestedUCP,
	SendConstraintRequestedUCC,

	Last
};

/**
 * Standard information used by almost all messages in the system.
 * Agents will be communicating among each other, so in many cases,
 * besides the packet type, a header containing the source and the
 * destination agents involved is needed.
 */
class PacketHeader {
public:
	PacketType packetType; // Which type is this packet
	uint16_t srcAgentId;   // Which agent sent this packet?
	uint16_t dstAgentId;   // Which agent is expected to receive the packet?
	PacketHeader() :
		packetType(PacketType::Last),
		srcAgentId(NULL_AGENT_ID),
		dstAgentId(NULL_AGENT_ID)
	{ }
	void Read(InputMemoryStream &stream) {
		stream.Read(packetType);
		stream.Read(srcAgentId);
		stream.Read(dstAgentId);
	}
	void Write(OutputMemoryStream &stream) {
		stream.Write(packetType);
		stream.Write(srcAgentId);
		stream.Write(dstAgentId);
	}
};

/**
 * To register a MCC we need to know which resource/item is
 * being provided by the MCC agent.
 */
class PacketRegisterMCC {
public:
	uint16_t itemId; // Which item has to be registered?
	void Read(InputMemoryStream &stream) {
		stream.Read(itemId);
	}
	void Write(OutputMemoryStream &stream) {
		stream.Write(itemId);
	}
};

/**
* The information is the same required for PacketRegisterMCC so...
*/
using PacketUnregisterMCC = PacketRegisterMCC;

/**
* The information is the same required for PacketRegisterMCC so...
*/
using PacketQueryMCCsForItem = PacketRegisterMCC;

/**
 * This packet is the response for PacketQueryMCCsForItem and
 * is sent by an MCP (MultiCastPetitioner) agent.
 * It contains a list of the addresses of MCC agents contributing
 * with the item specified by the PacketQueryMCCsForItem.
 */
class PacketReturnMCCsForItem {
public:
	std::vector<AgentLocation> mccAddresses;
	void Read(InputMemoryStream &stream) {
		uint16_t count;
		stream.Read(count);
		mccAddresses.resize(count);
		for (auto &mccAddress : mccAddresses) {
			mccAddress.Read(stream);
		}
	}
	void Write(OutputMemoryStream &stream) {
		auto count = static_cast<uint16_t>(mccAddresses.size());
		stream.Write(count);
		for (auto &mccAddress : mccAddresses) {
			mccAddress.Write(stream);
		}
	}
};


// MCP <-> MCC

// TODO: Add message classes
using PacketRequestMCCForNegotiation = PacketRegisterMCC;

class PacketAnswerMCPNegotiation
{
public:
	uint16_t UCC_ID;

	void Read(InputMemoryStream &stream) 
	{
		stream.Read(UCC_ID);
	}
	void Write(OutputMemoryStream &stream) 
	{
		stream.Write(UCC_ID);
	}
};


// UCP <-> UCC

// TODO: Add message classes
using PacketRequestUCCForItem = PacketRegisterMCC;

using PacketRequestUCPForConstraint = PacketRegisterMCC;

using PacketSendItemRequestedUCP = PacketRegisterMCC;

using PacketSendConstraintRequestedUCC = PacketRegisterMCC;

