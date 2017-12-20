#include "Agent.h"

uint16_t g_IdCounter = 1;

Agent::Agent(Node *node) :
	_finished(false),
	_node(node),
	_id(g_IdCounter++)
{
}


Agent::~Agent()
{
}

bool Agent::sendPacketToYellowPages(OutputMemoryStream &stream)
{
	// Create socket
	TCPSocketPtr agentSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (agentSocket == nullptr) {
		eLog << "SocketUtil::CreateTCPSocket() failed";
		return false;
	}

	// Connect to Yellow Pages
	char addressAndPort[128];
	sprintf_s(addressAndPort, "%s:%d", HOSTNAME_YP, LISTEN_PORT_YP);
	SocketAddress yellowPagesAddress(addressAndPort);
	int res = agentSocket->Connect(yellowPagesAddress);
	if (res != NO_ERROR) {
		eLog << "TCPSocket::Connect() failed";
		return false;
	}

	// Add socket to the network manager
	g_NetworkManager->AddSocket(agentSocket);

	// Append data
	agentSocket->SendPacket(stream.GetBufferPtr(), stream.GetSize());
	return true;
}

bool Agent::sendPacketToHost(const std::string &ip, uint16_t port, OutputMemoryStream &stream)
{
	// Create socket
	TCPSocketPtr agentSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (agentSocket == nullptr) {
		eLog << "SocketUtil::CreateTCPSocket() failed";
		return false;
	}

	// Connect to Yellow Pages
	char addressAndPort[128];
	sprintf_s(addressAndPort, "%s:%d", ip.c_str(), port);
	SocketAddress yellowPagesAddress(addressAndPort);
	int res = agentSocket->Connect(yellowPagesAddress);
	if (res != NO_ERROR) {
		eLog << "TCPSocket::Connect() failed";
		return false;
	}

	// Add socket to the network manager
	g_NetworkManager->AddSocket(agentSocket);

	// Append data
	agentSocket->SendPacket(stream.GetBufferPtr(), stream.GetSize());
	return true;
}

void Agent::finish()
{
	// Tell the AgentContainer to remove this Agent
	_finished = true;

	// Disconnect all sockets used by this agent
	for (auto socket : _sockets) {
		socket->Disconnect();
	}
	_sockets.clear();
}