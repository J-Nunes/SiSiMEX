#include "AgentContainer.h"


AgentContainer::AgentContainer()
{
}

AgentContainer::~AgentContainer()
{
}

void AgentContainer::addAgent(AgentPtr agent)
{
	_agents.push_back(agent);
}

AgentPtr AgentContainer::getAgent(int agentId)
{
	// Agent search
	for (auto agent : _agents) {
		if (agent->id() == agentId) {
			return agent;
		}
	}

	return nullptr;
}

bool AgentContainer::empty() const
{
	return _agents.empty();
}

void AgentContainer::update()
{
	// Update all agents
	for (auto agent : _agents)
	{
		if (!agent->finished()) {
			agent->update();
		}
	}
}

void AgentContainer::postUpdate()
{
	// Track alive agents
	std::vector<AgentPtr> agentsAlive;

	// Update all agents
	for (auto agent : _agents)
	{
		// Keep track of alive agents
		if (!agent->finished()) {
			agentsAlive.push_back(agent);
		}
	}

	// Remove finished agents
	_agents.swap(agentsAlive);
}

void AgentContainer::finalize()
{
	// Update all agents
	for (auto agent : _agents) {
		agent->finalize();
	}
}
