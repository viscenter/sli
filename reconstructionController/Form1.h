#pragma once
#include <cstdlib>
#include <iostream>

#include <signal.h>
#include <Windows.h>
#include "../common/cvCalibrateProCam.h"
#include "../common/cvScanProCam.h"
#include "../common/cvUtilProCam.h"
#include "../common/calibrationForm.h"
#include "../common/globals.h"

#define DEFAULT_PORT_NUM 7272
#define BUFFER_SIZE 1024
#define DEFAULT_BASE_FOLDER "X:/windowsDocuments/"
#define DEFAULT_SET_NAME "laserTests"
#define CONFIG_FILE "./config.xml"

using namespace std;

namespace reconstructionController {

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
			
			recon_ptrs = new reconPtrs();
			if(!loadSLConfigXML(sl_params, sl_calib))
				console->Text += "Could not load the Structured Light Configuration XML file!\r\nPlease correct the file and restart the program.\r\n";
			else
				console->Text += "Configuration File Read Successfully!\r\n";
			
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
				closesocket(sListen);
			}
			WSACleanup();
			if (components)
			{
				delete components;
			}
			
			cvReleaseMat(&sl_calib->cam_intrinsic);
			cvReleaseMat(&sl_calib->cam_distortion);
			cvReleaseMat(&sl_calib->cam_extrinsic);
			cvReleaseMat(&sl_calib->proj_intrinsic);
			cvReleaseMat(&sl_calib->proj_distortion);
			cvReleaseMat(&sl_calib->proj_extrinsic);
			cvReleaseMat(&sl_calib->cam_center);
			cvReleaseMat(&sl_calib->proj_center);
			cvReleaseMat(&sl_calib->cam_rays);
			cvReleaseMat(&sl_calib->proj_rays);
			cvReleaseMat(&sl_calib->proj_column_planes);
			cvReleaseMat(&sl_calib->proj_row_planes);
			cvReleaseMat(&sl_calib->background_depth_map);
			cvReleaseImage(&sl_calib->background_image);
			cvReleaseImage(&sl_calib->background_mask);
			
			delete sl_calib;
			delete sl_params;
			delete recon_ptrs;
		}

	private: System::Windows::Forms::Button^  connectBtn;


	private: int portNumber;
	private: System::String^ outMessage;
	private: BackgroundWorker^ helper;
	private: SOCKET sClient, sListen;
	private: bool running;
	private: DISPLAY_DEVICE* projectorDisplay;
	private: DEVMODE* projectorDefault;
	public: System::Windows::Forms::TextBox^  console;
	private: System::Windows::Forms::TextBox^  setNameBox;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;
	private: System::Windows::Forms::Button^  editBaseFolderBtn;

	private: System::Windows::Forms::Label^  baseFolderLocation;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  calibrationBtn;




	
	private: slParams* sl_params;
	private: slCalib* sl_calib;
	private: slData* sl_data;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::TextBox^  portBox;
private: reconPtrs* recon_ptrs;


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
			this->console = (gcnew System::Windows::Forms::TextBox());
			this->setNameBox = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->editBaseFolderBtn = (gcnew System::Windows::Forms::Button());
			this->baseFolderLocation = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->calibrationBtn = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->portBox = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// connectBtn
			// 
			this->connectBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->connectBtn->Location = System::Drawing::Point(405, 348);
			this->connectBtn->Name = L"connectBtn";
			this->connectBtn->Size = System::Drawing::Size(63, 23);
			this->connectBtn->TabIndex = 1;
			this->connectBtn->Text = L"Start";
			this->connectBtn->UseVisualStyleBackColor = true;
			this->connectBtn->Click += gcnew System::EventHandler(this, &Form1::connectBtn_Click);
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
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 320);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(60, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Listen Port:";
			// 
			// portBox
			// 
			this->portBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->portBox->Location = System::Drawing::Point(85, 317);
			this->portBox->Name = L"portBox";
			this->portBox->Size = System::Drawing::Size(38, 20);
			this->portBox->TabIndex = 3;
			this->portBox->Text = L"5555";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(480, 378);
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
			this->Text = L"Reconstruction Controller";
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

					sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if (sListen == INVALID_SOCKET)
					{
						sprintf(buffer, "socket() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}
					recon_ptrs->local.sin_addr.s_addr = htonl(INADDR_ANY);
					
					recon_ptrs->local.sin_family = AF_INET;
					recon_ptrs->local.sin_port = htons(portNumber);
									   
					if (bind(sListen, (struct sockaddr *)&recon_ptrs->local, sizeof(recon_ptrs->local)) == SOCKET_ERROR)
					{
						sprintf(buffer, "bind() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}
					listen(sListen, 8);

					console->Text += "Waiting for connection.\r\n";
					
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

					if (worker->CancellationPending)
					{
					   e->Cancel = true;
					   return;
					}

					int iAddrSize = sizeof(recon_ptrs->client);
					sClient = accept(sListen, (struct sockaddr *)&recon_ptrs->client,
									&iAddrSize);
					if (sClient == INVALID_SOCKET)
					{
						printf("accept() failed: %d\n", WSAGetLastError());
						return;
					}
					printf("Accepted client: %s:%d\n",
						inet_ntoa(recon_ptrs->client.sin_addr), ntohs(recon_ptrs->client.sin_port));
						
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
						
						outMessage += "Starting reconstruction...";
						worker->ReportProgress( 0 );

						HANDLE hThread;
						DWORD dwThreadId;
						hThread = CreateThread(NULL, 0, &reconstructionController::Form1::reconstructSurface, (LPVOID)this, 0, &dwThreadId);
						if (hThread == NULL)
						{
							sprintf(buffer, "CreateThread() failed: %d\n", GetLastError());
							outMessage += gcnew System::String(buffer);
							worker->ReportProgress( 0 );
						}
						else
							CloseHandle(hThread);
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

			int getLatestImages(IplImage**& imagesBuffer, int numImages)
			{
				return getImages(imagesBuffer, numImages, this->baseFolderLocation->Text + "/" + this->setNameBox->Text,
					this->setNameBox->Text + "_*.tif");
			}

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
				char* configFile = CONFIG_FILE;
				
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

