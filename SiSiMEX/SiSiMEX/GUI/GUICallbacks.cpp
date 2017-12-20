#include "../YellowPages.h"
#include "../MultiAgentApplication.h"
#include "../Log.h"
#include "GUICallbacks.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Singletons
////////////////////////////////////////////////////////////////////////////////

YellowPages *g_YellowPages = nullptr;
MultiAgentApplication *g_MultiAgentApp = nullptr;


////////////////////////////////////////////////////////////////////////////////
// LogOutputs
////////////////////////////////////////////////////////////////////////////////

class AgentsLogOutput : public LogOutput {
public:
	void writeMessage(const std::string &message) override {
		guiTextDisplayAgentsLog->insert_position(guiTextDisplayAgentsLog->buffer()->length());
		guiTextDisplayAgentsLog->insert( message.c_str());
	}
} agentsLogOutput;

class YellowPagesLogOutput : public LogOutput {
public:
	void writeMessage(const std::string &message) override {
		guiTextDisplayYPLog->insert_position(guiTextDisplayYPLog->buffer()->length());
		guiTextDisplayYPLog->insert(message.c_str());
	}
} yellowPagesLogOutput;


////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

void multiagentsApplicationUpdate(void*);
void multiagentsApplicationFinalize(Fl_Window *, void*);
void yellowPagesApplicationUpdate(void*);
void yellowPagesApplicationFinalize(Fl_Window *, void*);


void multiagentsApplicationInitialize()
{
	Fl_Window *w = make_window_multiagents_application();
	w->show();

	Fl_Text_Buffer *buff = new Fl_Text_Buffer();
	guiTextDisplayAgentsLog->buffer(buff);
	g_Log.enableFileOutput("log_multiagents.txt");
	g_Log.addOutput(&agentsLogOutput);

	Fl::add_idle(&multiagentsApplicationUpdate, nullptr);
	Fl::set_atclose(&multiagentsApplicationFinalize);

	g_MultiAgentApp = new MultiAgentApplication();
	g_MultiAgentApp->initialize();
}

void multiagentsApplicationUpdate(void*)
{
	g_MultiAgentApp->update();
}

void multiagentsApplicationFinalize(Fl_Window *window, void*)
{
	window->hide();

	Fl::remove_idle(&multiagentsApplicationUpdate, nullptr);

	g_MultiAgentApp->finalize();
	delete g_MultiAgentApp;
}


void yellowPagesApplicationInitialize()
{
	Fl_Window *w = make_window_yellowpages_application();
	w->show();

	Fl_Text_Buffer *buff = new Fl_Text_Buffer();
	guiTextDisplayYPLog->buffer(buff);
	g_Log.enableFileOutput("log_yellowpages.txt");
	g_Log.addOutput(&yellowPagesLogOutput);

	Fl::add_idle(&yellowPagesApplicationUpdate, nullptr);
	Fl::set_atclose(&yellowPagesApplicationFinalize);

	g_YellowPages = new YellowPages();
	g_YellowPages->initialize();
}

void yellowPagesApplicationUpdate(void*)
{
	g_YellowPages->update();
}

void yellowPagesApplicationFinalize(Fl_Window *window, void*)
{
	window->hide();

	Fl::remove_idle(&yellowPagesApplicationUpdate, nullptr);

	g_YellowPages->finalize();
	delete g_YellowPages;
}


////////////////////////////////////////////////////////////////////////////////
// SiSiMEX main menu
////////////////////////////////////////////////////////////////////////////////

void onguiRadioButtonApplicationModeChanged(Fl_Round_Button*b)
{
	if (b == radioButtonYellowPages) {
		radioButtonAgents->value(0);
		radioButtonYellowPages->value(1);
	} else {
		radioButtonAgents->value(1);
		radioButtonYellowPages->value(0);
	}
}

void onguiButtonStartApplication()
{
	guiWindowApplicationMode->hide();
	if (radioButtonAgents->value() == 1) {
		multiagentsApplicationInitialize();
	} else {
		yellowPagesApplicationInitialize();
	}
}


////////////////////////////////////////////////////////////////////////////////
// Multi-agents application
////////////////////////////////////////////////////////////////////////////////

void onGuiButtonListLocalNodes()
{
	g_MultiAgentApp->listLocalNodes();
}

void onGuiButtonInspectLocalNode()
{
	int nodeId = static_cast<int>(guiSpinnerNodeID->value());
	g_MultiAgentApp->inspectLocalNode(nodeId);
}

void onguiButtonSpawnMCC()
{
	int nodeId = static_cast<int>(guiSpinnerMCCNodeID->value());
	int contributedItemId = static_cast<int>(guiSpinnerMCCContributedItemID->value());
	int constraintItemId = static_cast<int>(guiSpinnerMCCConstraintItemID->value());
	if (constraintItemId == -1) { constraintItemId = NULL_ITEM_ID; }
	g_MultiAgentApp->spawnMCC(nodeId, contributedItemId, constraintItemId);
}

void onguiButtonSpawnMCP()
{
	int nodeId = static_cast<int>(guiSpinnerMCPNodeID->value());
	int itemId = static_cast<int>(guiSpinnerMCPItemID->value());
	g_MultiAgentApp->spawnMCP(nodeId, itemId);
}

void onGuiButtonQuitMultiagentApplication()
{
	multiagentsApplicationFinalize(guiWindowMultiagentApplication, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// YellowPages application
////////////////////////////////////////////////////////////////////////////////

void onguiButtonClearYellowPages()
{
	g_YellowPages->clear();
}

void onGuiButtonQuitYellowPages()
{
	yellowPagesApplicationFinalize(guiWindowYellowPagesApplication, nullptr);
}
