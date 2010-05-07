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

#define PHOTOSHOOT_IP "127.0.0.1"
#define PHOTOSHOOT_PORT 5555
#define PROJECTOR_IP "127.0.0.1"
#define PROJECTOR_PORT 6464
#define RECON_IP "127.0.0.1"
#define RECON_PORT 7272

#define BUFFER_SIZE 1024
#define DEFAULT_BASE_FOLDER "X:/windowsDocuments/"
#define DEFAULT_SET_NAME "laserTests"
#define CONFIG_FILE "./config.xml"

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
	using namespace reconstructionController;

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
			sl_data = new slData();

			if(!loadSLConfigXML(sl_params, sl_calib))
				console->Text += "Could not load the Structured Light Configuration XML file!\r\nPlease correct the file and restart the program.\r\n";
			else
				console->Text += "Configuration File Read Successfully!\r\n";
			
			sl_data->proj_chessboard = NULL;
			sl_data->proj_gray_codes = NULL;
			this->ipBox->Text = PHOTOSHOOT_IP;
			this->baseFolderLocation->Text = DEFAULT_BASE_FOLDER;
			this->setNameBox->Text = DEFAULT_SET_NAME;
			this->proj_ip_box->Text = PROJECTOR_IP;
			this->proj_port_box->Text = "" + PROJECTOR_PORT;
			this->recon_ip_box->Text = RECON_IP;
			this->recon_port_box->Text = "" + RECON_PORT;
			this->portBox->Text = "" + PHOTOSHOOT_PORT;
			portNumber = PHOTOSHOOT_PORT;
			helper = gcnew System::ComponentModel::BackgroundWorker();
			helper->DoWork += gcnew DoWorkEventHandler( this, &Form1::helper_DoWork );
			helper->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &Form1::helper_RunWorkerCompleted );
			helper->ProgressChanged += gcnew ProgressChangedEventHandler( this, &Form1::helper_ProgressChanged );
			helper->WorkerReportsProgress = true;
			helper->WorkerSupportsCancellation = true;
			running = false;
			sl_data->patternNum = 0;
			sl_data->gray_ncols = 0;
			sl_data->gray_nrows = 0;
			sl_data->gray_colshift = 0;
			sl_data->gray_rowshift = 0;
			
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
				closesocket(projClient);
				closesocket(reconClient);
			}
			WSACleanup();
			if (components)
			{
				delete components;
			}
			
			if(sl_data->proj_chessboard)
				cvReleaseImage(&sl_data->proj_chessboard);

			if(sl_data->proj_gray_codes)
			{
				for(int i=0; i<(sl_data->gray_ncols+sl_data->gray_nrows+1); i++)
					cvReleaseImage(&sl_data->proj_gray_codes[i]);
				delete[] sl_data->proj_gray_codes;
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
			cvReleaseImage(&sl_data->proj_frame);
			cvDestroyWindow("projWindow");
			
			delete sl_calib;
			delete sl_params;
			delete sl_data;
		}

	private: System::Windows::Forms::Button^  connectBtn;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  portBox;
	private: int portNumber;
	private: System::String^ outMessage;
	private: BackgroundWorker^ helper;
	private: SOCKET sClient, projClient, reconClient;
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

	private: System::Windows::Forms::TextBox^  ipBox;

	private: System::Windows::Forms::Label^  label4;
	
	private: slParams* sl_params;
	private: slCalib* sl_calib;
	private: slData* sl_data;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::TextBox^  proj_ip_box;
private: System::Windows::Forms::TextBox^  proj_port_box;


private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::TextBox^  recon_ip_box;
private: System::Windows::Forms::TextBox^  recon_port_box;


private: System::Windows::Forms::Label^  label11;



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
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->proj_ip_box = (gcnew System::Windows::Forms::TextBox());
			this->proj_port_box = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->recon_ip_box = (gcnew System::Windows::Forms::TextBox());
			this->recon_port_box = (gcnew System::Windows::Forms::TextBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// connectBtn
			// 
			this->connectBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->connectBtn->Location = System::Drawing::Point(396, 404);
			this->connectBtn->Name = L"connectBtn";
			this->connectBtn->Size = System::Drawing::Size(75, 23);
			this->connectBtn->TabIndex = 1;
			this->connectBtn->Text = L"Start";
			this->connectBtn->UseVisualStyleBackColor = true;
			this->connectBtn->Click += gcnew System::EventHandler(this, &Form1::connectBtn_Click);
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(246, 358);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(29, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Port:";
			// 
			// portBox
			// 
			this->portBox->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->portBox->Location = System::Drawing::Point(281, 355);
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
			this->console->Size = System::Drawing::Size(459, 268);
			this->console->TabIndex = 1;
			// 
			// setNameBox
			// 
			this->setNameBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->setNameBox->Location = System::Drawing::Point(78, 315);
			this->setNameBox->Name = L"setNameBox";
			this->setNameBox->Size = System::Drawing::Size(152, 20);
			this->setNameBox->TabIndex = 4;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(15, 318);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(57, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Set Name:";
			// 
			// editBaseFolderBtn
			// 
			this->editBaseFolderBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->editBaseFolderBtn->Location = System::Drawing::Point(132, 284);
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
			this->baseFolderLocation->Location = System::Drawing::Point(197, 289);
			this->baseFolderLocation->Name = L"baseFolderLocation";
			this->baseFolderLocation->Size = System::Drawing::Size(272, 18);
			this->baseFolderLocation->TabIndex = 7;
			this->baseFolderLocation->Text = L"./";
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(15, 289);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(111, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Base Image Directory:";
			// 
			// calibrationBtn
			// 
			this->calibrationBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->calibrationBtn->Location = System::Drawing::Point(363, 373);
			this->calibrationBtn->Name = L"calibrationBtn";
			this->calibrationBtn->Size = System::Drawing::Size(108, 23);
			this->calibrationBtn->TabIndex = 9;
			this->calibrationBtn->Text = L"Calibration Controls";
			this->calibrationBtn->UseVisualStyleBackColor = true;
			this->calibrationBtn->Click += gcnew System::EventHandler(this, &Form1::calibrationBtn_Click);
			// 
			// ipBox
			// 
			this->ipBox->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->ipBox->Location = System::Drawing::Point(157, 355);
			this->ipBox->Name = L"ipBox";
			this->ipBox->Size = System::Drawing::Size(73, 20);
			this->ipBox->TabIndex = 10;
			this->ipBox->Text = L"127.0.0.1";
			// 
			// label4
			// 
			this->label4->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(131, 358);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(20, 13);
			this->label4->TabIndex = 11;
			this->label4->Text = L"IP:";
			// 
			// label5
			// 
			this->label5->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(15, 358);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(75, 13);
			this->label5->TabIndex = 12;
			this->label5->Text = L"Photoshoot:";
			// 
			// label6
			// 
			this->label6->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(15, 383);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(62, 13);
			this->label6->TabIndex = 17;
			this->label6->Text = L"Projector:";
			// 
			// label7
			// 
			this->label7->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(131, 383);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(20, 13);
			this->label7->TabIndex = 16;
			this->label7->Text = L"IP:";
			// 
			// proj_ip_box
			// 
			this->proj_ip_box->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->proj_ip_box->Location = System::Drawing::Point(157, 380);
			this->proj_ip_box->Name = L"proj_ip_box";
			this->proj_ip_box->Size = System::Drawing::Size(73, 20);
			this->proj_ip_box->TabIndex = 15;
			this->proj_ip_box->Text = L"127.0.0.1";
			// 
			// proj_port_box
			// 
			this->proj_port_box->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->proj_port_box->Location = System::Drawing::Point(281, 380);
			this->proj_port_box->Name = L"proj_port_box";
			this->proj_port_box->Size = System::Drawing::Size(38, 20);
			this->proj_port_box->TabIndex = 14;
			this->proj_port_box->Text = L"6464";
			// 
			// label8
			// 
			this->label8->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(246, 383);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(29, 13);
			this->label8->TabIndex = 13;
			this->label8->Text = L"Port:";
			// 
			// label9
			// 
			this->label9->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label9->Location = System::Drawing::Point(15, 409);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(97, 13);
			this->label9->TabIndex = 22;
			this->label9->Text = L"Reconstruction:";
			// 
			// label10
			// 
			this->label10->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(131, 409);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(20, 13);
			this->label10->TabIndex = 21;
			this->label10->Text = L"IP:";
			// 
			// recon_ip_box
			// 
			this->recon_ip_box->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->recon_ip_box->Location = System::Drawing::Point(157, 406);
			this->recon_ip_box->Name = L"recon_ip_box";
			this->recon_ip_box->Size = System::Drawing::Size(73, 20);
			this->recon_ip_box->TabIndex = 20;
			this->recon_ip_box->Text = L"127.0.0.1";
			// 
			// recon_port_box
			// 
			this->recon_port_box->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->recon_port_box->Location = System::Drawing::Point(281, 406);
			this->recon_port_box->Name = L"recon_port_box";
			this->recon_port_box->Size = System::Drawing::Size(38, 20);
			this->recon_port_box->TabIndex = 19;
			this->recon_port_box->Text = L"7272";
			// 
			// label11
			// 
			this->label11->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(246, 409);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(29, 13);
			this->label11->TabIndex = 18;
			this->label11->Text = L"Port:";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(483, 434);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->recon_ip_box);
			this->Controls->Add(this->recon_port_box);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->proj_ip_box);
			this->Controls->Add(this->proj_port_box);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label5);
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
					struct sockaddr_in server, projServer, reconServer;

					sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					
					if (sClient == INVALID_SOCKET)
					{
						sprintf(buffer, "socket() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}

					projClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if (projClient == INVALID_SOCKET)
					{
						sprintf(buffer, "socket() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}

					reconClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if (reconClient == INVALID_SOCKET)
					{
						sprintf(buffer, "socket() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}

					server.sin_family = AF_INET;
					server.sin_port = htons(portNumber);
					server.sin_addr.s_addr = inet_addr(gc2std(this->ipBox->Text).c_str());

					projServer.sin_family = AF_INET;
					projServer.sin_port = htons(atoi(gc2std(this->proj_port_box->Text).c_str()));
					projServer.sin_addr.s_addr = inet_addr(gc2std(this->proj_ip_box->Text).c_str());

					reconServer.sin_family = AF_INET;
					reconServer.sin_port = htons(atoi(gc2std(this->recon_port_box->Text).c_str()));
					reconServer.sin_addr.s_addr = inet_addr(gc2std(this->recon_ip_box->Text).c_str());
				   
					if (connect(sClient, (struct sockaddr *)&server,
						sizeof(server)) == SOCKET_ERROR)
					{
						sprintf(buffer, "connect() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}

					if (connect(projClient, (struct sockaddr *)&projServer,
						sizeof(projServer)) == SOCKET_ERROR)
					{
						sprintf(buffer, "connect() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}

					if (connect(reconClient, (struct sockaddr *)&reconServer,
						sizeof(reconServer)) == SOCKET_ERROR)
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
					this->calibrationBtn->Text = "Toggle Projector";
				}
				else
				{
					helper->CancelAsync();
					closesocket(sClient);
					closesocket(projClient);
					closesocket(reconClient);
					portBox->Enabled = true;
					running = false;
					connectBtn->Text = "Connect";
					this->calibrationBtn->Text = "Calibration Controls";
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
							outMessage += sendMessage(sClient, "UBW FW D Version 1.4.3.eqpi\r\n");
							worker->ReportProgress( 0 );
						}
						else if(szBuffer[0] == 'W')
						{
							outMessage += sendMessage(sClient, "0027_20100101_0001_eqpi\r\n");//ProjectorClient\r\n");
							worker->ReportProgress( 0 );
						}
						/*else if(!strcmp(szBuffer, "O,64,0,0"))
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
						}*/
						else if(!strcmp(szBuffer, "O,128,0,0"))
						{
							outMessage += displayCheckerboard();
							worker->ReportProgress(0);
						}
						else if(!strcmp(szBuffer, "O,0,64,0"))   //Normally UV
						{
							outMessage += displayBlank();
							worker->ReportProgress(0);
						}
						else if(!strcmp(szBuffer, "O,0,128,0"))  //Royal Blue
						{
							outMessage += startPattern();
							worker->ReportProgress( 0 );
						}
						else if(!strcmp(szBuffer, "O,0,32,0"))  //Blue
						{
							outMessage += nextPattern();
							worker->ReportProgress( 0 );
						}
					}
					return;
			 }

			System::String^ sendMessage(SOCKET socket, const char* message)
			{
				int ret;
				char buffer[BUFFER_SIZE];

				ret = send(socket, message, strlen(message), 0);
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
			
			System::String^ startPattern()
			{
				sendMessage(projClient, "a");
				if(!sl_data->proj_gray_codes)
				{
					generateGrayCodes(sl_params->proj_w, sl_params->proj_h, sl_data->proj_gray_codes, 
						sl_data->gray_ncols, sl_data->gray_nrows, sl_data->gray_colshift, sl_data->gray_rowshift, 
						sl_params->scan_cols, sl_params->scan_rows);
				}
				Sleep(1000);
				sl_data->patternNum = 0;
				
				cvCopy(sl_data->proj_gray_codes[sl_data->patternNum], sl_data->proj_frame);
				cvScale(sl_data->proj_frame, sl_data->proj_frame, 2.*(sl_params->proj_gain/100.), 0);
				cvShowImage("projWindow", sl_data->proj_frame);
				
				return "Pattern Sequence Started\r\n";
			}

			System::String^ nextPattern()
			{
				static bool inverseTime = true;
				if(!sl_data->proj_gray_codes)
				{
					generateGrayCodes(sl_params->proj_w, sl_params->proj_h, sl_data->proj_gray_codes, 
						sl_data->gray_ncols, sl_data->gray_nrows, sl_data->gray_colshift, sl_data->gray_rowshift, 
						sl_params->scan_cols, sl_params->scan_rows);
				}
				
				bool last = false;
				if(inverseTime || sl_data->patternNum == 0)
				{
					cvSubRS(sl_data->proj_gray_codes[sl_data->patternNum], cvScalar(255), sl_data->proj_frame);
					sl_data->patternNum = (sl_data->patternNum+1)%(sl_data->gray_ncols + sl_data->gray_nrows+1);
					last = !(sl_data->patternNum);
					inverseTime = false;
				}
				else
				{
					cvCopy(sl_data->proj_gray_codes[sl_data->patternNum], sl_data->proj_frame);
					inverseTime = true;
				}
				cvScale(sl_data->proj_frame, sl_data->proj_frame, 2.*(sl_params->proj_gain/100.), 0);
				cvShowImage("projWindow", sl_data->proj_frame);

				if(last)  //TODO: Figure out how to remember what images to open for reconstruction!
				{
					sendMessage(projClient, "a");
					Sleep(1000);
					sendMessage(reconClient, gc2std(this->setNameBox->Text).c_str());
					return "Last Pattern Displayed\r\n";
				}
				
				return "Next Pattern Displayed\r\n";
			}

			System::String^ displayCheckerboard()
			{
				
				if(sl_data->proj_chessboard == NULL)
				{
					sl_data->proj_chessboard = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
					int proj_border_cols, proj_border_rows;
					if(generateChessboard(sl_params, sl_data->proj_chessboard, proj_border_cols, proj_border_rows) == -1)
						return "Calibration Checkerboard creation failed!\r\n";
				}
				
				cvCopy(sl_data->proj_chessboard, sl_data->proj_frame);
				cvScale(sl_data->proj_frame, sl_data->proj_frame, 2.*(sl_params->proj_gain/100.), 0);
				cvShowImage("projWindow", sl_data->proj_frame);
				
				sendMessage(projClient, "a");

				return "Displaying Calibration Checkerboard\r\n";
			}
			
			System::String^ displayBlank()
			{
				sendMessage(projClient, "a");
				Sleep(1000);

				cvSet(sl_data->proj_frame, cvScalar(255));
				cvScale(sl_data->proj_frame, sl_data->proj_frame, 2.*(sl_params->proj_gain/100.), 0);
				cvShowImage("projWindow", sl_data->proj_frame);

				return "Displaying Blank Image\r\n";
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
				if(!running)
				{
					calibrationForm^ calibrationWindow = gcnew calibrationForm(sl_params, sl_calib);
					calibrationWindow->Visible = true;
				}
				else
				{
					sendMessage(projClient, "a");
					Sleep(1000);
				}
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

				// Create fullscreen window (for controlling projector display).
				cvNamedWindow("projWindow", CV_WINDOW_AUTOSIZE);
				sl_data->proj_frame = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
				cvSet(sl_data->proj_frame, cvScalar(0, 0, 0));
				cvShowImage("projWindow", sl_data->proj_frame);
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

