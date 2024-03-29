#pragma once

#include "../common/cvCalibrateProCam.h"
#include "../common/cvUtilProCam.h"
#include "../common/globals.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace reconstructionController {

	/// <summary>
	/// Summary for calibrationForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class calibrationForm : public System::Windows::Forms::Form
	{
	public:
		calibrationForm(slParams* sl_params, slCalib* sl_calib)//struct slParams* sl_params)
		{
			InitializeComponent();
			
			this->sl_params = sl_params;
			this->sl_calib = sl_calib;

			if(sl_calib->cam_intrinsic_calib)
			{
				this->cameraStatusLbl->ForeColor = System::Drawing::Color::Green;
				this->cameraStatusLbl->Text = "Calibrated!";
			}

			if(sl_calib->proj_intrinsic_calib)
			{
				this->projStatusLbl->ForeColor = System::Drawing::Color::Green;
				this->projStatusLbl->Text = "Calibrated!";
			}

			if(sl_calib->procam_extrinsic_calib)
			{
				String^ myString = "Projector at: ";
				char temp[40];
				sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[0] / 25.4 ); 
				myString += gcnew String(temp) + ", ";
				sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[1] / 25.4 ); 
				myString += gcnew String(temp) + ", ";
				sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[2] / 25.4 ); 
				myString += gcnew String(temp) + " inches";
				
				this->extrinsicStatusLbl->ForeColor = System::Drawing::Color::Green;
				this->extrinsicStatusLbl->Text = myString;
			}

			helper = gcnew System::ComponentModel::BackgroundWorker();
			helper->DoWork += gcnew DoWorkEventHandler( this, &calibrationForm::helper_DoWork );
			helper->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &calibrationForm::helper_RunWorkerCompleted );
			helper->ProgressChanged += gcnew ProgressChangedEventHandler( this, &calibrationForm::helper_ProgressChanged );
			helper->WorkerReportsProgress = true;
			helper->WorkerSupportsCancellation = true;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~calibrationForm()
		{
			if(helper->IsBusy)
			{
				helper->CancelAsync();
			}
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  cameraCalibrationDirLbl;
	protected: 
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Button^  cameraEditBtn;
	private: System::Windows::Forms::Label^  cameraStatusLbl;


	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  cameraStartBtn;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Label^  projStatusLbl;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Button^  projectorStartBtn;

	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Button^  projEditBtn;
	private: System::Windows::Forms::Label^  projectorCalibrationDirLbl;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Label^  extrinsicStatusLbl;

	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Button^  extrinsicStartBtn;

	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Button^  extrensicEditBtn;
	private: System::Windows::Forms::Label^  extrinsicDirLbl;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;
	private: slParams* sl_params;
	private: slCalib* sl_calib;
	private: System::Windows::Forms::CheckBox^  checkBox1;



	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: BackgroundWorker^ helper;
	private: int helperType;
	private: System::String^ cameraStatus;
	private: System::String^ projectorStatus;
	private: System::String^ extrinsicStatus;
private: System::Windows::Forms::Label^  testOutLbl;

private: System::Windows::Forms::Label^  label4;






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
			this->cameraCalibrationDirLbl = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->cameraStatusLbl = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->cameraStartBtn = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->cameraEditBtn = (gcnew System::Windows::Forms::Button());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->projStatusLbl = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->projectorStartBtn = (gcnew System::Windows::Forms::Button());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->projEditBtn = (gcnew System::Windows::Forms::Button());
			this->projectorCalibrationDirLbl = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->testOutLbl = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->extrinsicStartBtn = (gcnew System::Windows::Forms::Button());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->extrensicEditBtn = (gcnew System::Windows::Forms::Button());
			this->extrinsicDirLbl = (gcnew System::Windows::Forms::Label());
			this->extrinsicStatusLbl = (gcnew System::Windows::Forms::Label());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// cameraCalibrationDirLbl
			// 
			this->cameraCalibrationDirLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cameraCalibrationDirLbl->AutoEllipsis = true;
			this->cameraCalibrationDirLbl->Location = System::Drawing::Point(160, 25);
			this->cameraCalibrationDirLbl->Name = L"cameraCalibrationDirLbl";
			this->cameraCalibrationDirLbl->Size = System::Drawing::Size(249, 19);
			this->cameraCalibrationDirLbl->TabIndex = 0;
			this->cameraCalibrationDirLbl->Text = L"./cameraCalibration/";
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->cameraStatusLbl);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->cameraStartBtn);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->cameraEditBtn);
			this->groupBox1->Controls->Add(this->cameraCalibrationDirLbl);
			this->groupBox1->Location = System::Drawing::Point(12, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(417, 76);
			this->groupBox1->TabIndex = 1;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Camera Intrinsic Calibration";
			// 
			// cameraStatusLbl
			// 
			this->cameraStatusLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cameraStatusLbl->AutoEllipsis = true;
			this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
			this->cameraStatusLbl->Location = System::Drawing::Point(53, 52);
			this->cameraStatusLbl->Name = L"cameraStatusLbl";
			this->cameraStatusLbl->Size = System::Drawing::Size(255, 13);
			this->cameraStatusLbl->TabIndex = 5;
			this->cameraStatusLbl->Text = L"Not Calibrated";
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 52);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(40, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Status:";
			// 
			// cameraStartBtn
			// 
			this->cameraStartBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cameraStartBtn->Location = System::Drawing::Point(314, 47);
			this->cameraStartBtn->Name = L"cameraStartBtn";
			this->cameraStartBtn->Size = System::Drawing::Size(97, 23);
			this->cameraStartBtn->TabIndex = 3;
			this->cameraStartBtn->Text = L"Run Calibration";
			this->cameraStartBtn->UseVisualStyleBackColor = true;
			this->cameraStartBtn->Click += gcnew System::EventHandler(this, &calibrationForm::cameraStartBtn_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 25);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(89, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Images Directory:";
			// 
			// cameraEditBtn
			// 
			this->cameraEditBtn->Location = System::Drawing::Point(104, 20);
			this->cameraEditBtn->Name = L"cameraEditBtn";
			this->cameraEditBtn->Size = System::Drawing::Size(50, 23);
			this->cameraEditBtn->TabIndex = 1;
			this->cameraEditBtn->Text = L"Edit";
			this->cameraEditBtn->UseVisualStyleBackColor = true;
			this->cameraEditBtn->Click += gcnew System::EventHandler(this, &calibrationForm::cameraEditBtn_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox2->Controls->Add(this->checkBox1);
			this->groupBox2->Controls->Add(this->projStatusLbl);
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Controls->Add(this->projectorStartBtn);
			this->groupBox2->Controls->Add(this->label6);
			this->groupBox2->Controls->Add(this->projEditBtn);
			this->groupBox2->Controls->Add(this->projectorCalibrationDirLbl);
			this->groupBox2->Location = System::Drawing::Point(12, 100);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(415, 98);
			this->groupBox2->TabIndex = 6;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Projector Intrinsic Calibration";
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(9, 50);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(174, 17);
			this->checkBox1->TabIndex = 6;
			this->checkBox1->Text = L"Calibrate Camera at Same Time";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// projStatusLbl
			// 
			this->projStatusLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->projStatusLbl->AutoEllipsis = true;
			this->projStatusLbl->ForeColor = System::Drawing::Color::Red;
			this->projStatusLbl->Location = System::Drawing::Point(53, 74);
			this->projStatusLbl->Name = L"projStatusLbl";
			this->projStatusLbl->Size = System::Drawing::Size(255, 13);
			this->projStatusLbl->TabIndex = 5;
			this->projStatusLbl->Text = L"Not Calibrated";
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(6, 74);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(40, 13);
			this->label5->TabIndex = 4;
			this->label5->Text = L"Status:";
			// 
			// projectorStartBtn
			// 
			this->projectorStartBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->projectorStartBtn->Location = System::Drawing::Point(312, 69);
			this->projectorStartBtn->Name = L"projectorStartBtn";
			this->projectorStartBtn->Size = System::Drawing::Size(97, 23);
			this->projectorStartBtn->TabIndex = 3;
			this->projectorStartBtn->Text = L"Run Calibration";
			this->projectorStartBtn->UseVisualStyleBackColor = true;
			this->projectorStartBtn->Click += gcnew System::EventHandler(this, &calibrationForm::projectorStartBtn_Click);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 25);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(89, 13);
			this->label6->TabIndex = 2;
			this->label6->Text = L"Images Directory:";
			// 
			// projEditBtn
			// 
			this->projEditBtn->Location = System::Drawing::Point(104, 20);
			this->projEditBtn->Name = L"projEditBtn";
			this->projEditBtn->Size = System::Drawing::Size(50, 23);
			this->projEditBtn->TabIndex = 1;
			this->projEditBtn->Text = L"Edit";
			this->projEditBtn->UseVisualStyleBackColor = true;
			this->projEditBtn->Click += gcnew System::EventHandler(this, &calibrationForm::projEditBtn_Click);
			// 
			// projectorCalibrationDirLbl
			// 
			this->projectorCalibrationDirLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->projectorCalibrationDirLbl->AutoEllipsis = true;
			this->projectorCalibrationDirLbl->Location = System::Drawing::Point(160, 25);
			this->projectorCalibrationDirLbl->Name = L"projectorCalibrationDirLbl";
			this->projectorCalibrationDirLbl->Size = System::Drawing::Size(249, 19);
			this->projectorCalibrationDirLbl->TabIndex = 0;
			this->projectorCalibrationDirLbl->Text = L"./projectorCalibration/";
			// 
			// groupBox3
			// 
			this->groupBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox3->Controls->Add(this->testOutLbl);
			this->groupBox3->Controls->Add(this->label8);
			this->groupBox3->Controls->Add(this->extrinsicStartBtn);
			this->groupBox3->Controls->Add(this->label9);
			this->groupBox3->Controls->Add(this->extrensicEditBtn);
			this->groupBox3->Controls->Add(this->extrinsicDirLbl);
			this->groupBox3->Location = System::Drawing::Point(12, 251);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(415, 80);
			this->groupBox3->TabIndex = 7;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Calibration Test";
			// 
			// testOutLbl
			// 
			this->testOutLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->testOutLbl->AutoEllipsis = true;
			this->testOutLbl->ForeColor = System::Drawing::Color::Red;
			this->testOutLbl->Location = System::Drawing::Point(52, 56);
			this->testOutLbl->Name = L"testOutLbl";
			this->testOutLbl->Size = System::Drawing::Size(254, 13);
			this->testOutLbl->TabIndex = 10;
			this->testOutLbl->Text = L"N/A";
			// 
			// label8
			// 
			this->label8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(6, 56);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(40, 13);
			this->label8->TabIndex = 4;
			this->label8->Text = L"Result:";
			// 
			// extrinsicStartBtn
			// 
			this->extrinsicStartBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->extrinsicStartBtn->Location = System::Drawing::Point(338, 51);
			this->extrinsicStartBtn->Name = L"extrinsicStartBtn";
			this->extrinsicStartBtn->Size = System::Drawing::Size(71, 23);
			this->extrinsicStartBtn->TabIndex = 3;
			this->extrinsicStartBtn->Text = L"Run Test";
			this->extrinsicStartBtn->UseVisualStyleBackColor = true;
			this->extrinsicStartBtn->Click += gcnew System::EventHandler(this, &calibrationForm::extrinsicStartBtn_Click);
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(6, 25);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(48, 13);
			this->label9->TabIndex = 2;
			this->label9->Text = L"Image 1:";
			// 
			// extrensicEditBtn
			// 
			this->extrensicEditBtn->Location = System::Drawing::Point(61, 20);
			this->extrensicEditBtn->Name = L"extrensicEditBtn";
			this->extrensicEditBtn->Size = System::Drawing::Size(50, 23);
			this->extrensicEditBtn->TabIndex = 1;
			this->extrensicEditBtn->Text = L"Edit";
			this->extrensicEditBtn->UseVisualStyleBackColor = true;
			this->extrensicEditBtn->Click += gcnew System::EventHandler(this, &calibrationForm::extrensicEditBtn_Click);
			// 
			// extrinsicDirLbl
			// 
			this->extrinsicDirLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->extrinsicDirLbl->AutoEllipsis = true;
			this->extrinsicDirLbl->Location = System::Drawing::Point(117, 25);
			this->extrinsicDirLbl->Name = L"extrinsicDirLbl";
			this->extrinsicDirLbl->Size = System::Drawing::Size(292, 19);
			this->extrinsicDirLbl->TabIndex = 0;
			this->extrinsicDirLbl->Text = L"./Pic1.tif";
			// 
			// extrinsicStatusLbl
			// 
			this->extrinsicStatusLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->extrinsicStatusLbl->AutoEllipsis = true;
			this->extrinsicStatusLbl->ForeColor = System::Drawing::Color::Red;
			this->extrinsicStatusLbl->Location = System::Drawing::Point(138, 219);
			this->extrinsicStatusLbl->Name = L"extrinsicStatusLbl";
			this->extrinsicStatusLbl->Size = System::Drawing::Size(283, 13);
			this->extrinsicStatusLbl->TabIndex = 5;
			this->extrinsicStatusLbl->Text = L"Not Calibrated";
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->DefaultExt = L"tif";
			this->openFileDialog1->FileName = L"image.tif";
			this->openFileDialog1->Filter = L"Tif Images (*.tif)|*.tif";
			this->openFileDialog1->RestoreDirectory = true;
			this->openFileDialog1->Title = L"Image Selection";
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(18, 219);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(115, 13);
			this->label4->TabIndex = 9;
			this->label4->Text = L"Projector Location:";
			// 
			// calibrationForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(442, 343);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->extrinsicStatusLbl);
			this->Name = L"calibrationForm";
			this->Text = L"Structured Light Calibration";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void cameraEditBtn_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				System::Windows::Forms::DialogResult result = folderBrowserDialog1->ShowDialog();
				if ( result == ::DialogResult::OK )
				{
					this->cameraCalibrationDirLbl->Text = folderBrowserDialog1->SelectedPath;
				}
			 }
	private: System::Void projEditBtn_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				System::Windows::Forms::DialogResult result = folderBrowserDialog1->ShowDialog();
				if ( result == ::DialogResult::OK )
				{
					this->projectorCalibrationDirLbl->Text = folderBrowserDialog1->SelectedPath;
				}
			 }
	private: System::Void extrensicEditBtn_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				System::Windows::Forms::DialogResult result = openFileDialog1->ShowDialog();
				if ( result == ::DialogResult::OK )
				{
					this->extrinsicDirLbl->Text = openFileDialog1->FileName;
				}
			 }

	private: System::Void cameraStartBtn_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				// Reset camera calibration status (will be set again, if successful).
				sl_calib->cam_intrinsic_calib = false;
				sl_calib->proj_intrinsic_calib   = false;
				sl_calib->procam_extrinsic_calib = false;
				
				this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
				this->cameraStatusLbl->Text = "Not Calibrated";

				this->projStatusLbl->ForeColor = System::Drawing::Color::Red;
				this->projStatusLbl->Text = "Not Calibrated";

				this->extrinsicStatusLbl->ForeColor = System::Drawing::Color::Red;
				this->extrinsicStatusLbl->Text = "Not Calibrated";

				// Create camera calibration directory (clear previous calibration first).
				char str[1024], calibDir[1024];
				sprintf(calibDir, "%s\\calib\\cam", sl_params->outdir);
				sprintf(str, "%s\\calib", sl_params->outdir);
				_mkdir(str);
				_mkdir(calibDir);
				sprintf(str, "rd /s /q \"%s\"", calibDir);
				system(str);
				if(_mkdir(calibDir) != 0){
					this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
					this->cameraStatusLbl->Text = "ERROR: Cannot open output directory!";
					return;
				}

				// Prompt user for maximum number of calibration boards.
				
				IplImage** imagesBuffer;
				int n_boards = getImages2(imagesBuffer, 50, this->cameraCalibrationDirLbl->Text, "*.tif");
				scanf("%d", &n_boards);
				if(n_boards<4){
					this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
					this->cameraStatusLbl->Text = "ERROR: At least two images are required!";
					for(int i=0; i<n_boards; i++)
						cvReleaseImage(&imagesBuffer[i]);
					if(n_boards)
						delete[] imagesBuffer;
					return;
				}

				// Evaluate derived chessboard parameters and allocate storage.
				int board_n            = sl_params->cam_board_w*sl_params->cam_board_h;
				CvSize board_size      = cvSize(sl_params->cam_board_w, sl_params->cam_board_h);
				CvMat* image_points    = cvCreateMat(n_boards*board_n, 2, CV_32FC1);
				CvMat* object_points   = cvCreateMat(n_boards*board_n, 3, CV_32FC1);
				CvMat* point_counts    = cvCreateMat(n_boards, 1, CV_32SC1);
				IplImage** calibImages = new IplImage*[n_boards];
				CvSize frame_size = cvGetSize(imagesBuffer[0]);
				for(int i=0; i<n_boards; i++)
					calibImages[i] = cvCreateImage(frame_size, imagesBuffer[0]->depth, imagesBuffer[0]->nChannels);

				// Create a window to display captured frames.
				cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
				cvCreateTrackbar("Cam. Gain", "camWindow", &sl_params->cam_gain, 100, NULL);
				HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
				BringWindowToTop(camWindow);
				cvWaitKey(1);

				// Capture live image stream, until "ESC" is pressed or calibration is complete.
				int successes = 0;
				bool goodFrame;
				IplImage* cam_frame = cvCreateImage(frame_size, imagesBuffer[0]->depth, imagesBuffer[0]->nChannels);

				for(int num=0; num<n_boards; num+=2)
				{
					goodFrame = false;
					// Get next available frame.
					cvCopyImage(imagesBuffer[num], cam_frame);
					cvScale(cam_frame, cam_frame, 2.*(sl_params->cam_gain/100.), 0);

					// Find chessboard corners.
					CvPoint2D32f* corners = new CvPoint2D32f[board_n];
					int corner_count;
					int found = detectChessboard(cam_frame, board_size, corners, &corner_count);

					// If chessboard is detected, then add points to calibration list.
					if(corner_count == board_n){
						for(int i=successes*board_n, j=0; j<board_n; ++i,++j){
							CV_MAT_ELEM(*image_points,  float, i, 0) = corners[j].x;
							CV_MAT_ELEM(*image_points,  float, i, 1) = corners[j].y;
							CV_MAT_ELEM(*object_points, float, i, 0) = sl_params->cam_board_w_mm*float(j/sl_params->cam_board_w);
							CV_MAT_ELEM(*object_points, float, i, 1) = sl_params->cam_board_h_mm*float(j%sl_params->cam_board_w);
							CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
						}
						CV_MAT_ELEM(*point_counts, int, successes, 0) = board_n;
						cvCopyImage(cam_frame, calibImages[successes]);
						successes++;
						goodFrame = true;
					}
					
					// Display frame.
					if(!found)
					{
						cvDrawChessboardCorners(cam_frame, board_size, corners, corner_count, found);
						cvShowImageResampled("camWindow", cam_frame, sl_params->window_w, sl_params->window_h);
					}

					// Free allocated resources.
					delete[] corners;

					// Process user input.
					if(!found)
					{
						int cvKey = cvWaitKey(0);
						if(cvKey==27)
						{
							break;
						}
						else if(cvKey=='r')
						{
							num-=2;
							if(goodFrame)
							{
								successes--;
							}
						}
					}
				}
				cvReleaseImage(&cam_frame);

				// Close the display window.
				cvDestroyWindow("camWindow");

				// Calibrate camera, if minimum number of frames are available.
				if(successes >= 2){

					// Allocate calibration matrices.
					CvMat* object_points2      = cvCreateMat(successes*board_n, 3, CV_32FC1);
					CvMat* image_points2       = cvCreateMat(successes*board_n, 2, CV_32FC1);
					CvMat* point_counts2       = cvCreateMat(successes, 1, CV_32SC1);
					CvMat* rotation_vectors    = cvCreateMat(successes, 3, CV_32FC1);
  					CvMat* translation_vectors = cvCreateMat(successes, 3, CV_32FC1);

					// Transfer calibration data from captured values.
					for(int i=0; i<successes*board_n; ++i){
						CV_MAT_ELEM(*image_points2,  float, i, 0) = CV_MAT_ELEM(*image_points,  float, i, 0);
						CV_MAT_ELEM(*image_points2,  float, i, 1) = CV_MAT_ELEM(*image_points,  float, i, 1);
						CV_MAT_ELEM(*object_points2, float, i, 0) =	CV_MAT_ELEM(*object_points, float, i, 0);
						CV_MAT_ELEM(*object_points2, float, i, 1) =	CV_MAT_ELEM(*object_points, float, i, 1);
						CV_MAT_ELEM(*object_points2, float, i, 2) = CV_MAT_ELEM(*object_points, float, i, 2);
					}
					for(int i=0; i<successes; ++i)
						CV_MAT_ELEM(*point_counts2, int, i, 0) = CV_MAT_ELEM(*point_counts, int, i, 0);

					// Calibrate the camera and save calibration parameters.
					
					int calib_flags = 0;
					if(!sl_params->cam_dist_model[0])
						calib_flags |= CV_CALIB_ZERO_TANGENT_DIST;
					if(!sl_params->cam_dist_model[1]){
						cvmSet(sl_calib->cam_distortion, 4, 0, 0);
						calib_flags |= CV_CALIB_FIX_K3;
					}
					cvCalibrateCamera2(
						object_points2, image_points2, point_counts2, frame_size, 
						sl_calib->cam_intrinsic, sl_calib->cam_distortion,
						rotation_vectors, translation_vectors, calib_flags);
					
					CvMat* R = cvCreateMat(3, 3, CV_32FC1);
					CvMat r;
					/*for(int i=0; i<successes; ++i){
						sprintf(str,"%s\\%0.2d.png", calibDir, i);
						cvSaveImage(str, calibImages[i]);
						cvGetRow(rotation_vectors, &r, i);
						cvRodrigues2(&r, R, NULL);
						sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
						cvSave(str, R);
					}*/
					sprintf(str,"%s\\cam_intrinsic.xml", calibDir);	
					cvSave(str, sl_calib->cam_intrinsic);
					sprintf(str,"%s\\cam_distortion.xml", calibDir);
					cvSave(str, sl_calib->cam_distortion);
					sprintf(str,"%s\\cam_rotation_vectors.xml", calibDir);
					cvSave(str, rotation_vectors);
					sprintf(str,"%s\\cam_translation_vectors.xml", calibDir);
					cvSave(str, translation_vectors);
					sprintf(str,"%s\\config.xml", calibDir);
					writeConfiguration(str, sl_params);

					// Release allocated resources.
					cvReleaseMat(&object_points2);
					cvReleaseMat(&image_points2);
					cvReleaseMat(&point_counts2);
					cvReleaseMat(&rotation_vectors);
  					cvReleaseMat(&translation_vectors);
					cvReleaseMat(&R);
				}
				else{
					this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
					this->cameraStatusLbl->Text = "ERROR: Not enough targets were found! ";
					cvReleaseMat(&image_points);
					cvReleaseMat(&object_points);
					cvReleaseMat(&point_counts);
					for(int i=0; i<n_boards; i++)
					{
						cvReleaseImage(&imagesBuffer[i]);
						cvReleaseImage(&calibImages[i]);
					}
					delete[] imagesBuffer;
					delete[] calibImages;
					return;
				}

				// Free allocated resources.
				cvReleaseMat(&image_points);
				cvReleaseMat(&object_points);
				cvReleaseMat(&point_counts);
				for(int i=0; i<n_boards; i++)
				{
					cvReleaseImage(&imagesBuffer[i]);
					cvReleaseImage(&calibImages[i]);
				}
				delete[] imagesBuffer;
				delete[] calibImages;

				// Set camera calibration status.
				sl_calib->cam_intrinsic_calib = true;

				// Return without errors.
				this->cameraStatusLbl->ForeColor = System::Drawing::Color::Green;
				this->cameraStatusLbl->Text = "Camera calibration was successful. (" + successes + "/" + n_boards/2 + ")";
				return; 
			 }



	private: System::Void projectorStartBtn_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			this->cameraStartBtn->Enabled = false;
			this->cameraEditBtn->Enabled = false;
			this->projEditBtn->Enabled = false;
			this->projectorStartBtn->Enabled = false;
			this->extrensicEditBtn->Enabled = false;
			this->extrinsicStartBtn->Enabled = false;
			this->checkBox1->Enabled = false;

			this->projStatusLbl->ForeColor = System::Drawing::Color::Red;
			this->projStatusLbl->Text = "Starting Calibration...";
			projectorStatus = "Not Calibrated";

			sl_calib->procam_extrinsic_calib = false;
			this->extrinsicStatusLbl->ForeColor = System::Drawing::Color::Red;
			this->extrinsicStatusLbl->Text = "Not Calibrated";
			extrinsicStatus = "Not Calibrated";

			if(this->checkBox1->Checked)
			{
				sl_calib->cam_intrinsic_calib = false;
				this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
				this->cameraStatusLbl->Text = "Not Calibrated";
				cameraStatus = "Not Calibrated";
			}
			helperType = 2;
			helper->RunWorkerAsync();
		}

	private: void extrinsicCalibrationRun(BackgroundWorker^ worker)
		{
			bool calibrate_both = this->checkBox1->Checked;
			 
			// Create camera calibration directory (clear previous calibration first).
			char str[1024], calibDir[1024];
			if(calibrate_both){
				printf("Creating camera calibration directory (overwrites existing data)...\n");
				sprintf(calibDir, "%s\\calib\\cam", sl_params->outdir);
				sprintf(str, "%s\\calib", sl_params->outdir);
				_mkdir(str);
				_mkdir(calibDir);
				sprintf(str, "rd /s /q \"%s\"", calibDir);
				system(str);
				if(_mkdir(calibDir) != 0){
					projectorStatus = "ERROR: Cannot open output directory!";
					worker->ReportProgress( 0 );
					return;
				}
			}
			else{
				if(!sl_calib->cam_intrinsic_calib){
					projectorStatus = "ERROR: Camera must be calibrated first!";
					worker->ReportProgress( 0 );
					return;
				}
			}

			// Create projector calibration directory (clear previous calibration first).
			sprintf(calibDir, "%s\\calib\\proj", sl_params->outdir);
			sprintf(str, "%s\\calib", sl_params->outdir);
			_mkdir(str);
			_mkdir(calibDir);
			sprintf(str, "rd /s /q \"%s\"", calibDir);
			system(str);
			if(_mkdir(calibDir) != 0){
				projectorStatus = "ERROR: Cannot open output directory!";
				worker->ReportProgress( 0 );
				return;
			}
			
			IplImage* proj_chessboard = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
			int proj_border_cols, proj_border_rows;
			if(generateChessboard(sl_params, proj_chessboard, proj_border_cols, proj_border_rows) == -1){
				projectorStatus = "Chessboard Generation failed.";
				worker->ReportProgress( 0 );
				return;
			}
			cvReleaseImage(&proj_chessboard);

			IplImage** imagesBuffer;
			int n_boards = getImages2(imagesBuffer, 100, this->projectorCalibrationDirLbl->Text, "*.tif");
			if(n_boards<4){
				projectorStatus = "ERROR: At least two images are required!";
				worker->ReportProgress( 0 );
				for(int i=0; i<n_boards; i++)
					cvReleaseImage(&imagesBuffer[i]);
				if(n_boards)
					delete[] imagesBuffer;
				return;
			}
			
			// Evaluate derived camera parameters and allocate storage.
			int cam_board_n            = sl_params->cam_board_w*sl_params->cam_board_h;
			CvSize cam_board_size      = cvSize(sl_params->cam_board_w, sl_params->cam_board_h);
			CvMat* cam_image_points    = cvCreateMat(n_boards*cam_board_n, 2, CV_32FC1);
			CvMat* cam_object_points   = cvCreateMat(n_boards*cam_board_n, 3, CV_32FC1);
			CvMat* cam_point_counts    = cvCreateMat(n_boards, 1, CV_32SC1);
			IplImage** cam_calibImages = new IplImage* [n_boards];

			// Evaluate derived projector parameters and allocate storage.
			int proj_board_n            = sl_params->proj_board_w*sl_params->proj_board_h;
			CvSize proj_board_size      = cvSize(sl_params->proj_board_w, sl_params->proj_board_h);
			CvMat* proj_image_points    = cvCreateMat(n_boards*proj_board_n, 2, CV_32FC1);
			CvMat* proj_point_counts    = cvCreateMat(n_boards, 1, CV_32SC1);
			IplImage** proj_calibImages = new IplImage* [n_boards];

			CvSize frame_size = cvGetSize(imagesBuffer[0]);

			// Initialize capture and allocate storage.
			IplImage* cam_frame_1 = cvCreateImage(frame_size, imagesBuffer[0]->depth, imagesBuffer[0]->nChannels);
			IplImage* cam_frame_2 = cvCreateImage(frame_size, imagesBuffer[0]->depth, imagesBuffer[0]->nChannels);
			IplImage* cam_frame_3 = cvCreateImage(frame_size, imagesBuffer[0]->depth, imagesBuffer[0]->nChannels);
			for(int i=0; i<n_boards; i++)
				cam_calibImages[i]  = cvCreateImage(frame_size, imagesBuffer[0]->depth, imagesBuffer[0]->nChannels);
			for(int i=0; i<n_boards; i++)
				proj_calibImages[i] = cvCreateImage(frame_size, imagesBuffer[0]->depth, imagesBuffer[0]->nChannels);


			// Create a window to display capture frames.
			//cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
			//cvCreateTrackbar("Cam. Gain",  "camWindow", &sl_params->cam_gain,  100, NULL);
			//cvCreateTrackbar("Proj. Gain",  "camWindow", &sl_params->proj_gain,  100, NULL);
			//HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
			//BringWindowToTop(camWindow);
			//cvWaitKey(1);
			sl_params->cam_gain = 35;
			sl_params->proj_gain = 53;
			bool projGainEdit = false;


			// Allocate storage for grayscale images.
			IplImage* cam_frame_1_gray = cvCreateImage(frame_size, IPL_DEPTH_8U, 1);
			IplImage* cam_frame_2_gray = cvCreateImage(frame_size, IPL_DEPTH_8U, 1);

			// Capture live image stream, until "ESC" is pressed or calibration is complete.
			int successes = 0;
			bool goodFrame;
			bool skip = false;
			int cvKey = -1;
			for(int num=0; num<n_boards; num+=2)
			{
				goodFrame = false;
				skip = false;
				// Get next available "safe" frame.
				//cvCopyImage(imagesBuffer[num], cam_frame_1);
				cvScale(imagesBuffer[num], cam_frame_1, 2.*(sl_params->cam_gain/100.), 0);
				

				// Find camera chessboard corners.
				CvPoint2D32f* cam_corners = new CvPoint2D32f[cam_board_n];
				int cam_corner_count;
				int proj_found = 0;
				int cam_found =	detectChessboard(cam_frame_1, cam_board_size, cam_corners, &cam_corner_count);

				// If camera chessboard is found, attempt to detect projector chessboard.
				if(cam_corner_count == cam_board_n){
					//cvCopyImage(imagesBuffer[num+1], cam_frame_2);
					cvScale(imagesBuffer[num+1], cam_frame_2, 2.*(sl_params->proj_gain/100.), 0);

					// Convert frames to grayscale and apply background subtraction.
					cvCvtColor(cam_frame_1, cam_frame_1_gray, CV_RGB2GRAY);
					cvCvtColor(cam_frame_2, cam_frame_2_gray, CV_RGB2GRAY);
					cvSub(cam_frame_1_gray, cam_frame_2_gray, cam_frame_2_gray);

					// Invert chessboard image.
					double min_val, max_val;
					cvMinMaxLoc(cam_frame_2_gray, &min_val, &max_val);
					cvConvertScale(cam_frame_2_gray, cam_frame_2_gray, 
						-255.0/(max_val-min_val), 255.0+((255.0*min_val)/(max_val-min_val)));

					// Find projector chessboard corners.
					CvPoint2D32f* proj_corners = new CvPoint2D32f[proj_board_n];
					int proj_corner_count;
					proj_found = detectChessboard(cam_frame_2_gray, proj_board_size, proj_corners, &proj_corner_count);
					
					// Display current projector tracking results.
					if(!proj_found)
					{
						//cvCvtColor(cam_frame_2_gray, cam_frame_3, CV_GRAY2RGB);
						//cvDrawChessboardCorners(cam_frame_3, proj_board_size, proj_corners, proj_corner_count, proj_found);
						//cvShowImageResampled("camWindow", cam_frame_3, sl_params->window_w, sl_params->window_h);
						sl_params->cam_gain -= 5;
						if(sl_params->cam_gain < 5)
						{
							if(projGainEdit)
							{
								skip = true;
							}
							else
							{
								projGainEdit = true;
								sl_params->proj_gain = 65;
								sl_params->cam_gain = 35;
							}
						}
					}

					// If chessboard is detected, then update calibration lists.
					if(proj_corner_count == proj_board_n){
						
						// Add camera calibration data.
						for(int i=successes*cam_board_n, j=0; j<cam_board_n; ++i,++j){
							CV_MAT_ELEM(*cam_image_points,  float, i, 0) = cam_corners[j].x;
							CV_MAT_ELEM(*cam_image_points,  float, i, 1) = cam_corners[j].y;
							CV_MAT_ELEM(*cam_object_points, float, i, 0) = sl_params->cam_board_w_mm*float(j/sl_params->cam_board_w);
							CV_MAT_ELEM(*cam_object_points, float, i, 1) = sl_params->cam_board_h_mm*float(j%sl_params->cam_board_w);
							CV_MAT_ELEM(*cam_object_points, float, i, 2) = 0.0f;
						}
						CV_MAT_ELEM(*cam_point_counts, int, successes, 0) = cam_board_n;
						cvCopyImage(cam_frame_1, cam_calibImages[successes]);

						// Add projector calibration data.
						for(int i=successes*proj_board_n, j=0; j<proj_board_n; ++i,++j){
							CV_MAT_ELEM(*proj_image_points, float, i, 0) = proj_corners[j].x;
							CV_MAT_ELEM(*proj_image_points, float, i, 1) = proj_corners[j].y;
						}
						CV_MAT_ELEM(*proj_point_counts, int, successes, 0) = proj_board_n;
						cvCopyImage(cam_frame_2, proj_calibImages[successes]);

						// Update display.
						successes++;
						goodFrame = true;
					}

					// Free allocated resources.
					delete[] proj_corners;
				}
				else
				{	
					// Camera chessboard not found, display current camera tracking results.
					//cvDrawChessboardCorners(cam_frame_1, cam_board_size, cam_corners, cam_corner_count, cam_found);
					//cvShowImageResampled("camWindow", cam_frame_1, sl_params->window_w, sl_params->window_h);
					sl_params->cam_gain -= 5;
					if(sl_params->cam_gain < 5)
						skip = true;
				}

				// Free allocated resources.
				delete[] cam_corners;

				// Process user input.
				if(!proj_found && !skip)
				{
					//if(cvKey==27)
					//	break;
					//else if(cvKey=='r')
					//{
						num-=2;
						if(goodFrame)
						{
							successes--;
						}
					//}
				}
				else
				{
					projectorStatus = "Analyzed:" + (num+2)/2 + " of " + n_boards/2;
					worker->ReportProgress( 0 );
					//cvWaitKey(0);

					projGainEdit = false;
					sl_params->cam_gain = 35;
					sl_params->proj_gain = 53;
				}
			}

			// Close the display window.
			//cvDestroyWindow("camWindow");

			// Calibrate projector, if minimum number of frames are available.
			if(successes >= 2){
				
				// Allocate calibration matrices.
				CvMat* cam_object_points2       = cvCreateMat(successes*cam_board_n, 3, CV_32FC1);
				CvMat* cam_image_points2        = cvCreateMat(successes*cam_board_n, 2, CV_32FC1);
				CvMat* cam_point_counts2        = cvCreateMat(successes, 1, CV_32SC1);
				CvMat* cam_rotation_vectors     = cvCreateMat(successes, 3, CV_32FC1);
  				CvMat* cam_translation_vectors  = cvCreateMat(successes, 3, CV_32FC1);
				CvMat* proj_object_points2      = cvCreateMat(successes*proj_board_n, 3, CV_32FC1);
				CvMat* proj_image_points2       = cvCreateMat(successes*proj_board_n, 2, CV_32FC1);
				CvMat* proj_image_points3       = cvCreateMat(successes*proj_board_n, 2, CV_32FC1);
				CvMat* proj_point_counts2       = cvCreateMat(successes, 1, CV_32SC1);
				CvMat* proj_rotation_vectors    = cvCreateMat(successes, 3, CV_32FC1);
  				CvMat* proj_translation_vectors = cvCreateMat(successes, 3, CV_32FC1);

				// Transfer camera calibration data from captured values.
				for(int i=0; i<successes*cam_board_n; ++i){
					CV_MAT_ELEM(*cam_image_points2,  float, i, 0) = CV_MAT_ELEM(*cam_image_points,  float, i, 0);
					CV_MAT_ELEM(*cam_image_points2,  float, i, 1) = CV_MAT_ELEM(*cam_image_points,  float, i, 1);
					CV_MAT_ELEM(*cam_object_points2, float, i, 0) =	CV_MAT_ELEM(*cam_object_points, float, i, 0);
					CV_MAT_ELEM(*cam_object_points2, float, i, 1) =	CV_MAT_ELEM(*cam_object_points, float, i, 1);
					CV_MAT_ELEM(*cam_object_points2, float, i, 2) = CV_MAT_ELEM(*cam_object_points, float, i, 2);
				}
				
				for(int i=0; i<successes*proj_board_n; ++i)
				{
					CV_MAT_ELEM(*proj_image_points3,  float, i, 0) = CV_MAT_ELEM(*proj_image_points,  float, i, 0);
					CV_MAT_ELEM(*proj_image_points3,  float, i, 1) = CV_MAT_ELEM(*proj_image_points,  float, i, 1);
				}

				for(int i=0; i<successes; ++i)
					CV_MAT_ELEM(*cam_point_counts2, int, i, 0) = CV_MAT_ELEM(*cam_point_counts, int, i, 0);
				
				if(calibrate_both){
					
					int calib_flags = 0;
					if(!sl_params->cam_dist_model[0])
						calib_flags |= CV_CALIB_ZERO_TANGENT_DIST;
					if(!sl_params->cam_dist_model[1]){
						cvmSet(sl_calib->cam_distortion, 4, 0, 0);
						calib_flags |= CV_CALIB_FIX_K3;
					}
					cvCalibrateCamera2(
						cam_object_points2, cam_image_points2, cam_point_counts2, 
						cvSize(sl_params->cam_w, sl_params->cam_h), 
						sl_calib->cam_intrinsic, sl_calib->cam_distortion,
						cam_rotation_vectors, cam_translation_vectors, calib_flags);
					
					sprintf(calibDir, "%s\\calib\\cam", sl_params->outdir);
					CvMat* R = cvCreateMat(3, 3, CV_32FC1);
					CvMat r;
					/*for(int i=0; i<successes; ++i){
						sprintf(str,"%s\\%0.2d.png", calibDir, i);
						cvSaveImage(str, cam_calibImages[i]);
						cvGetRow(cam_rotation_vectors, &r, i);
						cvRodrigues2(&r, R, NULL);
						sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
						//cvSave(str, R);
					}*/
					sprintf(str,"%s\\cam_intrinsic.xml", calibDir);	
					cvSave(str, sl_calib->cam_intrinsic);
					sprintf(str,"%s\\cam_distortion.xml", calibDir);
					cvSave(str, sl_calib->cam_distortion);
					sprintf(str,"%s\\cam_rotation_vectors.xml", calibDir);
					cvSave(str, cam_rotation_vectors);
					sprintf(str,"%s\\cam_translation_vectors.xml", calibDir);
					cvSave(str, cam_translation_vectors);
					cvReleaseMat(&R);
					sl_calib->cam_intrinsic_calib = true;
				}

				// Transfer projector calibration data from captured values.
				for(int i=0; i<successes; ++i){

					// Define image points corresponding to projector chessboard (i.e., considering projector as an inverse camera).
					if(!sl_params->proj_invert){
						for(int j=0; j<proj_board_n; ++j){
							CV_MAT_ELEM(*proj_image_points2, float, proj_board_n*i+j, 0) = 
								sl_params->proj_board_w_pixels*float(j%sl_params->proj_board_w) + (float)proj_border_cols + (float)sl_params->proj_board_w_pixels - (float)0.5;
							CV_MAT_ELEM(*proj_image_points2, float, proj_board_n*i+j, 1) = 
								sl_params->proj_board_h_pixels*float(j/sl_params->proj_board_w) + (float)proj_border_rows + (float)sl_params->proj_board_h_pixels - (float)0.5;
						}
					}
					else{
						for(int j=0; j<proj_board_n; ++j){
							CV_MAT_ELEM(*proj_image_points2, float, proj_board_n*i+j, 0) = 
								sl_params->proj_board_w_pixels*float((proj_board_n-j-1)%sl_params->proj_board_w) + (float)proj_border_cols + (float)sl_params->proj_board_w_pixels - (float)0.5;
							CV_MAT_ELEM(*proj_image_points2, float, proj_board_n*i+j, 1) = 
								sl_params->proj_board_h_pixels*float((proj_board_n-j-1)/sl_params->proj_board_w) + (float)proj_border_rows + (float)sl_params->proj_board_h_pixels - (float)0.5;
						}
					}

					// Evaluate undistorted image pixels for both the camera and the projector chessboard corners.
					CvMat* cam_dist_image_points    = cvCreateMat(cam_board_n,  1, CV_32FC2);
					CvMat* cam_undist_image_points  = cvCreateMat(cam_board_n,  1, CV_32FC2);
					CvMat* proj_dist_image_points   = cvCreateMat(proj_board_n, 1, CV_32FC2);
					CvMat* proj_undist_image_points = cvCreateMat(proj_board_n, 1, CV_32FC2);
					for(int j=0; j<cam_board_n; ++j)
						cvSet1D(cam_dist_image_points, j, 
							cvScalar(CV_MAT_ELEM(*cam_image_points, float, cam_board_n*i+j, 0), 
									 CV_MAT_ELEM(*cam_image_points, float, cam_board_n*i+j, 1)));
					for(int j=0; j<proj_board_n; ++j)
						cvSet1D(proj_dist_image_points, j, 
							cvScalar(CV_MAT_ELEM(*proj_image_points, float, proj_board_n*i+j, 0), 
									 CV_MAT_ELEM(*proj_image_points, float, proj_board_n*i+j, 1)));
					cvUndistortPoints(cam_dist_image_points, cam_undist_image_points, 
						sl_calib->cam_intrinsic, sl_calib->cam_distortion, NULL, NULL);
					cvUndistortPoints(proj_dist_image_points, proj_undist_image_points, 
						sl_calib->cam_intrinsic, sl_calib->cam_distortion, NULL, NULL);
					cvReleaseMat(&cam_dist_image_points);
					cvReleaseMat(&proj_dist_image_points);

					// Estimate homography that maps undistorted image pixels to positions on the chessboard.
					CvMat* homography = cvCreateMat(3, 3, CV_32FC1);
					CvMat* cam_src    = cvCreateMat(cam_board_n, 3, CV_32FC1);
					CvMat* cam_dst    = cvCreateMat(cam_board_n, 3, CV_32FC1);
					for(int j=0; j<cam_board_n; ++j){
						CvScalar pd = cvGet1D(cam_undist_image_points, j);
						CV_MAT_ELEM(*cam_src, float, j, 0) = (float)pd.val[0];
						CV_MAT_ELEM(*cam_src, float, j, 1) = (float)pd.val[1];
						CV_MAT_ELEM(*cam_src, float, j, 2) = 1.0;
						CV_MAT_ELEM(*cam_dst, float, j, 0) = CV_MAT_ELEM(*cam_object_points, float, cam_board_n*i+j, 0);
						CV_MAT_ELEM(*cam_dst, float, j, 1) = CV_MAT_ELEM(*cam_object_points, float, cam_board_n*i+j, 1);
						CV_MAT_ELEM(*cam_dst, float, j, 2) = 1.0;
					}
					cvReleaseMat(&cam_undist_image_points);
					cvFindHomography(cam_src, cam_dst, homography);
					cvReleaseMat(&cam_src);
					cvReleaseMat(&cam_dst);

					// Map undistorted projector image corners to positions on the chessboard plane.
					CvMat* proj_src = cvCreateMat(proj_board_n, 1, CV_32FC2);
					CvMat* proj_dst = cvCreateMat(proj_board_n, 1, CV_32FC2);
					for(int j=0; j<proj_board_n; j++)
						cvSet1D(proj_src, j, cvGet1D(proj_undist_image_points, j));
					cvReleaseMat(&proj_undist_image_points);
					cvPerspectiveTransform(proj_src, proj_dst, homography);
					cvReleaseMat(&homography);
					cvReleaseMat(&proj_src);
					
					// Define object points corresponding to projector chessboard.
					for(int j=0; j<proj_board_n; j++){
						CvScalar pd = cvGet1D(proj_dst, j);
						CV_MAT_ELEM(*proj_object_points2, float, proj_board_n*i+j, 0) = (float)pd.val[0];
						CV_MAT_ELEM(*proj_object_points2, float, proj_board_n*i+j, 1) = (float)pd.val[1];
						CV_MAT_ELEM(*proj_object_points2, float, proj_board_n*i+j, 2) = 0.0f;
					}
					cvReleaseMat(&proj_dst);
				}
				for(int i=0; i<successes; ++i)
					CV_MAT_ELEM(*proj_point_counts2, int, i, 0) = CV_MAT_ELEM(*proj_point_counts, int, i, 0);

				// Calibrate the projector and save calibration parameters (if camera calibration is enabled).
				int calib_flags = 0;
				if(!sl_params->proj_dist_model[0])
					calib_flags |= CV_CALIB_ZERO_TANGENT_DIST;
				if(!sl_params->proj_dist_model[1]){
					cvmSet(sl_calib->proj_distortion, 4, 0, 0);
					calib_flags |= CV_CALIB_FIX_K3;
				}
				cvCalibrateCamera2(
					proj_object_points2, proj_image_points2, proj_point_counts2, 
					cvSize(sl_params->proj_w, sl_params->proj_h), 
					sl_calib->proj_intrinsic, sl_calib->proj_distortion,
					proj_rotation_vectors, proj_translation_vectors, calib_flags);
				
				sprintf(calibDir, "%s\\calib\\proj", sl_params->outdir);
				CvMat* R = cvCreateMat(3, 3, CV_32FC1);
				CvMat r;
				/*for(int i=0; i<successes; ++i){
					sprintf(str,"%s\\%0.2d.png", calibDir, i);
					cvSaveImage(str, proj_calibImages[i]);
					sprintf(str,"%s\\%0.2db.png", calibDir, i);
					cvSaveImage(str, cam_calibImages[i]);
					cvGetRow(proj_rotation_vectors, &r, i);
					cvRodrigues2(&r, R, NULL);
					sprintf(str,"%s\\proj_rotation_matrix_%0.2d.xml", calibDir, i);
					//cvSave(str, R);
				}*/
				sprintf(str,"%s\\proj_intrinsic.xml", calibDir);	
				cvSave(str, sl_calib->proj_intrinsic);
				sprintf(str,"%s\\proj_distortion.xml", calibDir);
				cvSave(str, sl_calib->proj_distortion);

				CvMat* proj_rotation_matrix   = cvCreateMat(3, 3, CV_32FC1);
  				CvMat* proj_translation_vector = cvCreateMat(3, 1, CV_32FC1);
				recalibrateExtrinsics(sl_params, sl_calib, successes, cam_image_points2, cam_object_points2, cam_point_counts2, 
						proj_image_points2, proj_object_points2, proj_point_counts2, proj_rotation_matrix, proj_translation_vector);
				
				/*cvStereoCalibrate(proj_object_points2, proj_image_points3, proj_image_points2, proj_point_counts2, sl_calib->cam_intrinsic, sl_calib->cam_distortion,
					sl_calib->proj_intrinsic, sl_calib->proj_distortion, frame_size, proj_rotation_matrix, proj_translation_vector, NULL, NULL, 
					cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5), CV_CALIB_FIX_INTRINSIC);*/

				CvMat* proj_rotation_vector   = cvCreateMat(1, 3, CV_32FC1);
				cvRodrigues2( proj_rotation_matrix, proj_rotation_vector );

				sprintf(str,"%s\\proj_rotation_vector.xml", calibDir);
				cvSave(str, proj_rotation_vector);
				sprintf(str,"%s\\proj_translation_vector.xml", calibDir);
				cvSave(str, proj_translation_vector);
				sprintf(str,"%s\\proj_rotation_vectors.xml", calibDir);
				cvSave(str, proj_rotation_vectors);
				sprintf(str,"%s\\proj_translation_vectors.xml", calibDir);
				cvSave(str, proj_translation_vectors);

				// Save the camera calibration parameters (in case camera is recalibrated).
				sprintf(calibDir, "%s\\calib\\proj", sl_params->outdir);
				/*for(int i=0; i<successes; ++i){
					cvGetRow(cam_rotation_vectors, r, i);
					cvRodrigues2(r, R, NULL);
					sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
					//cvSave(str, R);
				}*/
				sprintf(str,"%s\\cam_intrinsic.xml", calibDir);	
				cvSave(str, sl_calib->cam_intrinsic);
				sprintf(str,"%s\\cam_distortion.xml", calibDir);
				cvSave(str, sl_calib->cam_distortion);
				sprintf(str,"%s\\cam_rotation_vectors.xml", calibDir);
				cvSave(str, cam_rotation_vectors);
				sprintf(str,"%s\\cam_translation_vectors.xml", calibDir);
				cvSave(str, cam_translation_vectors);

				// Save extrinsic calibration of projector-camera system.
				// Note: First calibration image is used to define extrinsic calibration.
				CvMat* cam_object_points_00      = cvCreateMat(cam_board_n, 3, CV_32FC1);
				CvMat* cam_image_points_00       = cvCreateMat(cam_board_n, 2, CV_32FC1);
				CvMat* cam_rotation_vector_00    = cvCreateMat(1, 3, CV_32FC1);
  				CvMat* cam_translation_vector_00 = cvCreateMat(1, 3, CV_32FC1);
				
				if(!calibrate_both)
				{
					for(int i=0; i<cam_board_n; ++i){
						CV_MAT_ELEM(*cam_image_points_00,  float, i, 0) = CV_MAT_ELEM(*cam_image_points2,  float, i, 0);
						CV_MAT_ELEM(*cam_image_points_00,  float, i, 1) = CV_MAT_ELEM(*cam_image_points2,  float, i, 1);
						CV_MAT_ELEM(*cam_object_points_00, float, i, 0) = CV_MAT_ELEM(*cam_object_points2, float, i, 0);
						CV_MAT_ELEM(*cam_object_points_00, float, i, 1) = CV_MAT_ELEM(*cam_object_points2, float, i, 1);
						CV_MAT_ELEM(*cam_object_points_00, float, i, 2) = CV_MAT_ELEM(*cam_object_points2, float, i, 2);
					}
					cvFindExtrinsicCameraParams2(
						cam_object_points_00, cam_image_points_00, 
						sl_calib->cam_intrinsic, sl_calib->cam_distortion,
						cam_rotation_vector_00, cam_translation_vector_00);
					for(int i=0; i<3; i++)
						CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 0, i) = (float)cvmGet(cam_rotation_vector_00, 0, i);
					for(int i=0; i<3; i++)
						CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 1, i) = (float)cvmGet(cam_translation_vector_00, 0, i);
				}
				else{
					for(int i=0; i<3; i++)
						CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 0, i) = (float)cvmGet(cam_rotation_vectors, 0, i);
					for(int i=0; i<3; i++)
						CV_MAT_ELEM(*sl_calib->cam_extrinsic, float, 1, i) = (float)cvmGet(cam_translation_vectors, 0, i);
				}
				
				sprintf(str, "%s\\cam_extrinsic.xml", calibDir);
				cvSave(str, sl_calib->cam_extrinsic);
				for(int i=0; i<3; i++)
					CV_MAT_ELEM(*sl_calib->proj_extrinsic, float, 0, i) = (float)cvmGet(proj_rotation_vector, 0, i);
				for(int i=0; i<3; i++)
					CV_MAT_ELEM(*sl_calib->proj_extrinsic, float, 1, i) = (float)cvmGet(proj_translation_vector, i, 0);
				sprintf(str, "%s\\proj_extrinsic.xml", calibDir);
				cvSave(str, sl_calib->proj_extrinsic);
				sprintf(str,"%s\\config.xml", calibDir);
				writeConfiguration(str, sl_params);

				// Free allocated resources.
				cvReleaseMat(&cam_object_points2);
				cvReleaseMat(&cam_image_points2);
				cvReleaseMat(&cam_point_counts2);
				cvReleaseMat(&cam_rotation_vectors);
  				cvReleaseMat(&cam_translation_vectors);
				cvReleaseMat(&proj_object_points2);
				cvReleaseMat(&proj_image_points2);
				cvReleaseMat(&proj_point_counts2);
				cvReleaseMat(&proj_rotation_vectors);
  				cvReleaseMat(&proj_translation_vectors);
				cvReleaseMat(&R);
				cvReleaseMat(&cam_object_points_00);
				cvReleaseMat(&cam_image_points_00);
				cvReleaseMat(&cam_rotation_vector_00);
  				cvReleaseMat(&cam_translation_vector_00);
				cvReleaseMat(&proj_rotation_matrix);
				cvReleaseMat(&proj_translation_vector);
				cvReleaseMat(&proj_rotation_vector);
			}
			else{
				projectorStatus = "ERROR: At least two detected chessboards are required!";
				worker->ReportProgress( 0 );
				cvReleaseMat(&cam_image_points);
				cvReleaseMat(&cam_object_points);
				cvReleaseMat(&cam_point_counts);
				cvReleaseMat(&proj_image_points);
				cvReleaseMat(&proj_point_counts);
				cvReleaseImage(&proj_chessboard);
				cvReleaseImage(&cam_frame_1);
				cvReleaseImage(&cam_frame_2);
				cvReleaseImage(&cam_frame_3);
				cvReleaseImage(&cam_frame_1_gray);
				cvReleaseImage(&cam_frame_2_gray);
				for(int i=0; i<n_boards; i++)
				{
					cvReleaseImage(&imagesBuffer[i]);
					cvReleaseImage(&cam_calibImages[i]);
					cvReleaseImage(&proj_calibImages[i]);
				}
				delete[] cam_calibImages;
				delete[] proj_calibImages;
				delete[] imagesBuffer;
				return;
			}

			// Update calibration status.
			sl_calib->proj_intrinsic_calib   = true;
			sl_calib->procam_extrinsic_calib = true;

			// Evaluate projector-camera geometry.
			evaluateProCamGeometry(sl_params, sl_calib);

			// Free allocated resources.
			cvReleaseMat(&cam_image_points);
			cvReleaseMat(&cam_object_points);
			cvReleaseMat(&cam_point_counts);
			cvReleaseMat(&proj_image_points);
			cvReleaseMat(&proj_point_counts);
			cvReleaseImage(&proj_chessboard);
			cvReleaseImage(&cam_frame_1);
			cvReleaseImage(&cam_frame_2);
			cvReleaseImage(&cam_frame_3);
			cvReleaseImage(&cam_frame_1_gray);
			cvReleaseImage(&cam_frame_2_gray);
			for(int i=0; i<n_boards; i++)
			{
				cvReleaseImage(&imagesBuffer[i]);
				cvReleaseImage(&cam_calibImages[i]);
				cvReleaseImage(&proj_calibImages[i]);
			}
			delete[] cam_calibImages;
			delete[] proj_calibImages;
			delete[] imagesBuffer;

			// Return without errors.
			cameraStatus = "Calibrated!";
			projectorStatus = "Projector calibration was successful. " + "("+successes+"/"+n_boards/2+")";
			sl_calib->proj_intrinsic_calib   = true;
			
			return;
		 }


private: System::Void extrinsicStartBtn_Click(System::Object^  sender, System::EventArgs^  e) 
		 {

			// Check that projector and camera have already been calibrated.
			if(!sl_calib->cam_intrinsic_calib || !sl_calib->proj_intrinsic_calib){
				this->testOutLbl->ForeColor = System::Drawing::Color::Red;
				this->testOutLbl->Text = "ERROR: Camera and projector must be calibrated first!";
				return;
			}
			
			// Define number of calibration boards (one for now).
			int n_boards = 1;

			// Evaluate derived projector chessboard parameters and allocate storage.
			int proj_board_n                = sl_params->proj_board_w*sl_params->proj_board_h;
			CvSize proj_board_size          = cvSize(sl_params->proj_board_w, sl_params->proj_board_h);
			CvMat* proj_image_points        = cvCreateMat(n_boards*proj_board_n, 2, CV_32FC1);
			CvMat* proj_image_points2        = cvCreateMat(proj_board_n, 2, CV_32FC1);

			// Generate projector calibration chessboard pattern.
			IplImage* proj_chessboard = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
			int proj_border_cols, proj_border_rows;
			if(generateChessboard(sl_params, proj_chessboard, proj_border_cols, proj_border_rows) == -1){
				this->testOutLbl->ForeColor = System::Drawing::Color::Red;
				this->testOutLbl->Text = "Chessboard Generation Failed!";
				return;
			}
			
			// Initialize capture and allocate storage.
			IplImage* pic1 = cvLoadImage(gc2std(this->extrinsicDirLbl->Text).c_str());
			IplImage* cam_frame_1 = cvCreateImage(cvGetSize(pic1), pic1->depth, pic1->nChannels);
			
			sl_params->proj_gain = 53;

			int successes = 0;

			while(true)
			{
				cvCopyImage(pic1, cam_frame_1);
				cvScale(cam_frame_1, cam_frame_1, 2.*(sl_params->proj_gain/100.), 0);
				
				// Find camera chessboard corners.
				CvPoint2D32f* proj_corners = new CvPoint2D32f[proj_board_n];
				int proj_corner_count;
				int proj_found = detectChessboard(cam_frame_1, proj_board_size, proj_corners, &proj_corner_count);

				if(!proj_found)
				{
					sl_params->proj_gain -= 5;
					if(sl_params->proj_gain < 5)
							break;
				}
				if(proj_corner_count == proj_board_n){

					for(int j=0; j<proj_board_n; ++j){
						CV_MAT_ELEM(*proj_image_points2,  float, j, 0) = proj_corners[j].x;
						CV_MAT_ELEM(*proj_image_points2,  float, j, 1) = proj_corners[j].y;
					}
					
					// Add projector calibration data.
					for(int i=successes*proj_board_n, j=0; j<proj_board_n; ++i,++j){
						if(!sl_params->proj_invert){
							CV_MAT_ELEM(*proj_image_points,  float, i, 0) = sl_params->proj_board_w_pixels*float(j%sl_params->proj_board_w) + (float)proj_border_cols + (float)sl_params->proj_board_w_pixels - (float)0.5;
							CV_MAT_ELEM(*proj_image_points,  float, i, 1) = sl_params->proj_board_h_pixels*float(j/sl_params->proj_board_w) + (float)proj_border_rows + (float)sl_params->proj_board_h_pixels - (float)0.5;
						}
						else{
							CV_MAT_ELEM(*proj_image_points,  float, i, 0) = sl_params->proj_board_w_pixels*float((proj_board_n-j-1)%sl_params->proj_board_w) + (float)proj_border_cols + (float)sl_params->proj_board_w_pixels - (float)0.5;
							CV_MAT_ELEM(*proj_image_points,  float, i, 1) = sl_params->proj_board_h_pixels*float((proj_board_n-j-1)/sl_params->proj_board_w) + (float)proj_border_rows + (float)sl_params->proj_board_h_pixels - (float)0.5;
						}
					}
					successes++;
				}

				// Free allocated resources.
				delete[] proj_corners;
				
				// Process user input.
				if(proj_found)
					break;
			}

			double l2err = 0.0;
			double maxErr = 0.0;
			double avg = 0.0;
			// Calibrate projector-camera alignment, if a single frame is available.
			if(successes == 1){
				
				CvMat* proj_rotation    = cvCreateMat(1, 3, CV_32FC1);
				CvMat* proj_translation = cvCreateMat(3, 1, CV_32FC1);
				CvMat* reproj_points = cvCreateMat(proj_board_n, 2, CV_32FC1);
				CvMat* proj_points = cvCreateMat(proj_board_n, 3, CV_32FC1);
				
				int cam_nelems = sl_params->cam_w*sl_params->cam_h;
				
				float point_rows[3];
				float depth_rows;
				for(int j=0; j<proj_board_n; j++)
				{
					float q[3], v[3], w[4];
					int rc = (sl_params->cam_w)*cvRound(CV_MAT_ELEM(*proj_image_points2, float, j, 1))+cvRound(CV_MAT_ELEM(*proj_image_points2, float, j, 0));
					for(int i=0; i<3; i++){
						q[i] = sl_calib->cam_center->data.fl[i];
						v[i] = sl_calib->cam_rays->data.fl[rc+cam_nelems*i];
					}
					for(int i=0; i<4; i++)
						w[i] = sl_calib->proj_row_planes->data.fl[4*(int)CV_MAT_ELEM(*proj_image_points, float, j, 1)+i];
					intersectLineWithPlane3D(q, v, w, point_rows, depth_rows);
					
					for(int i=0; i<3; i++)
						CV_MAT_ELEM(*proj_points, float, j, i) = point_rows[i];
				}
					
				for(int i=0; i<3; i++)
				{
					cvmSet(proj_rotation, i, 0, cvmGet(sl_calib->proj_extrinsic, 0, i));
					cvmSet(proj_translation, i, 0, cvmGet(sl_calib->proj_extrinsic, 1, i));
				}
				cvProjectPoints2(proj_points, proj_rotation, proj_translation, sl_calib->proj_intrinsic, sl_calib->proj_distortion, reproj_points);
				
				cvSub( reproj_points, proj_image_points, reproj_points );
				
				for(int i=0; i<proj_board_n; i++)
					avg += sqrt(pow(CV_MAT_ELEM(*reproj_points, float, i, 0), 2.0f) + pow(CV_MAT_ELEM(*reproj_points, float, i, 1), 2.0f));
				avg /= proj_board_n;

				l2err = cvNorm(reproj_points, 0, CV_L2 );
				maxErr = cvNorm(reproj_points, 0, CV_C );

				cvReleaseMat(&proj_rotation);
				cvReleaseMat(&proj_translation);
				cvReleaseMat(&reproj_points);
				cvReleaseMat(&proj_points);

			}
			else{
				// Free allocated resources.
				
				cvReleaseMat(&proj_image_points);
				cvReleaseMat(&proj_image_points2);
				cvReleaseImage(&proj_chessboard);
				cvReleaseImage(&cam_frame_1);
				cvReleaseImage(&pic1);
				
				this->testOutLbl->ForeColor = System::Drawing::Color::Red;
				this->testOutLbl->Text = "ERROR: At least one chessboard is required!";
				return;
			}

			// Free allocated resources.
			cvReleaseMat(&proj_image_points);
			cvReleaseMat(&proj_image_points2);
			cvReleaseImage(&proj_chessboard);
			cvReleaseImage(&cam_frame_1);
			cvReleaseImage(&pic1);

			String^ myString = "";
			char temp[40];
			sprintf(temp, "L2 Error: %.2g", l2err ); 
			myString += gcnew String(temp) + ", ";
			sprintf(temp, "Max Error: %.2g", maxErr ); 
			myString += gcnew String(temp) + ", ";
			sprintf(temp, "Avg Error: %.2g", avg ); 
			myString += gcnew String(temp);
			
			// Return without errors.
			this->testOutLbl->ForeColor = System::Drawing::Color::Green;
			this->testOutLbl->Text = myString;
			return;		 
		 }

public: void helper_RunWorkerCompleted( Object^ /*sender*/, RunWorkerCompletedEventArgs^ e )
		 {
			this->cameraStartBtn->Enabled = true;
			this->cameraEditBtn->Enabled = true;
			this->projEditBtn->Enabled = true;
			this->projectorStartBtn->Enabled = true;
			this->extrensicEditBtn->Enabled = true;
			this->extrinsicStartBtn->Enabled = true;
			this->checkBox1->Enabled = true;
			
			if(sl_calib->cam_intrinsic_calib && (helperType == 1 || (helperType == 2 && this->checkBox1->Checked)))
			{
				this->cameraStatusLbl->ForeColor = System::Drawing::Color::Green;
				this->cameraStatusLbl->Text = cameraStatus;
			}

			if(sl_calib->proj_intrinsic_calib && helperType == 2)
			{
				this->projStatusLbl->ForeColor = System::Drawing::Color::Green;
				this->projStatusLbl->Text = projectorStatus;
			}

			if(sl_calib->procam_extrinsic_calib)
			{
				String^ myString = "Projector at: ";
				char temp[40];
				sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[0] / 25.4 ); 
				myString += gcnew String(temp) + ", ";
				sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[1] / 25.4 ); 
				myString += gcnew String(temp) + ", ";
				sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[2] / 25.4 ); 
				myString += gcnew String(temp) + " inches";

				this->extrinsicStatusLbl->ForeColor = System::Drawing::Color::Green;
				this->extrinsicStatusLbl->Text = myString;
			}

		 }

		public: void helper_ProgressChanged( Object^ /*sender*/, ProgressChangedEventArgs^ e )
		 {
			if(helperType == 1 || (helperType == 2 && this->checkBox1->Checked))
			{
				this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
				this->cameraStatusLbl->Text = cameraStatus;
			}
			
			if(helperType == 2)
			{
				this->projStatusLbl->ForeColor = System::Drawing::Color::Red;
				this->projStatusLbl->Text = projectorStatus;
			}
		 }

		public: void helper_DoWork(Object^ sender, DoWorkEventArgs^ e )
		 {
				BackgroundWorker^ worker = dynamic_cast<BackgroundWorker^>(sender);

				if(helperType == 2)
					extrinsicCalibrationRun(worker);
		 }

};
}
