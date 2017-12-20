#include "YellowPages.h"
#include "Globals.h"
#include "Packets.h"
#include "Agent.h"
#include "Log.h"
#include <Net.h>
#include <iostream>

#if 0

#endif

YellowPages::YellowPages() :
	_networkManager(this)
{
	g_NetworkManager = &_networkManager;
}


YellowPages::~YellowPages()
{
	g_NetworkManager = nullptr;
}


bool YellowPages::initialize()
{
	iLog << "-------------------------------------------------------------";
	iLog << "                   SiSiMEX: Yellow Pages                     ";
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
	const int port = LISTEN_PORT_YP;
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

	return true;
}

void YellowPages::update()
{
	const int timeoutMillis = 30;
	_networkManager.HandleIncomingPackets(timeoutMillis);

	_networkManager.HandleOutgoingPackets();
}

void YellowPages::finalize()
{
	SocketUtil::CleanUp();
}

void YellowPages::clear()
{
	iLog << "All MCCs were removed from the registry...";
	_mccByItem.clear();
}

void YellowPages::OnAccepted(TCPSocketPtr socket)
{
	//std::cout << "OnAccepted" << std::endl;
}

void YellowPages::OnPacketReceived(TCPSocketPtr socket, InputMemoryStream &stream)
{
	iLog << "OnPacketReceived: ";

	// Read packet header
	PacketHeader inPacketHead;
	inPacketHead.Read(stream);

	if (inPacketHead.packetType == PacketType::RegisterMCC)
	{
		iLog << "PacketType::RegisterMCC";

		// Read the packet
		PacketRegisterMCC inPacketData;
		inPacketData.Read(stream);

		// Register the MCC into the yellow pages
		AgentLocation mcc;
		mcc.hostIP = socket->RemoteAddress().GetIPString();
		mcc.hostPort = LISTEN_PORT_AGENTS;
		mcc.agentId = inPacketHead.srcAgentId;
		_mccByItem[inPacketData.itemId].push_back(mcc);

		// Host address
		std::string hostAddress = socket->RemoteAddress().GetString();

		iLog << " - MCC Agent ID: " << inPacketHead.srcAgentId;
		iLog << " - Contributed Item ID: " << inPacketData.itemId;
		iLog << " - Remote host address: " << hostAddress;

		// Send RegisterMCCAck packet
		OutputMemoryStream outStream;
		PacketHeader outPacket;
		outPacket.packetType = PacketType::RegisterMCCAck;
		outPacket.dstAgentId = inPacketHead.srcAgentId;
		outPacket.Write(outStream);
		socket->SendPacket(outStream.GetBufferPtr(), outStream.GetSize());
	}
	else if (inPacketHead.packetType == PacketType::UnregisterMCC)
	{
		iLog << "PacketType::UnregisterMCC";

		// Read the packet
		PacketUnregisterMCC inPacketData;
		inPacketData.Read(stream);

		// Unregister the MCC from the yellow pages
		std::list<AgentLocation> &mccs(_mccByItem[inPacketData.itemId]);
		for (auto it = mccs.begin(); it != mccs.end();) {
			if (it->agentId == inPacketHead.srcAgentId) {
				auto oldIt = it++;
				mccs.erase(oldIt);
				break;
			} else {
				++it;
			}
		}

		iLog << " - MCC Agent ID: " << inPacketHead.srcAgentId;
		iLog << " - Contributed Item ID: " << inPacketData.itemId;

		// Send RegisterMCCAck packet
		OutputMemoryStream outStream;
		PacketHeader outPacket;
		outPacket.packetType = PacketType::UnregisterMCCAck;
		outPacket.dstAgentId = inPacketHead.srcAgentId;
		outPacket.Write(outStream);
		socket->SendPacket(outStream.GetBufferPtr(), outStream.GetSize());
	}
	else if (inPacketHead.packetType == PacketType::QueryMCCsForItem)
	{
		iLog << "PacketType::QueryMCCsForItem";
		
		// Read packet
		PacketQueryMCCsForItem inPacketData;
		inPacketData.Read(stream);

		// Response packet
		PacketReturnMCCsForItem outPacketData;

		// Obtain the MCCAddresses
		auto itemId = inPacketData.itemId;
		auto &mccAddressList = _mccByItem[itemId];
		for (auto &mccAddress : mccAddressList) {
			outPacketData.mccAddresses.push_back(mccAddress);
		}

		// Send response packet
		OutputMemoryStream outStream;
		PacketHeader outPacketHead;
		outPacketHead.packetType = PacketType::ReturnMCCsForItem;
		outPacketHead.dstAgentId = inPacketHead.srcAgentId;
		outPacketHead.Write(outStream);
		outPacketData.Write(outStream);
		socket->SendPacket(outStream.GetBufferPtr(), outStream.GetSize());
	}
}

void YellowPages::OnDisconnected(TCPSocketPtr socket)
{
	//std::cout << "OnDisconnected" << std::endl;
}
