#pragma once
#include <cstdlib>
#include <iostream>

#include <signal.h>
#include <Windows.h>
#include "gdiForm.h"
#include "cvStructuredLight.h"
#include "cvCalibrateProCam.h"
#include "cvScanProCam.h"
#include "cvUtilProCam.h"
#include "calibrationForm.h"
#include "globals.h"

#define CLIENT_IP "127.0.0.1"
#define DEFAULT_PORT_NUM 5555
#define BUFFER_SIZE 1024
#define DEFAULT_BASE_FOLDER "X:/windowsDocuments/"
#define DEFAULT_SET_NAME "laserTests"

using namespace std;

namespace projectorController {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace System::IO;

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
			sl_params = new slParams();
			sl_calib = new slCalib();

			if(!loadSLConfigXML(sl_params, sl_calib))
				console->Text += "Could not load the Structured Light Configuration XML file!\r\n";

			this->ipBox->Text = CLIENT_IP;
			this->baseFolderLocation->Text = DEFAULT_BASE_FOLDER;
			this->setNameBox->Text = DEFAULT_SET_NAME;
			portNumber = DEFAULT_PORT_NUM;
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
	public: System::Windows::Forms::TextBox^  console;
	private: System::Windows::Forms::TextBox^  setNameBox;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;
	private: System::Windows::Forms::Button^  editBaseFolderBtn;

	private: System::Windows::Forms::Label^  baseFolderLocation;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  calibrationBtn;

	private: System::Windows::Forms::TextBox^  ipBox;

	private: System::Windows::Forms::Label^  label4;
	private: slParams* sl_params;
	private: slCalib* sl_calib;


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
			this->setNameBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->editBaseFolderBtn = (gcnew System::Windows::Forms::Button());
			this->baseFolderLocation = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->calibrationBtn = (gcnew System::Windows::Forms::Button());
			this->ipBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// connectBtn
			// 
			this->connectBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->connectBtn->Location = System::Drawing::Point(393, 348);
			this->connectBtn->Name = L"connectBtn";
			this->connectBtn->Size = System::Drawing::Size(75, 23);
			this->connectBtn->TabIndex = 1;
			this->connectBtn->Text = L"Connect";
			this->connectBtn->UseVisualStyleBackColor = true;
			this->connectBtn->Click += gcnew System::EventHandler(this, &Form1::connectBtn_Click);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 320);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(53, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"TCP Port:";
			// 
			// portBox
			// 
			this->portBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->portBox->Location = System::Drawing::Point(71, 317);
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
			this->console->Cursor = System::Windows::Forms::Cursors::Default;
			this->console->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->console->ForeColor = System::Drawing::Color::Chartreuse;
			this->console->Location = System::Drawing::Point(12, 12);
			this->console->Multiline = true;
			this->console->Name = L"console";
			this->console->ReadOnly = true;
			this->console->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->console->Size = System::Drawing::Size(456, 265);
			this->console->TabIndex = 1;
			// 
			// setNameBox
			// 
			this->setNameBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->setNameBox->Location = System::Drawing::Point(330, 317);
			this->setNameBox->Name = L"setNameBox";
			this->setNameBox->Size = System::Drawing::Size(139, 20);
			this->setNameBox->TabIndex = 4;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(267, 320);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(57, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Set Name:";
			// 
			// editBaseFolderBtn
			// 
			this->editBaseFolderBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->editBaseFolderBtn->Location = System::Drawing::Point(129, 286);
			this->editBaseFolderBtn->Name = L"editBaseFolderBtn";
			this->editBaseFolderBtn->Size = System::Drawing::Size(59, 23);
			this->editBaseFolderBtn->TabIndex = 6;
			this->editBaseFolderBtn->Text = L"Edit";
			this->editBaseFolderBtn->UseVisualStyleBackColor = true;
			this->editBaseFolderBtn->Click += gcnew System::EventHandler(this, &Form1::editBaseFolderBtn_Click);
			// 
			// baseFolderLocation
			// 
			this->baseFolderLocation->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->baseFolderLocation->AutoEllipsis = true;
			this->baseFolderLocation->Location = System::Drawing::Point(194, 291);
			this->baseFolderLocation->Name = L"baseFolderLocation";
			this->baseFolderLocation->Size = System::Drawing::Size(272, 18);
			this->baseFolderLocation->TabIndex = 7;
			this->baseFolderLocation->Text = L"./";
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(12, 291);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(111, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Base Image Directory:";
			// 
			// calibrationBtn
			// 
			this->calibrationBtn->Location = System::Drawing::Point(15, 348);
			this->calibrationBtn->Name = L"calibrationBtn";
			this->calibrationBtn->Size = System::Drawing::Size(108, 23);
			this->calibrationBtn->TabIndex = 9;
			this->calibrationBtn->Text = L"Calibration Controls";
			this->calibrationBtn->UseVisualStyleBackColor = true;
			this->calibrationBtn->Click += gcnew System::EventHandler(this, &Form1::calibrationBtn_Click);
			// 
			// ipBox
			// 
			this->ipBox->Location = System::Drawing::Point(152, 317);
			this->ipBox->Name = L"ipBox";
			this->ipBox->Size = System::Drawing::Size(100, 20);
			this->ipBox->TabIndex = 10;
			this->ipBox->Text = L"127.0.0.1";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(126, 320);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(20, 13);
			this->label4->TabIndex = 11;
			this->label4->Text = L"IP:";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(480, 378);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->ipBox);
			this->Controls->Add(this->calibrationBtn);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->baseFolderLocation);
			this->Controls->Add(this->editBaseFolderBtn);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->setNameBox);
			this->Controls->Add(this->console);
			this->Controls->Add(this->portBox);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->connectBtn);
			this->Name = L"Form1";
			this->Text = L"Structured Light Control";
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
					server.sin_addr.s_addr = inet_addr(gc2std(this->ipBox->Text).c_str());
				   
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

			int getImages(IplImage**& imagesBuffer, int numImages, String^ dirLocation, String^ filePattern)
			{
				DirectoryInfo^ dir = gcnew DirectoryInfo(dirLocation);
				if(!dir->Exists)
				{
					outMessage += dirLocation + " does not exist!\r\n";
					return 0;
				}

				array<FileInfo^>^ files = dir->GetFiles(filePattern);
				files->Sort(files, gcnew FileInfoNameComparer());
				
				numImages = (numImages < files->Length)?numImages:files->Length;
				if(numImages <= 0)
					return 0;

				imagesBuffer = new IplImage* [numImages];
				for(int i=0; i<numImages; i++)
					imagesBuffer[i] = cvLoadImage(gc2std(files[0]->FullName).c_str());
				
				return numImages;
				/*cvNamedWindow("test",1);
				cvShowImage("test", myImage);

				cvWaitKey();
				cvDestroyWindow("test");
				cvReleaseImage(&myImage);*/
			}

			int getLatestImages(IplImage**& imagesBuffer, int numImages)
			{
				return getImages(imagesBuffer, numImages, this->baseFolderLocation->Text + "/" + this->setNameBox->Text,
					this->setNameBox->Text + "_*.tif");
			}

			//Compares FileInfo objects backwards to produce a list where the biggest
			//numbered images are first.
			public: ref class FileInfoNameComparer : IComparer
			{
				public: virtual int Compare(Object^ x, Object^ y)
				{
					FileInfo^ objX = (FileInfo^)x;
					FileInfo^ objY = (FileInfo^)y;
					return objX->Name->CompareTo(objY->Name)* -1;
				}
			};
			
			private: System::Void editBaseFolderBtn_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				System::Windows::Forms::DialogResult result = folderBrowserDialog1->ShowDialog();
				if ( result == ::DialogResult::OK )
				{
					this->baseFolderLocation->Text = folderBrowserDialog1->SelectedPath;
				}
			}
			
			private: System::Void calibrationBtn_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				calibrationForm^ calibrationWindow = gcnew calibrationForm(sl_params, sl_calib);
				calibrationWindow->Visible = true;
			}
			
			private: bool loadSLConfigXML(slParams* sl_params, slCalib* sl_calib)
			{
				char* configFile = "./config.xml";
				
				// Read structured lighting parameters from configuration file.
				FILE* pFile = fopen(configFile, "r");
				if(pFile != NULL){
					fclose(pFile);
					console->Text += "Reading configuration file...\r\n";
					readConfiguration(configFile, sl_params);
				}
				else{
					return false;
				}

				// Create fullscreen window (for controlling projector display).
				cvNamedWindow("projWindow", CV_WINDOW_AUTOSIZE);
				IplImage* proj_frame = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 3);
				cvSet(proj_frame, cvScalar(0, 0, 0));
				cvShowImage("projWindow", proj_frame);
				cvMoveWindow("projWindow", -sl_params->proj_w-7, -33);
				cvWaitKey(1);
				
				// Allocate storage for calibration parameters.
				int cam_nelems                  = sl_params->cam_w*sl_params->cam_h;
				int proj_nelems                 = sl_params->proj_w*sl_params->proj_h;
				sl_calib->cam_intrinsic_calib    = false;
				sl_calib->proj_intrinsic_calib   = false;
				sl_calib->procam_extrinsic_calib = false;
				sl_calib->cam_intrinsic          = cvCreateMat(3,3,CV_32FC1);
				sl_calib->cam_distortion         = cvCreateMat(5,1,CV_32FC1);
				sl_calib->cam_extrinsic          = cvCreateMat(2, 3, CV_32FC1);
				sl_calib->proj_intrinsic         = cvCreateMat(3, 3, CV_32FC1);
				sl_calib->proj_distortion        = cvCreateMat(5, 1, CV_32FC1);
				sl_calib->proj_extrinsic         = cvCreateMat(2, 3, CV_32FC1);
				sl_calib->cam_center             = cvCreateMat(3, 1, CV_32FC1);
				sl_calib->proj_center            = cvCreateMat(3, 1, CV_32FC1);
				sl_calib->cam_rays               = cvCreateMat(3, cam_nelems, CV_32FC1);
				sl_calib->proj_rays              = cvCreateMat(3, proj_nelems, CV_32FC1);
				sl_calib->proj_column_planes     = cvCreateMat(sl_params->proj_w, 4, CV_32FC1);
				sl_calib->proj_row_planes        = cvCreateMat(sl_params->proj_h, 4, CV_32FC1);
				
				// Load intrinsic camera calibration parameters (if found).
				char str1[1024], str2[1024];
				sprintf(str1, "%s\\calib\\cam\\cam_intrinsic.xml",  sl_params->outdir);
				sprintf(str2, "%s\\calib\\cam\\cam_distortion.xml", sl_params->outdir);
				if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
					sl_calib->cam_intrinsic  = (CvMat*)cvLoad(str1);
					sl_calib->cam_distortion = (CvMat*)cvLoad(str2);
					sl_calib->cam_intrinsic_calib = true;
					console->Text += "Loaded previous intrinsic camera calibration.\r\n";
				}

				// Load intrinsic projector calibration parameters (if found);
				sprintf(str1, "%s\\calib\\proj\\proj_intrinsic.xml",  sl_params->outdir);
				sprintf(str2, "%s\\calib\\proj\\proj_distortion.xml", sl_params->outdir);
				if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
					sl_calib->proj_intrinsic  = (CvMat*)cvLoad(str1);
					sl_calib->proj_distortion = (CvMat*)cvLoad(str2);
					sl_calib->proj_intrinsic_calib = true;
					console->Text += "Loaded previous intrinsic projector calibration.\r\n";
				}
				
				// Load extrinsic projector-camera parameters (if found).
				sprintf(str1, "%s\\calib\\proj\\cam_extrinsic.xml",  sl_params->outdir);
				sprintf(str2, "%s\\calib\\proj\\proj_extrinsic.xml", sl_params->outdir);
				if( (sl_calib->cam_intrinsic_calib && sl_calib->proj_intrinsic_calib) &&
					( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ) ){
					sl_calib->cam_extrinsic  = (CvMat*)cvLoad(str1);
					sl_calib->proj_extrinsic = (CvMat*)cvLoad(str2);
					sl_calib->procam_extrinsic_calib = true;
					evaluateProCamGeometry(sl_params, sl_calib);
					console->Text += "Loaded previous extrinsic projector-camera calibration.\r\n";
				}
				
				// Initialize background model.
				sl_calib->background_depth_map = cvCreateMat(sl_params->cam_h, sl_params->cam_w, CV_32FC1);
				sl_calib->background_image     = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 3);
				sl_calib->background_mask      = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 1);
				cvSet(sl_calib->background_depth_map, cvScalar(FLT_MAX));
				cvZero(sl_calib->background_image);
				cvSet(sl_calib->background_mask, cvScalar(255));
				
				return true;
			}
};
}
