// reconstructionController.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace reconstructionController;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
    if (Thread::CurrentThread->GetApartmentState() != ApartmentState::STA)
    {
        CoUninitialize();
        CoInitialize(NULL);
    }

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew Form1());
	return 0;
}
