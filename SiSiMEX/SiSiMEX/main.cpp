#include <Windows.h>
#include <iostream>
#include "GUI/GUI.h"

int WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	Fl_Double_Window* window = make_window_select_mode();
	window->show();
	const int res = Fl::run();
	return res;
}