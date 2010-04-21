#pragma once

#include "cvStructuredLight.h"
#include "cvCalibrateProCam.h"
#include "cvUtilProCam.h"
#include "globals.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace projectorController {

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
				this->extrinsicStatusLbl->ForeColor = System::Drawing::Color::Green;
				this->extrinsicStatusLbl->Text = "Calibrated!";
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~calibrationForm()
		{
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
			this->projStatusLbl = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->projectorStartBtn = (gcnew System::Windows::Forms::Button());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->projEditBtn = (gcnew System::Windows::Forms::Button());
			this->projectorCalibrationDirLbl = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->extrinsicStatusLbl = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->extrinsicStartBtn = (gcnew System::Windows::Forms::Button());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->extrensicEditBtn = (gcnew System::Windows::Forms::Button());
			this->extrinsicDirLbl = (gcnew System::Windows::Forms::Label());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// cameraCalibrationDirLbl
			// 
			this->cameraCalibrationDirLbl->AutoEllipsis = true;
			this->cameraCalibrationDirLbl->Location = System::Drawing::Point(160, 25);
			this->cameraCalibrationDirLbl->Name = L"cameraCalibrationDirLbl";
			this->cameraCalibrationDirLbl->Size = System::Drawing::Size(209, 19);
			this->cameraCalibrationDirLbl->TabIndex = 0;
			this->cameraCalibrationDirLbl->Text = L"./cameraCalibration/";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->cameraStatusLbl);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->cameraStartBtn);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->cameraEditBtn);
			this->groupBox1->Controls->Add(this->cameraCalibrationDirLbl);
			this->groupBox1->Location = System::Drawing::Point(12, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(375, 76);
			this->groupBox1->TabIndex = 1;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Camera Intrinsic Calibration";
			// 
			// cameraStatusLbl
			// 
			this->cameraStatusLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->cameraStatusLbl->AutoSize = true;
			this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
			this->cameraStatusLbl->Location = System::Drawing::Point(53, 52);
			this->cameraStatusLbl->Name = L"cameraStatusLbl";
			this->cameraStatusLbl->Size = System::Drawing::Size(74, 13);
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
			this->cameraStartBtn->Location = System::Drawing::Point(272, 47);
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
			this->groupBox2->Controls->Add(this->projStatusLbl);
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Controls->Add(this->projectorStartBtn);
			this->groupBox2->Controls->Add(this->label6);
			this->groupBox2->Controls->Add(this->projEditBtn);
			this->groupBox2->Controls->Add(this->projectorCalibrationDirLbl);
			this->groupBox2->Location = System::Drawing::Point(12, 105);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(375, 76);
			this->groupBox2->TabIndex = 6;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Projector Intrinsic Calibration";
			// 
			// projStatusLbl
			// 
			this->projStatusLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->projStatusLbl->AutoSize = true;
			this->projStatusLbl->ForeColor = System::Drawing::Color::Red;
			this->projStatusLbl->Location = System::Drawing::Point(53, 52);
			this->projStatusLbl->Name = L"projStatusLbl";
			this->projStatusLbl->Size = System::Drawing::Size(74, 13);
			this->projStatusLbl->TabIndex = 5;
			this->projStatusLbl->Text = L"Not Calibrated";
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(6, 52);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(40, 13);
			this->label5->TabIndex = 4;
			this->label5->Text = L"Status:";
			// 
			// projectorStartBtn
			// 
			this->projectorStartBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->projectorStartBtn->Location = System::Drawing::Point(272, 47);
			this->projectorStartBtn->Name = L"projectorStartBtn";
			this->projectorStartBtn->Size = System::Drawing::Size(97, 23);
			this->projectorStartBtn->TabIndex = 3;
			this->projectorStartBtn->Text = L"Run Calibration";
			this->projectorStartBtn->UseVisualStyleBackColor = true;
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
			this->projectorCalibrationDirLbl->AutoEllipsis = true;
			this->projectorCalibrationDirLbl->Location = System::Drawing::Point(160, 25);
			this->projectorCalibrationDirLbl->Name = L"projectorCalibrationDirLbl";
			this->projectorCalibrationDirLbl->Size = System::Drawing::Size(209, 19);
			this->projectorCalibrationDirLbl->TabIndex = 0;
			this->projectorCalibrationDirLbl->Text = L"./projectorCalibration/";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->extrinsicStatusLbl);
			this->groupBox3->Controls->Add(this->label8);
			this->groupBox3->Controls->Add(this->extrinsicStartBtn);
			this->groupBox3->Controls->Add(this->label9);
			this->groupBox3->Controls->Add(this->extrensicEditBtn);
			this->groupBox3->Controls->Add(this->extrinsicDirLbl);
			this->groupBox3->Location = System::Drawing::Point(12, 198);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(375, 76);
			this->groupBox3->TabIndex = 7;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Projector / Camera Extrinsic Calibration";
			// 
			// extrinsicStatusLbl
			// 
			this->extrinsicStatusLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->extrinsicStatusLbl->AutoSize = true;
			this->extrinsicStatusLbl->ForeColor = System::Drawing::Color::Red;
			this->extrinsicStatusLbl->Location = System::Drawing::Point(53, 52);
			this->extrinsicStatusLbl->Name = L"extrinsicStatusLbl";
			this->extrinsicStatusLbl->Size = System::Drawing::Size(74, 13);
			this->extrinsicStatusLbl->TabIndex = 5;
			this->extrinsicStatusLbl->Text = L"Not Calibrated";
			// 
			// label8
			// 
			this->label8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(6, 52);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(40, 13);
			this->label8->TabIndex = 4;
			this->label8->Text = L"Status:";
			// 
			// extrinsicStartBtn
			// 
			this->extrinsicStartBtn->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->extrinsicStartBtn->Location = System::Drawing::Point(272, 47);
			this->extrinsicStartBtn->Name = L"extrinsicStartBtn";
			this->extrinsicStartBtn->Size = System::Drawing::Size(97, 23);
			this->extrinsicStartBtn->TabIndex = 3;
			this->extrinsicStartBtn->Text = L"Run Calibration";
			this->extrinsicStartBtn->UseVisualStyleBackColor = true;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(6, 25);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(89, 13);
			this->label9->TabIndex = 2;
			this->label9->Text = L"Images Directory:";
			// 
			// extrensicEditBtn
			// 
			this->extrensicEditBtn->Location = System::Drawing::Point(104, 20);
			this->extrensicEditBtn->Name = L"extrensicEditBtn";
			this->extrensicEditBtn->Size = System::Drawing::Size(50, 23);
			this->extrensicEditBtn->TabIndex = 1;
			this->extrensicEditBtn->Text = L"Edit";
			this->extrensicEditBtn->UseVisualStyleBackColor = true;
			this->extrensicEditBtn->Click += gcnew System::EventHandler(this, &calibrationForm::extrensicEditBtn_Click);
			// 
			// extrinsicDirLbl
			// 
			this->extrinsicDirLbl->AutoEllipsis = true;
			this->extrinsicDirLbl->Location = System::Drawing::Point(160, 25);
			this->extrinsicDirLbl->Name = L"extrinsicDirLbl";
			this->extrinsicDirLbl->Size = System::Drawing::Size(209, 19);
			this->extrinsicDirLbl->TabIndex = 0;
			this->extrinsicDirLbl->Text = L"./extrinsicCalibration/";
			// 
			// calibrationForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(399, 299);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"calibrationForm";
			this->Text = L"Structured Light Calibration";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->ResumeLayout(false);

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
				System::Windows::Forms::DialogResult result = folderBrowserDialog1->ShowDialog();
				if ( result == ::DialogResult::OK )
				{
					this->extrinsicDirLbl->Text = folderBrowserDialog1->SelectedPath;
				}
			 }

	private: System::Void cameraStartBtn_Click(System::Object^  sender, System::EventArgs^  e) 
			 {
				// Reset camera calibration status (will be set again, if successful).
				sl_calib->cam_intrinsic_calib = false;

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
				int n_boards = getImages(imagesBuffer, 25, "X:/windowsDocuments/laserTests/", "*.tif");
				scanf("%d", &n_boards);
				if(n_boards<2){
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

				// Create a window to display captured frames.
				cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
				cvCreateTrackbar("Cam. Gain", "camWindow", &sl_params->cam_gain, 100, NULL);
				HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
				BringWindowToTop(camWindow);
				cvWaitKey(1);

				// Capture live image stream, until "ESC" is pressed or calibration is complete.
				int successes = 0;
				for(int num=0; num<n_boards; num++)
				{
					// Get next available frame.
					cvScale(imagesBuffer[num], imagesBuffer[num], 2.*(sl_params->cam_gain/100.), 0);

					// Find chessboard corners.
					CvPoint2D32f* corners = new CvPoint2D32f[board_n];
					int corner_count;
					int found = detectChessboard(imagesBuffer[num], board_size, corners, &corner_count);

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
						calibImages[successes] = imagesBuffer[num];
						successes++;
						//printf("+ Captured frame %d of %d.\n", successes, n_boards);
					}
					
					// Display frame.
					cvDrawChessboardCorners(imagesBuffer[num], board_size, corners, corner_count, found);
					cvShowImageResampled("camWindow", imagesBuffer[num], sl_params->window_w, sl_params->window_h);

					// Free allocated resources.
					delete[] corners;

					// Process user input.
					int cvKey = cvWaitKey(1000);
					if(cvKey==27)
						break;
				}

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
					CvMat* r = cvCreateMat(1, 3, CV_32FC1);
					for(int i=0; i<successes; ++i){
						sprintf(str,"%s\\%0.2d.png", calibDir, i);
						cvSaveImage(str, calibImages[i]);
						cvGetRow(rotation_vectors, r, i);
						cvRodrigues2(r, R, NULL);
						sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
						//cvSave(str, R);
					}
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
					cvReleaseMat(&r);
				}
				else{
					this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
					this->cameraStatusLbl->Text = "ERROR: At least two detected chessboards are required!";
					for(int i=0; i<n_boards; i++)
						cvReleaseImage(&imagesBuffer[i]);
					delete[] imagesBuffer;
					return;
				}

				// Free allocated resources.
				cvReleaseMat(&image_points);
				cvReleaseMat(&object_points);
				cvReleaseMat(&point_counts);
				for(int i=0; i<n_boards; i++)
					cvReleaseImage(&imagesBuffer[i]);
				delete[] imagesBuffer;
				delete[] calibImages;

				// Set camera calibration status.
				sl_calib->cam_intrinsic_calib = true;

				// Return without errors.
				this->cameraStatusLbl->ForeColor = System::Drawing::Color::Red;
				this->cameraStatusLbl->Text = "Camera calibration was successful.";
				displayCamCalib(sl_calib);
				return; 
			 }
};
}
