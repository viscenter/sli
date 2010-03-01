#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <cstdio>
#include <string>
#include <signal.h>
#include <Windows.h>
#include "gdiForm.h"

#define CLIENT_IP "127.0.0.1"
#define BUFFER_SIZE 1024

std::string gc2std(System::String^ s)
{
	using System::IntPtr;
	using System::Runtime::InteropServices::Marshal;

	IntPtr ip = Marshal::StringToHGlobalAnsi(s);
	std::string ss;
	try 
	{
		ss = static_cast<const char*>(ip.ToPointer());
	} catch (std::bad_alloc&) 
	{
		Marshal::FreeHGlobal(ip);
		throw;
	}
	Marshal::FreeHGlobal(ip);
	return ss;
}

using namespace std;

namespace projectorController {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			portNumber = 5555;
			helper = gcnew System::ComponentModel::BackgroundWorker();
			helper->DoWork += gcnew DoWorkEventHandler( this, &Form1::helper_DoWork );
			helper->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &Form1::helper_RunWorkerCompleted );
			helper->ProgressChanged += gcnew ProgressChangedEventHandler( this, &Form1::helper_ProgressChanged );
			helper->WorkerReportsProgress = true;
			helper->WorkerSupportsCancellation = true;
			running = false;

			projectorDisplay = new DISPLAY_DEVICE();
			projectorDefault = new DEVMODE();

			if(!getProjector())
			{
				console->Text += "Could not find the projector as a secondary display!\r\n";
			}

			WSADATA wsd;
			if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
			{
				console->Text += "Failed to load Winsock library!\r\n";
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if(helper->IsBusy)
			{
				helper->CancelAsync();
				closesocket(sClient);
			}
			WSACleanup();
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button^  connectBtn;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  portBox;
	private: int portNumber;
	private: System::String^ outMessage;
	private: BackgroundWorker^ helper;
	private: SOCKET sClient;
	private: bool running;
	private: DISPLAY_DEVICE* projectorDisplay;
	private: DEVMODE* projectorDefault;
	private: gdiForm^ patternForm;
	private: System::Windows::Forms::TextBox^  console;



	protected: 


	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->connectBtn = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->portBox = (gcnew System::Windows::Forms::TextBox());
			this->console = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// connectBtn
			// 
			this->connectBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->connectBtn->Location = System::Drawing::Point(391, 306);
			this->connectBtn->Name = L"connectBtn";
			this->connectBtn->Size = System::Drawing::Size(75, 23);
			this->connectBtn->TabIndex = 1;
			this->connectBtn->Text = L"Connect";
			this->connectBtn->UseVisualStyleBackColor = true;
			this->connectBtn->Click += gcnew System::EventHandler(this, &Form1::connectBtn_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 304);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(53, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"TCP Port:";
			// 
			// portBox
			// 
			this->portBox->Location = System::Drawing::Point(71, 301);
			this->portBox->Name = L"portBox";
			this->portBox->Size = System::Drawing::Size(38, 20);
			this->portBox->TabIndex = 3;
			this->portBox->Text = L"5555";
			// 
			// console
			// 
			this->console->AcceptsReturn = true;
			this->console->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->console->BackColor = System::Drawing::Color::Black;
			this->console->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->console->ForeColor = System::Drawing::Color::Chartreuse;
			this->console->Location = System::Drawing::Point(12, 12);
			this->console->Multiline = true;
			this->console->Name = L"console";
			this->console->ReadOnly = true;
			this->console->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->console->Size = System::Drawing::Size(454, 283);
			this->console->TabIndex = 1;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(478, 336);
			this->Controls->Add(this->console);
			this->Controls->Add(this->portBox);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->connectBtn);
			this->Name = L"Form1";
			this->Text = L"Projector Control";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void connectBtn_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				char buffer[BUFFER_SIZE];
				if(!running)
				{
					portNumber = atoi(gc2std(this->portBox->Text).c_str());
					struct sockaddr_in server;

					sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if (sClient == INVALID_SOCKET)
					{
						sprintf(buffer, "socket() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}
					server.sin_family = AF_INET;
					server.sin_port = htons(portNumber);
					server.sin_addr.s_addr = inet_addr(CLIENT_IP);
				   
					if (connect(sClient, (struct sockaddr *)&server,
						sizeof(server)) == SOCKET_ERROR)
					{
						sprintf(buffer, "connect() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}

					console->Text += "Connected...Waiting for signal.\r\n";
					
					helper->RunWorkerAsync();
					portBox->Enabled = false;
					running = true;
					connectBtn->Text = "Stop";
				}
				else
				{
					helper->CancelAsync();
					closesocket(sClient);
					portBox->Enabled = true;
					running = false;
					connectBtn->Text = "Connect";
					
				}
			 }

	 public: void helper_RunWorkerCompleted( Object^ /*sender*/, RunWorkerCompletedEventArgs^ e )
			 {
				 console->Text += "Closing Connection...";
			 }

	 public: void helper_ProgressChanged( Object^ /*sender*/, ProgressChangedEventArgs^ e )
			 {
				console->Text += outMessage;
				outMessage = "";
				console->SelectionStart = console->TextLength;
				console->ScrollToCaret();
			 }

	 public: void helper_DoWork(Object^ sender, DoWorkEventArgs^ e )
			 {
					BackgroundWorker^ worker = dynamic_cast<BackgroundWorker^>(sender);
					char buffer[BUFFER_SIZE];
					char szBuffer[BUFFER_SIZE];
					int ret;

					while(true)
					{
						if (worker->CancellationPending)
						{
						   e->Cancel = true;
						   return;
						}

						ret = recv(sClient, szBuffer, BUFFER_SIZE, 0);
						if (ret == 0)
							break;
						else if (ret == SOCKET_ERROR)
						{
							sprintf(buffer, "recv() failed: %d\r\n", WSAGetLastError());
							outMessage += gcnew System::String(buffer);
							worker->ReportProgress( 0 );
							break;
						}
						if(szBuffer[ret-1] == '\n')
							szBuffer[ret-1] = '\0';
						else
							szBuffer[ret] = '\0';
						
						sprintf(buffer, "Received [%d bytes]: '%s'\r\n", ret, szBuffer);
						outMessage += gcnew System::String(buffer);
						worker->ReportProgress( 0 );

						if(szBuffer[0] == 'V')
						{
							outMessage += sendMessage("UBW FW D Version 1.4.3.eqpi\r\n");
							worker->ReportProgress( 0 );
						}
						else if(szBuffer[0] == 'W')
						{
							outMessage += sendMessage("ProjectorClient\r\n");
							worker->ReportProgress( 0 );
						}
						else if(!strcmp(szBuffer, "O,0,0,0"))
						{
							outMessage += stopPattern();
							worker->ReportProgress( 0 );
						}
						else if(!strcmp(szBuffer, "O,64,0,0"))
						{
							projectorOff();
							outMessage += "Turning projector off...\r\n";
							worker->ReportProgress( 0 );
						}
						else if(!strcmp(szBuffer, "O,1,0,0"))
						{
							projectorOn();
							outMessage += "Turning projector on...\r\n";
							worker->ReportProgress( 0 );
						}
						else if(szBuffer[0] == 'O')
						{
							outMessage += startPattern(szBuffer);
							worker->ReportProgress( 0 );
						}
					}
					return;
			 }

			System::String^ sendMessage(char* message)
			{
				int ret;
				char buffer[BUFFER_SIZE];

				ret = send(sClient, message, strlen(message), 0);
				if (ret == 0)
					return "";
				else if (ret == SOCKET_ERROR)
				{
					sprintf(buffer, "send() failed: %d\r\n", WSAGetLastError());
					return gcnew System::String(buffer);
				}
				sprintf(buffer, "Sent [%d bytes]: '%s'\r\n", ret, message);
				return gcnew System::String(buffer);
			}
			
			System::String^ startPattern(char* message)
			{
				if(!patternForm)
					patternForm = gcnew gdiForm(projectorDefault);
				
				int patternNo = 1;
				
				if(!strcmp(message, "O,0,64,0"))
					patternNo = 1;
				else if(!strcmp(message, "O,0,128,0"))
					patternNo = 2;
				else if(!strcmp(message, "O,0,32,0"))
					patternNo = 3;
				else if(!strcmp(message, "O,32,0,0"))
					patternNo = 4;

				patternForm->showPattern(patternNo);
				return "Pattern Started\r\n";
			}

			System::String^ stopPattern()
			{
				if(!patternForm)
					patternForm = gcnew gdiForm(projectorDefault);
				patternForm->hidePattern();
				return "Pattern Stopped\r\n";
			}

			bool getProjector()
			{
				bool foundProjector = false;
				DWORD DispNum = 0;
				DISPLAY_DEVICE DisplayDevice;

				// initialize DisplayDevice
				ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
				DisplayDevice.cb = sizeof(DisplayDevice);

				// get all display devices
				while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
				{
					if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
						!(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
					{
						console->Text += "Projector found on!  Storing current settings...\r\n";
						ZeroMemory(projectorDefault, sizeof(DEVMODE));
						projectorDefault->dmSize = sizeof(DEVMODE);
						if (!EnumDisplaySettings(DisplayDevice.DeviceName, ENUM_REGISTRY_SETTINGS, projectorDefault))
						{
							console->Text += "Storing the current projector settings failed! Using defaults.\r\n";
						}
						else
						{
							ZeroMemory(projectorDefault, sizeof(DEVMODE));
							projectorDefault->dmSize = sizeof(DEVMODE);
							projectorDefault->dmFields = DM_POSITION;
							projectorDefault->dmPosition.x = 1440;
							projectorDefault->dmPosition.y = 0;
							projectorDefault->dmPelsWidth = 800;
							projectorDefault->dmPelsHeight = 600;
						}

						(*projectorDisplay) = DisplayDevice;
						//projectorOff();
						foundProjector = true;
						break;
					}
					else if (!(DisplayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) &&
						!(DisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
					{
						console->Text += "Projector found off!  Using default settings.\r\n";
						ZeroMemory(projectorDefault, sizeof(DEVMODE));
						projectorDefault->dmSize = sizeof(DEVMODE);
						projectorDefault->dmFields = DM_POSITION;
						projectorDefault->dmPosition.x = 1440;
						projectorDefault->dmPosition.y = 0;
						projectorDefault->dmPelsWidth = 800;
						projectorDefault->dmPelsHeight = 600;
						(*projectorDisplay) = DisplayDevice;
						foundProjector = true;
						projectorOn();
						break;
					}

					ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
					DisplayDevice.cb = sizeof(DisplayDevice);
					DispNum++;
				}

				return foundProjector;
			}

			void projectorOn()
			{
					ChangeDisplaySettingsEx(projectorDisplay->DeviceName, projectorDefault, NULL, CDS_UPDATEREGISTRY, NULL);
			}

			void projectorOff()
			{
					DEVMODE DevMode;
					ZeroMemory(&DevMode, sizeof(DevMode));
					DevMode.dmSize = sizeof(DevMode);
					DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_POSITION
							| DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS ;

					ChangeDisplaySettingsEx(projectorDisplay->DeviceName, &DevMode, NULL, CDS_UPDATEREGISTRY, NULL);
			}
	};
}

