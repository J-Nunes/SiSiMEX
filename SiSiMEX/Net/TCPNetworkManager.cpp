#include "Net.h"


TCPNetworkManager::TCPNetworkManager(TCPNetworkManagerDelegate *dlg) :
	mDelegate(dlg)
{
}

TCPNetworkManager::~TCPNetworkManager()
{
}

void TCPNetworkManager::AddSocket(TCPSocketPtr socket)
{
	mSockets.push_back(socket);
}

void TCPNetworkManager::HandleIncomingPackets(int timeoutMillis)
{
	std::vector<TCPSocketPtr> socketsToReadOutput;
	SocketUtil::Select(&mSockets, &socketsToReadOutput, nullptr, nullptr, nullptr, nullptr, timeoutMillis);

	for (auto socket : socketsToReadOutput)
	{
		if (socket->IsListening())
		{
			SocketAddress fromAddress;
			TCPSocketPtr connectedSocket = socket->Accept(fromAddress);
			if (connectedSocket != nullptr) {
				mSockets.push_back(connectedSocket);
				mDelegate->OnAccepted(connectedSocket);
			}
		}
		else
		{
			socket->HandleIncomingData();

			if (socket->IsDisconnected())
			{
				// Nothing to do
			}
			else
			{
				// 1) Crear in InputMemoryStream
				InputMemoryStream inputMemoryStream;

				while (socket->ReceivePacket(inputMemoryStream.GetBufferPtr(), inputMemoryStream.GetCapacity()))
				{
					mDelegate->OnPacketReceived(socket, inputMemoryStream);
					inputMemoryStream.Clear();
				}
			}
		}
	}
}

void TCPNetworkManager::HandleOutgoingPackets()
{
	// Select sockets that have outgoing data
	std::vector<TCPSocketPtr> socketsWithDataToWrite;
	for (auto socket : mSockets) {
		if (socket->HasOutgoingData()) {
			socketsWithDataToWrite.push_back(socket);
		}
	}

	// Select writable sockets
	std::vector<TCPSocketPtr> writableSockets;
	SocketUtil::Select(nullptr, nullptr, &socketsWithDataToWrite, &writableSockets, nullptr, nullptr);

	// Send replication data
	for (auto socket : writableSockets)
	{
		if (!socket->IsListening() && !socket->IsDisconnected()) // Maybe not needed... check
		{
			socket->HandleOutgoingData();
		}
	}

	// Remove disconnected sockets
	std::vector<TCPSocketPtr> connectedSockets;
	std::vector<SocketAddress> disconnectedSocketAddresses;
	for (auto socket : mSockets) {
		if (!socket->IsDisconnected()) {
			connectedSockets.push_back(socket);
		} else {
			mDelegate->OnDisconnected(socket);
#if 0
			disconnectedSocketAddresses.push_back(socket->RemoteAddress());
#endif
		}
	}
	mSockets.swap(connectedSockets);

#if 0
	// Remove disconnected clients
	for (auto socketAddress : disconnectedSocketAddresses) {
		g_Clients.erase(socketAddress);
	}
#endif
}

void TCPNetworkManager::Finalize()
{
	mSockets.clear();
}
