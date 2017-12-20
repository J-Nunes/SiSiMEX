#pragma once
#include "Net.h"

class TCPNetworkManagerDelegate
{
public:

	virtual ~TCPNetworkManagerDelegate() { }

	virtual void OnAccepted(TCPSocketPtr socket) = 0;
	virtual void OnPacketReceived(TCPSocketPtr socket, InputMemoryStream &stream) = 0;
	virtual void OnDisconnected(TCPSocketPtr socket) = 0;
};

class TCPNetworkManager
{
public:

	TCPNetworkManager(TCPNetworkManagerDelegate *dlg);
	virtual ~TCPNetworkManager();

	void AddSocket(TCPSocketPtr socket);

	void HandleIncomingPackets(int timeoutMillis = 0);

	void HandleOutgoingPackets();

	void Finalize();

private:

	TCPNetworkManagerDelegate *mDelegate;
	std::vector<TCPSocketPtr> mSockets;
};

