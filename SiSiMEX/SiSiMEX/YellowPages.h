#pragma once
#include "AgentLocation.h"
#include <Net.h>
#include<map>

class YellowPages : TCPNetworkManagerDelegate
{
public:

	// Constructor and destructor
	YellowPages();
	~YellowPages();

	// Main methods
	bool initialize();
	void update();
	void finalize();

	// Clear
	void clear();

	// TCPNetworkManagerDelegate methods
	void OnAccepted(TCPSocketPtr socket) override;
	void OnPacketReceived(TCPSocketPtr socket, InputMemoryStream &stream) override;
	void OnDisconnected(TCPSocketPtr socket) override;

private:

	TCPNetworkManager _networkManager; /**< The network manager. */

	std::map<uint16_t, std::list<AgentLocation> > _mccByItem; /**< MCCs accessed by item id. */
};

