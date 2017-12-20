#include "MultiAgentApplication.h"
#include "Globals.h"
#include "Packets.h"
#include "Log.h"
#include "MCC.h"
#include "MCP.h"
#include <Windows.h>
#include <iostream>
#include <sstream>


MultiAgentApplication::MultiAgentApplication() :
	_running(true),
	_networkManager(this)
{
	g_AgentContainer = &_agentContainer;
	g_NetworkManager = &_networkManager;
}

MultiAgentApplication::~MultiAgentApplication()
{
	g_NetworkManager = nullptr;
}

bool MultiAgentApplication::initialize()
{
	iLog << "-------------------------------------------------------------";
	iLog << "                    SiSiMEX Multi-Agents                     ";
	iLog << "-------------------------------------------------------------";
	iLog << "";

	// Initialize sockets library
	int res;
	if (!SocketUtil::StaticInit()) {
		eLog << "SocketUtil::StaticInit() failed";
		return false;
	}

	// Create listen socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr) {
		eLog << "SocketUtil::CreateTCPSocket() failed";
		return false;
	}
	iLog << " - Server Listen socket created";

	// Bind
	const int port = LISTEN_PORT_AGENTS;
	SocketAddress bindAddress(port); // localhost:LISTEN_PORT_YP
	listenSocket->SetReuseAddress(true);
	res = listenSocket->Bind(bindAddress);
	if (res != NO_ERROR) { return false; }
	iLog << " - Socket Bind to interface 127.0.0.1:" << LISTEN_PORT_YP;

	// Listen mode
	res = listenSocket->Listen();
	if (res != NO_ERROR) { return false; }
	iLog << " - Socket entered in Listen state...";

	// Add the socket to the manager
	_networkManager.AddSocket(listenSocket);

//#define RANDOM
#ifdef RANDOM
	// Initialize nodes
	for (int i = 0; i < MAX_NODES; ++i)
	{
		// Create and intialize nodes
		NodePtr node = std::make_shared<Node>();
		node->initialize();
		_nodes.push_back(node);
	}

	// Create MCP agents for each node
	for (int i = 0; i < MAX_NODES; ++i)
	{
		// Spawn MCC (MultiCastContributors) one for each spare item
		NodePtr node = _nodes[i];
		ItemList spareItems = node->itemList().getSpareItems();
		for (auto item : spareItems.items()) {
			spawnMCC(i, item.id());
		}
	}
#else
	_nodes.push_back(std::make_shared<Node>());
	_nodes.push_back(std::make_shared<Node>());
	_nodes.push_back(std::make_shared<Node>());
	_nodes.push_back(std::make_shared<Node>());
	_nodes[0]->itemList().addItem(Item(0));
	_nodes[1]->itemList().addItem(Item(1));
	_nodes[2]->itemList().addItem(Item(2));
	_nodes[3]->itemList().addItem(Item(3));

	spawnMCC(1, 1, 2); // Node 1 offers 1 but wants 2
	spawnMCC(2, 2, 3); // Node 2 offers 2 but wants 3
	spawnMCC(3, 3, 0); // Node 3 offers 3 but wants 0

	spawnMCC(0, 0); // Node 0 offers 0
	//spawnMCP(0, 1); // Node 0 wants  1
#endif

	return true;
}

void MultiAgentApplication::update()
{
	const int timeoutMillis = 100;
	_networkManager.HandleIncomingPackets(timeoutMillis);

	// Invoke update on all agents
	_agentContainer.update();

	// Check the results of agents
	std::vector<MCC*> mccsAlive;
	std::vector<MCP*> mcpsAlive;
	for (auto mcc : _mccs) {
		if (mcc->negotiationFinished()) {
			Node *node = mcc->node();
			node->itemList().removeItem(mcc->contributedItemId());
			node->itemList().addItem(mcc->constraintItemId());
			mcc->finalize();
		} else {
			mccsAlive.push_back(mcc);
		}
	}
	for (auto mcp : _mcps) {
		if (mcp->negotiationFinished()) {
			if (mcp->negotiationAgreement()) {
				Node *node = mcp->node();
				node->itemList().addItem(mcp->requestedItemId());
			}
			mcp->finalize();
		}
		else {
			mcpsAlive.push_back(mcp);
		}
	}
	_mcps.swap(mcpsAlive);
	_mccs.swap(mccsAlive);

	// Remove finished agents
	_agentContainer.postUpdate();

	_networkManager.HandleOutgoingPackets();
}

void MultiAgentApplication::finalize()
{
	_nodes.clear();

	_agentContainer.finalize();

	// Extra updates to wait agents to finish
	for (auto i = 0; i < 10 && !_agentContainer.empty(); ++i) {
		update(); 
		Sleep(17);
	}

	_networkManager.Finalize(); // To close sockets

	SocketUtil::CleanUp();
}

// User actions
void MultiAgentApplication::listLocalNodes()
{
	std::ostringstream oss;
	for (auto i = 0; i < _nodes.size(); ++i) {
		oss << i << " ";
	}
	iLog << "List of Local Nodes: " << oss.str().c_str();
}

void MultiAgentApplication::inspectLocalNode(int nodeId)
{
	iLog << "Inpect local node " << nodeId;
	if (nodeId >= 0 && nodeId < _nodes.size()) {
		NodePtr node = _nodes[nodeId];

		{ std::ostringstream oss;
		for (auto item : node->itemList().items()) {
			oss << item.id() << " ";
		}
		iLog << " - Items: " << oss.str().c_str(); }

		{ std::ostringstream oss;
		ItemList spareItems = node->itemList().getSpareItems();
		for (auto item : spareItems.items()) {
			oss << item.id() << " ";
		}
		iLog << " - Spare Items: " << oss.str().c_str(); }
	} else {
		wLog << "Could not find node with ID " << nodeId;
	}
}

void MultiAgentApplication::spawnMCP(int nodeId, int itemId)
{
	iLog << "Spawn MCP for node " << nodeId << " petitioning item " << itemId;
	if (nodeId >= 0 && nodeId < _nodes.size()) {
		NodePtr node = _nodes[nodeId];
		MCP *mcp = new MCP(node.get(), itemId);
		_mcps.push_back(mcp);
		AgentPtr agentPtr(mcp);
		g_AgentContainer->addAgent(agentPtr);
	} else {
		wLog << "Could not find node with ID " << nodeId;
	}
}

void MultiAgentApplication::spawnMCC(int nodeId, int contributedItemId, int constraintItemId)
{
	iLog << "Spawn MCC for node " << nodeId << " contributing item " << contributedItemId << " - constraint item " << constraintItemId;
	if (nodeId >= 0 && nodeId < _nodes.size()) {
		NodePtr node = _nodes[nodeId];
		MCC *mcc = new MCC(node.get(), contributedItemId, constraintItemId);
		_mccs.push_back(mcc);
		AgentPtr agentPtr(mcc);
		g_AgentContainer->addAgent(agentPtr);
	} else {
		wLog << "Could not find node with ID " << nodeId;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// Network manager delegate
//////////////////////////////////////////////////////////////////////////////////////////

void MultiAgentApplication::OnAccepted(TCPSocketPtr socket)
{
	//iLog << "OnAccepted";
}

void MultiAgentApplication::OnPacketReceived(TCPSocketPtr socket, InputMemoryStream &stream)
{
	//iLog << "OnPacketReceived";
	
	PacketHeader packetHead;
	packetHead.Read(stream);

	// Get the agent
	auto agentPtr = _agentContainer.getAgent(packetHead.dstAgentId);
	if (agentPtr != nullptr)
	{
		agentPtr->OnPacketReceived(socket, packetHead, stream);
	}
	else
	{
		eLog << "Couldn't find agent: " << packetHead.dstAgentId;
	}
}

void MultiAgentApplication::OnDisconnected(TCPSocketPtr socket)
{
	//iLog << "OnDisconnected";
}
