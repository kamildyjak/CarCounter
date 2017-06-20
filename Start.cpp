#include "Start.h"

using namespace System;
using namespace System::Windows::Forms;

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main() {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	CarCounter::Start start;
	Application::Run(%start);

	return 0;
}