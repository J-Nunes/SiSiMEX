#pragma once
#include "Agent.h"
#include <vector>

class AgentContainer
{
public:

	AgentContainer();
	~AgentContainer();

	// Setters
	void addAgent(AgentPtr agent);

	// Getters
	AgentPtr getAgent(int agentId);
	bool empty() const;

	// Update
	void update();

	// Post update
	void postUpdate();

	// Finalize
	void finalize();

public:

	std::vector<AgentPtr> _agents; /**< Array of agents. */
};
