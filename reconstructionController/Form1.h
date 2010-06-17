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
#define DEFAULT_SET_NAME ""
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
			recon_indir = new reconIndirect();

			if(!loadSLConfigXML(&recon_indir->data.sl_params, &recon_indir->data.sl_calib))
				console->Text += "Could not load the Structured Light Configuration XML file!\r\nPlease correct the file and restart the program.\r\n";
			else
				console->Text += "Configuration File Read Successfully!\r\n";
			
			this->outputDirLbl->Text = gcnew String(recon_indir->data.sl_params.outdir);
			this->baseFolderLocation->Text = DEFAULT_BASE_FOLDER;
			this->setNameBox->Text = DEFAULT_SET_NAME;
			this->portBox->Text = "" + DEFAULT_PORT_NUM;
			portNumber = DEFAULT_PORT_NUM;
			helper = gcnew System::ComponentModel::BackgroundWorker();
			helper->DoWork += gcnew DoWorkEventHandler( this, &Form1::helper_DoWork );
			helper->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &Form1::helper_RunWorkerCompleted );
			helper->ProgressChanged += gcnew ProgressChangedEventHandler( this, &Form1::helper_ProgressChanged );
			helper->WorkerReportsProgress = true;
			helper->WorkerSupportsCancellation = true;
			running = false;
			reconPattern = "";
			
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
			
			cvReleaseMat(&recon_indir->data.sl_calib.cam_intrinsic);
			cvReleaseMat(&recon_indir->data.sl_calib.cam_distortion);
			cvReleaseMat(&recon_indir->data.sl_calib.cam_extrinsic);
			cvReleaseMat(&recon_indir->data.sl_calib.proj_intrinsic);
			cvReleaseMat(&recon_indir->data.sl_calib.proj_distortion);
			cvReleaseMat(&recon_indir->data.sl_calib.proj_extrinsic);
			cvReleaseMat(&recon_indir->data.sl_calib.cam_center);
			cvReleaseMat(&recon_indir->data.sl_calib.proj_center);
			cvReleaseMat(&recon_indir->data.sl_calib.cam_rays);
			cvReleaseMat(&recon_indir->data.sl_calib.proj_rays);
			cvReleaseMat(&recon_indir->data.sl_calib.proj_column_planes);
			cvReleaseMat(&recon_indir->data.sl_calib.proj_row_planes);
			cvReleaseMat(&recon_indir->data.sl_calib.background_depth_map);
			cvReleaseImage(&recon_indir->data.sl_calib.background_image);
			cvReleaseImage(&recon_indir->data.sl_calib.background_mask);
			
			delete recon_indir;
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




	

	public: reconIndirect* recon_indir;

private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::TextBox^  portBox;
private: reconPtrs* recon_ptrs;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::TextBox^  tileBox;
private: System::Windows::Forms::Button^  manualReconBtn;
public: String^ reconPattern;
private: System::Windows::Forms::Label^  label4;
public: 
private: System::Windows::Forms::Label^  outputDirLbl;
private: System::Windows::Forms::Button^  editOutputDirBtn;


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
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->tileBox = (gcnew System::Windows::Forms::TextBox());
			this->manualReconBtn = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->outputDirLbl = (gcnew System::Windows::Forms::Label());
			this->editOutputDirBtn = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// connectBtn
			// 
			this->connectBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->connectBtn->Location = System::Drawing::Point(406, 397);
			this->connectBtn->Name = L"connectBtn";
			this->connectBtn->Size = System::Drawing::Size(63, 23);
			this->connectBtn->TabIndex = 1;
			this->connectBtn->Text = L"Listen";
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
			this->console->Size = System::Drawing::Size(457, 260);
			this->console->TabIndex = 1;
			// 
			// setNameBox
			// 
			this->setNameBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->setNameBox->Location = System::Drawing::Point(77, 345);
			this->setNameBox->Name = L"setNameBox";
			this->setNameBox->Size = System::Drawing::Size(139, 20);
			this->setNameBox->TabIndex = 40;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 348);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(57, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Set Name:";
			// 
			// editBaseFolderBtn
			// 
			this->editBaseFolderBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->editBaseFolderBtn->Location = System::Drawing::Point(130, 311);
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
			this->baseFolderLocation->Location = System::Drawing::Point(195, 316);
			this->baseFolderLocation->Name = L"baseFolderLocation";
			this->baseFolderLocation->Size = System::Drawing::Size(272, 18);
			this->baseFolderLocation->TabIndex = 7;
			this->baseFolderLocation->Text = L"./";
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(13, 316);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(111, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Base Image Directory:";
			// 
			// calibrationBtn
			// 
			this->calibrationBtn->Location = System::Drawing::Point(361, 366);
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
			this->label1->Location = System::Drawing::Point(276, 402);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(60, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Listen Port:";
			// 
			// portBox
			// 
			this->portBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->portBox->Location = System::Drawing::Point(349, 399);
			this->portBox->Name = L"portBox";
			this->portBox->Size = System::Drawing::Size(38, 20);
			this->portBox->TabIndex = 3;
			this->portBox->Text = L"5555";
			// 
			// label12
			// 
			this->label12->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(13, 376);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(58, 13);
			this->label12->TabIndex = 26;
			this->label12->Text = L"Tile Name:";
			// 
			// tileBox
			// 
			this->tileBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->tileBox->Location = System::Drawing::Point(77, 373);
			this->tileBox->Name = L"tileBox";
			this->tileBox->Size = System::Drawing::Size(138, 20);
			this->tileBox->TabIndex = 41;
			// 
			// manualReconBtn
			// 
			this->manualReconBtn->Location = System::Drawing::Point(15, 399);
			this->manualReconBtn->Name = L"manualReconBtn";
			this->manualReconBtn->Size = System::Drawing::Size(121, 23);
			this->manualReconBtn->TabIndex = 27;
			this->manualReconBtn->Text = L"Manual Reconstruct";
			this->manualReconBtn->UseVisualStyleBackColor = true;
			this->manualReconBtn->Click += gcnew System::EventHandler(this, &Form1::manualReconBtn_Click);
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(13, 284);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(87, 13);
			this->label4->TabIndex = 44;
			this->label4->Text = L"Output Directory:";
			// 
			// outputDirLbl
			// 
			this->outputDirLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->outputDirLbl->AutoEllipsis = true;
			this->outputDirLbl->Location = System::Drawing::Point(195, 284);
			this->outputDirLbl->Name = L"outputDirLbl";
			this->outputDirLbl->Size = System::Drawing::Size(272, 18);
			this->outputDirLbl->TabIndex = 43;
			this->outputDirLbl->Text = L"./";
			// 
			// editOutputDirBtn
			// 
			this->editOutputDirBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->editOutputDirBtn->Location = System::Drawing::Point(130, 279);
			this->editOutputDirBtn->Name = L"editOutputDirBtn";
			this->editOutputDirBtn->Size = System::Drawing::Size(59, 23);
			this->editOutputDirBtn->TabIndex = 42;
			this->editOutputDirBtn->Text = L"Edit";
			this->editOutputDirBtn->UseVisualStyleBackColor = true;
			this->editOutputDirBtn->Click += gcnew System::EventHandler(this, &Form1::editOutputDirBtn_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(481, 432);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->outputDirLbl);
			this->Controls->Add(this->editOutputDirBtn);
			this->Controls->Add(this->manualReconBtn);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->tileBox);
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
					//this->setNameBox->Enabled = false;
					//this->tileBox->Enabled = false;
					//this->manualReconBtn->Enabled = false;
					//this->calibrationBtn->Enabled = false;
					running = true;
					connectBtn->Text = "Stop";
				}
				else
				{
					helper->CancelAsync();
					closesocket(sClient);
					closesocket(sListen);
					portBox->Enabled = true;
					//this->setNameBox->Enabled = true;
					//this->tileBox->Enabled = true;
					//this->manualReconBtn->Enabled = true;
					//this->calibrationBtn->Enabled = true;
					running = false;
					connectBtn->Text = "Listen";
					
				}
			 }

	 public: void helper_RunWorkerCompleted( Object^ /*sender*/, RunWorkerCompletedEventArgs^ e )
			 {
				 console->Text += "Closing Connection...";
				 console->SelectionStart = console->TextLength;
				 console->ScrollToCaret();
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
						sprintf(buffer, "accept() failed: %d\r\n", WSAGetLastError());
						outMessage += gcnew System::String(buffer);
						worker->ReportProgress( 0 );
						return;
					}
					sprintf(buffer, "Accepted client: %s:%d\r\n",
						inet_ntoa(recon_ptrs->client.sin_addr), ntohs(recon_ptrs->client.sin_port));
						outMessage += gcnew System::String(buffer);
							worker->ReportProgress( 0 );
						
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
						
						sprintf(buffer, "Received: '%s'\r\n", szBuffer);
						outMessage += gcnew System::String(buffer);
						worker->ReportProgress( 0 );
						
						outMessage += "Starting reconstruction...\r\n";
						worker->ReportProgress( 0 );

						for(int i=0; i<4; i++)
						{
							Sleep(10000);
							int numImages = checkImages(this->baseFolderLocation->Text+"/"+gcnew System::String(szBuffer)+"/Processed", gcnew System::String(szBuffer)+"_*.tif");
							if(numImages >= 22)
							{
								this->reconPattern = gcnew System::String(szBuffer);
								outMessage += "Found " + numImages + " images of " + reconPattern + "\r\n";
								outMessage += "Starting reconstruction...\r\n";
								worker->ReportProgress( 0 );
								Thread^ newThread = gcnew Thread(gcnew ParameterizedThreadStart(reconstructSurface));
								newThread->Start(this);
								break;
							}
							else
							{
								outMessage += "Only " + numImages + " images were found.\r\n";
								outMessage += "22 are needed for reconstruction.\r\n";
								if(i<4)
									outMessage += "Trying again in 10 seconds...\r\n";
								else
									outMessage += "Aborting reconstruction...\r\n";
								worker->ReportProgress( 0 );
							}
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

			int getLatestImages(IplImage**& imagesBuffer, int numImages)
			{
				return getImages2(imagesBuffer, numImages, this->baseFolderLocation->Text+"/"+this->reconPattern+"/Processed", this->reconPattern + "_*.tif");
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
				calibrationForm^ calibrationWindow = gcnew calibrationForm(&recon_indir->data.sl_params, &recon_indir->data.sl_calib);
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
					readConfiguration(configFile, &recon_indir->data.sl_params);
				}
				else{
					return false;
				}

				// Allocate storage for calibration parameters.
				int cam_nelems                  = recon_indir->data.sl_params.cam_w*recon_indir->data.sl_params.cam_h;
				int proj_nelems                 = recon_indir->data.sl_params.proj_w*recon_indir->data.sl_params.proj_h;
				recon_indir->data.sl_calib.cam_intrinsic_calib    = false;
				recon_indir->data.sl_calib.proj_intrinsic_calib   = false;
				recon_indir->data.sl_calib.procam_extrinsic_calib = false;
				recon_indir->data.sl_calib.cam_intrinsic          = cvCreateMat(3,3,CV_32FC1);
				recon_indir->data.sl_calib.cam_distortion         = cvCreateMat(5,1,CV_32FC1);
				recon_indir->data.sl_calib.cam_extrinsic          = cvCreateMat(2, 3, CV_32FC1);
				recon_indir->data.sl_calib.proj_intrinsic         = cvCreateMat(3, 3, CV_32FC1);
				recon_indir->data.sl_calib.proj_distortion        = cvCreateMat(5, 1, CV_32FC1);
				recon_indir->data.sl_calib.proj_extrinsic         = cvCreateMat(2, 3, CV_32FC1);
				recon_indir->data.sl_calib.cam_center             = cvCreateMat(3, 1, CV_32FC1);
				recon_indir->data.sl_calib.proj_center            = cvCreateMat(3, 1, CV_32FC1);
				recon_indir->data.sl_calib.cam_rays               = cvCreateMat(3, cam_nelems, CV_32FC1);
				recon_indir->data.sl_calib.proj_rays              = cvCreateMat(3, proj_nelems, CV_32FC1);
				recon_indir->data.sl_calib.proj_column_planes     = cvCreateMat(recon_indir->data.sl_params.proj_w, 4, CV_32FC1);
				recon_indir->data.sl_calib.proj_row_planes        = cvCreateMat(recon_indir->data.sl_params.proj_h, 4, CV_32FC1);
				
				// Load intrinsic camera calibration parameters (if found).
				char str1[1024], str2[1024];
				sprintf(str1, "%s\\calib\\cam\\cam_intrinsic.xml",  recon_indir->data.sl_params.outdir);
				sprintf(str2, "%s\\calib\\cam\\cam_distortion.xml", recon_indir->data.sl_params.outdir);
				if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
					recon_indir->data.sl_calib.cam_intrinsic  = (CvMat*)cvLoad(str1);
					recon_indir->data.sl_calib.cam_distortion = (CvMat*)cvLoad(str2);
					recon_indir->data.sl_calib.cam_intrinsic_calib = true;
					console->Text += "Loaded previous intrinsic camera calibration.\r\n";
				}

				// Load intrinsic projector calibration parameters (if found);
				sprintf(str1, "%s\\calib\\proj\\proj_intrinsic.xml",  recon_indir->data.sl_params.outdir);
				sprintf(str2, "%s\\calib\\proj\\proj_distortion.xml", recon_indir->data.sl_params.outdir);
				if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
					recon_indir->data.sl_calib.proj_intrinsic  = (CvMat*)cvLoad(str1);
					recon_indir->data.sl_calib.proj_distortion = (CvMat*)cvLoad(str2);
					recon_indir->data.sl_calib.proj_intrinsic_calib = true;
					console->Text += "Loaded previous intrinsic projector calibration.\r\n";
				}
				
				// Load extrinsic projector-camera parameters (if found).
				sprintf(str1, "%s\\calib\\proj\\cam_extrinsic.xml",  recon_indir->data.sl_params.outdir);
				sprintf(str2, "%s\\calib\\proj\\proj_extrinsic.xml", recon_indir->data.sl_params.outdir);
				if( (recon_indir->data.sl_calib.cam_intrinsic_calib && recon_indir->data.sl_calib.proj_intrinsic_calib) &&
					( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ) ){
					recon_indir->data.sl_calib.cam_extrinsic  = (CvMat*)cvLoad(str1);
					recon_indir->data.sl_calib.proj_extrinsic = (CvMat*)cvLoad(str2);
					recon_indir->data.sl_calib.procam_extrinsic_calib = true;
					evaluateProCamGeometry(&recon_indir->data.sl_params, &recon_indir->data.sl_calib);
					console->Text += "Loaded previous extrinsic projector-camera calibration.\r\n";
				}
				
				// Initialize background model.
				recon_indir->data.sl_calib.background_depth_map = cvCreateMat(recon_indir->data.sl_params.cam_h, recon_indir->data.sl_params.cam_w, CV_32FC1);
				recon_indir->data.sl_calib.background_image     = cvCreateImage(cvSize(recon_indir->data.sl_params.cam_w, recon_indir->data.sl_params.cam_h), IPL_DEPTH_8U, 3);
				recon_indir->data.sl_calib.background_mask      = cvCreateImage(cvSize(recon_indir->data.sl_params.cam_w, recon_indir->data.sl_params.cam_h), IPL_DEPTH_8U, 1);
				cvSet(recon_indir->data.sl_calib.background_depth_map, cvScalar(FLT_MAX));
				cvZero(recon_indir->data.sl_calib.background_image);
				cvSet(recon_indir->data.sl_calib.background_mask, cvScalar(255));
				
				return true;
			}

			static void reconstructSurface(System::Object^ myForm)
			{
				reconstructionController::Form1^ theForm = (reconstructionController::Form1^)myForm;
				string baseName = gc2std(theForm->reconPattern);	
				slParams* sl_params = &theForm->recon_indir->data.sl_params;
				slCalib* sl_calib = &theForm->recon_indir->data.sl_calib;
				
				IplImage** proj_gray_codes = NULL;
				int gray_ncols, gray_nrows;
				int gray_colshift, gray_rowshift;
				
				generateGrayCodes(sl_params->proj_w, sl_params->proj_h, proj_gray_codes, 
				gray_ncols, gray_nrows, gray_colshift, gray_rowshift, 
				sl_params->scan_cols, sl_params->scan_rows);

				IplImage** cam_gray_codes;
				int numImages = theForm->getLatestImages(cam_gray_codes, 22);

				IplImage* gray_decoded_cols = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_16U, 1);
				IplImage* gray_decoded_rows = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_16U, 1);
				IplImage* gray_mask         = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U,  1);
				
				decodeGrayCodes(sl_params->proj_w, sl_params->proj_h,
								cam_gray_codes, 
								gray_decoded_cols, gray_decoded_rows, gray_mask,
								gray_ncols, gray_nrows, 
								gray_colshift, gray_rowshift, 
								sl_params->thresh);
				
				char str[1024], outputDir[1024];
				sprintf(outputDir, "%s\\%s", sl_params->outdir, baseName.c_str());
				_mkdir(outputDir);

				// Display and save the correspondences.
				if(sl_params->display)
					displayDecodingResults(gray_decoded_cols, gray_decoded_rows, gray_mask, sl_params);

				// Reconstruct the point cloud and depth map.
				//printf("Reconstructing the point cloud and the depth map...\n");
				CvMat *points    = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
				CvMat *colors    = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
				CvMat *depth_map = cvCreateMat(sl_params->cam_h, sl_params->cam_w, CV_32FC1);
				CvMat *mask      = cvCreateMat(1, sl_params->cam_h*sl_params->cam_w, CV_32FC1);

				CvMat *resampled_points = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);

				reconstructStructuredLight(sl_params, sl_calib, 
										   cam_gray_codes[0],
										   gray_decoded_cols, gray_decoded_rows, gray_mask,
										   points, colors, depth_map, mask);
				
				downsamplePoints(sl_params, sl_calib, points, mask, resampled_points, depth_map);

				double min_val, max_val;
				cvMinMaxLoc(depth_map, &min_val, &max_val);
				
				// Display and save the depth map.
				if(sl_params->display)
					displayDepthMap(depth_map, gray_mask, sl_params);

					//printf("Saving the depth map...\n");
				IplImage* depth_map_image = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 1);
				for(int r=0; r<sl_params->cam_h; r++){
					for(int c=0; c<sl_params->cam_w; c++){
						char* depth_map_image_data = (char*)(depth_map_image->imageData + r*depth_map_image->widthStep);
						if(mask->data.fl[sl_params->cam_w*r+c])
							depth_map_image_data[c] = 
								255-int(255*(depth_map->data.fl[sl_params->cam_w*r+c]-sl_params->dist_range[0])/
									(sl_params->dist_range[1]-sl_params->dist_range[0]));
						else
							depth_map_image_data[c] = 0;
					}
				}
				CvMat* dist_range = cvCreateMat(1, 2, CV_32FC1);
				cvmSet(dist_range, 0, 0, sl_params->dist_range[0]);
				cvmSet(dist_range, 0, 1, sl_params->dist_range[1]);

				sprintf(str, "%s\\depth_map.png", outputDir);
				cvSaveImage(str, depth_map_image);
				sprintf(str, "%s\\depth_map_range.xml", outputDir);
				cvSave(str, dist_range);
				cvReleaseImage(&depth_map_image);
				cvReleaseMat(&dist_range);


				// Save the texture map.
				//printf("Saving the texture map...\n");
				sprintf(str, "%s\\%s.png", outputDir, baseName.c_str());
				cvSaveImage(str, cam_gray_codes[0]);

				// Save the point cloud.
				//printf("Saving the point cloud...\n");
				sprintf(str, "%s\\%s.ply", outputDir, baseName.c_str());
				if(savePointsPLY(str, resampled_points, NULL, NULL, mask)){
					MessageBox::Show("Saving the reconstructed point cloud failed!", "Reconstruction Error", 
						MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
					return;
				}

				sprintf(str,"%s\\proj_intrinsic.xml", outputDir);	
				cvSave(str, sl_calib->proj_intrinsic);
				sprintf(str,"%s\\proj_distortion.xml", outputDir);
				cvSave(str, sl_calib->proj_distortion);
				sprintf(str,"%s\\cam_intrinsic.xml", outputDir);	
				cvSave(str, sl_calib->cam_intrinsic);
				sprintf(str,"%s\\cam_distortion.xml", outputDir);
				cvSave(str, sl_calib->cam_distortion);
				sprintf(str, "%s\\cam_extrinsic.xml", outputDir);
				cvSave(str, sl_calib->cam_extrinsic);
				sprintf(str, "%s\\proj_extrinsic.xml", outputDir);
				cvSave(str, sl_calib->proj_extrinsic);


				// Free allocated resources.
				cvReleaseImage(&gray_decoded_cols);
				cvReleaseImage(&gray_decoded_rows);
				cvReleaseImage(&gray_mask);
				cvReleaseMat(&points);
				cvReleaseMat(&colors);
				cvReleaseMat(&depth_map);
				cvReleaseMat(&mask);
				for(int i=0; i<(gray_ncols+gray_nrows+1); i++)
					cvReleaseImage(&proj_gray_codes[i]);
				delete[] proj_gray_codes;
				for(int i=0; i<2*(gray_ncols+gray_nrows+1); i++)
					cvReleaseImage(&cam_gray_codes[i]);
				delete[] cam_gray_codes;

				return;
			}
private: System::Void manualReconBtn_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 int numImages = checkImages(this->baseFolderLocation->Text+"/"+this->setNameBox->Text + this->tileBox->Text+"/Processed", this->setNameBox->Text + this->tileBox->Text + "_*.tif");
			 if(numImages >= 22)
			 {
				 this->reconPattern = this->setNameBox->Text + this->tileBox->Text;
				 this->console->Text += "Found " + numImages + " images of " + reconPattern + "\r\n";
				 this->console->Text += "Starting reconstruction...\r\n";
				 console->SelectionStart = console->TextLength;
				 console->ScrollToCaret();
				 Thread^ newThread = gcnew Thread(gcnew ParameterizedThreadStart(reconstructSurface));
				 newThread->Start(this);
			 }
			 else
			 {
				 this->console->Text += "Only " + numImages + " images were found.\r\n";
				 this->console->Text += "22 are needed for construction.\r\n";
				 console->SelectionStart = console->TextLength;
				 console->ScrollToCaret();
			 }

		 }
private: System::Void editOutputDirBtn_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
				System::Windows::Forms::DialogResult result = folderBrowserDialog1->ShowDialog();
				if ( result == ::DialogResult::OK )
				{
					this->outputDirLbl->Text = folderBrowserDialog1->SelectedPath;
					strcpy(recon_indir->data.sl_params.outdir, gc2std(folderBrowserDialog1->SelectedPath).c_str());
					
				}
		 }
};
}

