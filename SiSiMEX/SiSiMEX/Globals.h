#pragma once
#include <cinttypes>

// Constants ///////////////////////////////////////////////////////////

/**
 * Typically, each host of the distirbuted system corresponds
 * to a single Node in the network topology. However, for
 * testing purposes and in order to allow local executions
 * of this application, a single host allows having more than
 * one node. This constant defines how many nodes there will
 * exist in a single machine.
 */
static const int MAX_NODES = 3;

/**
 * This constant defines which is the maximum number of
 * items of the catalogue. Items will be identified by an
 * index between 0 and MAX_ITEMS - 1.
 */
static const int MAX_ITEMS = 5;

/** Hostname (or IP address) of the YellowPages process. */
static const char *HOSTNAME_YP = "localhost";

/** Listen port used by the YellowPages process. */
static const uint16_t LISTEN_PORT_YP = 8000;

/** Listen port used by the multi-agent application. */
static const uint16_t LISTEN_PORT_AGENTS = 8001;

/**
 * Constant used to specify that a message was sent to,
 * or received from no agent. This is the case when
 * communicating with the YellowPages. Yellow pages is
 * a global service that contains information about
 * contributor agents, but uses no agents to work.
 */
static const uint16_t NULL_AGENT_ID = 0;

/**
 * Constant used to specify a null item ID.
 * This is used mainly in MCCs without an item
 * constraint, that is, those contributors that don't
 * require another item in exchange to provide its
 * resource. In those cases, the constraint item
 * attribute is assigned the NULL_ITEM_ID value.
 */
static const uint16_t NULL_ITEM_ID = 9999;



// Singletons //////////////////////////////////////////////////////////

// Forward class declarations
class AgentContainer;
class TCPNetworkManager;

/**
 * Singleton.
 * Global access to the agent container.
 */
extern AgentContainer *g_AgentContainer;

/**
* Singleton.
* Global access to the network manager.
*/
extern TCPNetworkManager *g_NetworkManager;
