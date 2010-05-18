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
#define RECON_IP "127.0.0.1"
#define RECON_PORT 7272

#define BUFFER_SIZE 1024
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
			this->setNameBox->Text = DEFAULT_SET_NAME;
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
				if(reconOn)
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
	private: SOCKET sClient, reconClient;
	private: bool running;


	public: System::Windows::Forms::TextBox^  console;
	private: System::Windows::Forms::TextBox^  setNameBox;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;




	private: System::Windows::Forms::Button^  calibrationBtn;

	private: System::Windows::Forms::TextBox^  ipBox;

	private: System::Windows::Forms::Label^  label4;
	
	private: slParams* sl_params;
	private: slCalib* sl_calib;
	private: slData* sl_data;
private: System::Windows::Forms::Label^  label5;







private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::TextBox^  recon_ip_box;
private: System::Windows::Forms::TextBox^  recon_port_box;


private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::TextBox^  tileBox;
private: bool reconOn;



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
			this->calibrationBtn = (gcnew System::Windows::Forms::Button());
			this->ipBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->recon_ip_box = (gcnew System::Windows::Forms::TextBox());
			this->recon_port_box = (gcnew System::Windows::Forms::TextBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->tileBox = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// connectBtn
			// 
			this->connectBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->connectBtn->Location = System::Drawing::Point(396, 363);
			this->connectBtn->Name = L"connectBtn";
			this->connectBtn->Size = System::Drawing::Size(75, 23);
			this->connectBtn->TabIndex = 1;
			this->connectBtn->Text = L"Start";
			this->connectBtn->UseVisualStyleBackColor = true;
			this->connectBtn->Click += gcnew System::EventHandler(this, &Form1::connectBtn_Click);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(246, 337);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(29, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Port:";
			// 
			// portBox
			// 
			this->portBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->portBox->Location = System::Drawing::Point(281, 334);
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
			this->console->Size = System::Drawing::Size(459, 276);
			this->console->TabIndex = 1;
			// 
			// setNameBox
			// 
			this->setNameBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->setNameBox->Location = System::Drawing::Point(78, 294);
			this->setNameBox->Name = L"setNameBox";
			this->setNameBox->Size = System::Drawing::Size(152, 20);
			this->setNameBox->TabIndex = 40;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(15, 297);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(57, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Set Name:";
			// 
			// calibrationBtn
			// 
			this->calibrationBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->calibrationBtn->Location = System::Drawing::Point(363, 332);
			this->calibrationBtn->Name = L"calibrationBtn";
			this->calibrationBtn->Size = System::Drawing::Size(108, 23);
			this->calibrationBtn->TabIndex = 9;
			this->calibrationBtn->Text = L"Calibration Controls";
			this->calibrationBtn->UseVisualStyleBackColor = true;
			this->calibrationBtn->Click += gcnew System::EventHandler(this, &Form1::calibrationBtn_Click);
			// 
			// ipBox
			// 
			this->ipBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->ipBox->Location = System::Drawing::Point(157, 334);
			this->ipBox->Name = L"ipBox";
			this->ipBox->Size = System::Drawing::Size(73, 20);
			this->ipBox->TabIndex = 10;
			this->ipBox->Text = L"127.0.0.1";
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(131, 337);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(20, 13);
			this->label4->TabIndex = 11;
			this->label4->Text = L"IP:";
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(15, 337);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(75, 13);
			this->label5->TabIndex = 12;
			this->label5->Text = L"Photoshoot:";
			// 
			// label9
			// 
			this->label9->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label9->Location = System::Drawing::Point(15, 366);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(97, 13);
			this->label9->TabIndex = 22;
			this->label9->Text = L"Reconstruction:";
			// 
			// label10
			// 
			this->label10->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(131, 366);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(20, 13);
			this->label10->TabIndex = 21;
			this->label10->Text = L"IP:";
			// 
			// recon_ip_box
			// 
			this->recon_ip_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->recon_ip_box->Location = System::Drawing::Point(157, 363);
			this->recon_ip_box->Name = L"recon_ip_box";
			this->recon_ip_box->Size = System::Drawing::Size(73, 20);
			this->recon_ip_box->TabIndex = 20;
			this->recon_ip_box->Text = L"127.0.0.1";
			// 
			// recon_port_box
			// 
			this->recon_port_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->recon_port_box->Location = System::Drawing::Point(281, 363);
			this->recon_port_box->Name = L"recon_port_box";
			this->recon_port_box->Size = System::Drawing::Size(38, 20);
			this->recon_port_box->TabIndex = 19;
			this->recon_port_box->Text = L"7272";
			// 
			// label11
			// 
			this->label11->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(246, 366);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(29, 13);
			this->label11->TabIndex = 18;
			this->label11->Text = L"Port:";
			// 
			// label12
			// 
			this->label12->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(260, 297);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(58, 13);
			this->label12->TabIndex = 24;
			this->label12->Text = L"Tile Name:";
			// 
			// tileBox
			// 
			this->tileBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->tileBox->Location = System::Drawing::Point(324, 294);
			this->tileBox->Name = L"tileBox";
			this->tileBox->Size = System::Drawing::Size(145, 20);
			this->tileBox->TabIndex = 41;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(483, 395);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->tileBox);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->recon_ip_box);
			this->Controls->Add(this->recon_port_box);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->ipBox);
			this->Controls->Add(this->calibrationBtn);
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
					struct sockaddr_in server, reconServer;

					sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					
					if (sClient == INVALID_SOCKET)
					{
						sprintf(buffer, "socket() failed: %d\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
						return;
					}

					reconOn = false;
					reconClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if (reconClient == INVALID_SOCKET)
					{
						sprintf(buffer, "Reconstruction Client socket() failed: %d\r\nNot connecting to recon client.\r\n", WSAGetLastError());
						console->Text += gcnew System::String(buffer);
					}
					else
					{
						reconServer.sin_family = AF_INET;
						reconServer.sin_port = htons(atoi(gc2std(this->recon_port_box->Text).c_str()));
						reconServer.sin_addr.s_addr = inet_addr(gc2std(this->recon_ip_box->Text).c_str());
						
						if (connect(reconClient, (struct sockaddr *)&reconServer,
							sizeof(reconServer)) == SOCKET_ERROR)
						{
							sprintf(buffer, "recon connect() failed: %d\r\n", WSAGetLastError());
							console->Text += gcnew System::String(buffer);
						}
						reconOn = true;
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
					this->ipBox->Enabled = false;
					portBox->Enabled = false;
					this->recon_ip_box->Enabled = false;
					this->recon_port_box->Enabled = false;
					running = true;
					connectBtn->Text = "Stop";
					//this->calibrationBtn->Enabled = false;
				}
				else
				{
					helper->CancelAsync();
					closesocket(sClient);
					if(reconOn)
						closesocket(reconClient);
					this->ipBox->Enabled = true;
					portBox->Enabled = true;
					this->recon_ip_box->Enabled = true;
					this->recon_port_box->Enabled = true;
					running = false;
					connectBtn->Text = "Connect";
					//this->calibrationBtn->Enabled = true;
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
							outMessage += sendMessage(sClient, "1337_20100101_0099_eqpi\r\n");//ProjectorClient\r\n");
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
				if(!sl_data->proj_gray_codes)
				{
					generateGrayCodes(sl_params->proj_w, sl_params->proj_h, sl_data->proj_gray_codes, 
						sl_data->gray_ncols, sl_data->gray_nrows, sl_data->gray_colshift, sl_data->gray_rowshift, 
						sl_params->scan_cols, sl_params->scan_rows);
				}

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
					if(reconOn)
						sendMessage(reconClient, gc2std(this->setNameBox->Text + this->tileBox->Text).c_str());
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

				return "Displaying Calibration Checkerboard\r\n";
			}
			
			System::String^ displayBlank()
			{
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

