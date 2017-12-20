#pragma once
#include "Globals.h"
#include "Log.h"
#include "Item.h"
#include "Packets.h"
#include "Node.h"
#include <list>
#include <memory>


class Agent
{
public:

	// Constructor and destructor
	Agent(Node *node);
	virtual ~Agent();

	// Update method (called once per frame)
	virtual void update() = 0;

	// Finalize method, called once before closing the app
	virtual void finalize() = 0;

	// Finish condition (if true, the agent will be removed from AgentManager)
	bool finished() const { return _finished; }

	// State machine
	int state() const { return _state; }
	void setState(int state) { _state = state; }

	// Packet send functions
	bool sendPacketToYellowPages(OutputMemoryStream &stream);
	bool sendPacketToHost(const std::string &ip, uint16_t port, OutputMemoryStream &stream);

	// Function called from MultiAgentApplication to forward packets received from the network
	virtual void OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream) = 0;

protected:

	// Finish condition (if true, will be removed from AgentManager)
	// Should be called by the agent itself when it finished
	void finish();

private:

	bool _finished; /**< Whether or not the agent finished. */


public:

	/** It returns the parent node of the agent. */
	Node *node() const { return _node; }

	/** It returns the identifier of the agent (within the host) */
	uint16_t id() const { return _id; }

private:

	Node *_node; /**< Parent Node/player of the agent. */

	uint16_t _id; /**< Agent identifier. */

	int _state; /**< Current state of the agent. */

	std::vector<TCPSocketPtr> _sockets; /**< Sockets used from this agent. */
};

using AgentPtr = std::shared_ptr<Agent>;
