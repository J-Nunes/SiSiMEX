#pragma once

#include "Node.h"
#include "AgentContainer.h"
#include <Net.h>
#include <thread>
#include <mutex>
#include <vector>

class MCC;
class MCP;

class MultiAgentApplication : public TCPNetworkManagerDelegate
{
public:

	// Constructor and destructor
	MultiAgentApplication();
	~MultiAgentApplication();

	// Main methods
	bool initialize();
	void update();
	void finalize();

	// User actions (called from GUICallbacks.cpp)
	void listLocalNodes();
	void inspectLocalNode(int nodeId);
	void spawnMCP(int nodeId, int itemId);
	void spawnMCC(int nodeId, int contributedItemId, int constraintItemId = NULL_ITEM_ID);

private:

	// TCPNetworkManagerDelegate methods
	void OnAccepted(TCPSocketPtr socket) override;
	void OnPacketReceived(TCPSocketPtr socket, InputMemoryStream &stream) override;
	void OnDisconnected(TCPSocketPtr socket) override;

	bool _running; /**< Running flag. */

	std::vector<NodePtr> _nodes; /**< Array of nodes spawn in this host. */

	AgentContainer _agentContainer; /**< Manager for all agents in this host (from one or more Nodes). */

	TCPNetworkManager _networkManager; /**< The network manager. */

	std::vector<MCC*> _mccs; /**< Multicast contributors. */
	std::vector<MCP*> _mcps; /**< Multicast petitioners. */
};
