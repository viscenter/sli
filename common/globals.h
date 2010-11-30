#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdlib>
#include <string>

#ifdef WIN32
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::IO;
#endif

struct slData
{
	IplImage* proj_chessboard;
	IplImage* proj_frame;
	IplImage** proj_gray_codes;
	int patternNum;
	int gray_ncols;
	int gray_nrows;
	int gray_colshift;
	int gray_rowshift;
};

#ifdef WIN32
static std::string gc2std(System::String^ s)
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

ref class FileInfoNameComparer : IComparer
{
	public: virtual int Compare(Object^ x, Object^ y)
	{
		FileInfo^ objX = (FileInfo^)x;
		FileInfo^ objY = (FileInfo^)y;
		return objX->Name->CompareTo(objY->Name)* -1;
	}
};

static int getImages(IplImage**& imagesBuffer, int numImages, String^ dirLocation, String^ filePattern)
{
	DirectoryInfo^ dir = gcnew DirectoryInfo(dirLocation);
	if(!dir->Exists)
		return 0;

	array<FileInfo^>^ files = dir->GetFiles(filePattern);
	files->Sort(files, gcnew FileInfoNameComparer());
	
	numImages = (numImages < files->Length)?numImages:files->Length;
	if(numImages <= 0)
		return 0;

	imagesBuffer = new IplImage* [numImages];
	for(int i=0; i<numImages; i++)
		imagesBuffer[i] = cvLoadImage(gc2std(files[i]->FullName).c_str());
	
	return numImages;
}

static int getImages2(IplImage**& imagesBuffer, int numImages, String^ dirLocation, String^ filePattern)
{
	DirectoryInfo^ dir = gcnew DirectoryInfo(dirLocation);
	if(!dir->Exists)
		return 0;

	array<FileInfo^>^ files = dir->GetFiles(filePattern);
	files->Sort(files, gcnew FileInfoNameComparer());
	
	numImages = (numImages < files->Length)?numImages:files->Length;
	if(numImages <= 0)
		return 0;

	imagesBuffer = new IplImage* [numImages];
	for(int i=numImages-1; i>=0; i--)
		imagesBuffer[i] = cvLoadImage(gc2std(files[numImages-i-1]->FullName).c_str());
	
	return numImages;
}

static int checkImages(String^ dirLocation, String^ filePattern)
{
	DirectoryInfo^ dir = gcnew DirectoryInfo(dirLocation);
	if(!dir->Exists)
		return 0;

	array<FileInfo^>^ files = dir->GetFiles(filePattern);
	
	return files->Length;
}
#endif

struct reconPtrs
{
	sockaddr_in local;
	sockaddr_in client;
};

struct slParams{

	// Output options.
	char outdir[1024];	            // base output directory
	//char object[1024];              // object name
	bool save;                      // enable/disable saving of image sequence

	// Camera options.
	int  cam_w;                     // camera columns
	int  cam_h;                     // camera rows
	//bool Logitech_9000;             // enable/disable Logitech QuickCam 9000 raw-mode (should be disabled for all other cameras)

	// Projector options.
	int  proj_w;                    // projector columns
	int  proj_h;                    // projector rows
	bool proj_invert;               // enable/disable inverted projector mode (i.e., camera and projector are flipped with respect to each other)

	// Projector-camera gain parameters.
	int cam_gain;                   // scale factor for camera images
	int proj_gain;                  // scale factor for projector images

	// Calibration model options.
	bool cam_dist_model[2];         // enable/disable [tangential, 6th-order radial] distortion components for camera
	bool proj_dist_model[2];        // enable/disable [tangential, 6th-order radial] distortion components for projector

	// Define camera calibration chessboard parameters.
	// Note: Width/height are number of "interior" corners, excluding outside edges.
	int   cam_board_w;              // interior chessboard corners (along width)
	int   cam_board_h;              // interior chessboard corners (along height)
	float cam_board_w_mm;           // physical length of chessboard square (width in mm)
	float cam_board_h_mm;           // physical length of chessboard square (height in mm)

	// Define projector calibration chessboard parameters.
	// Note: Width/height are number of "interior" corners, excluding outside edges.
	int proj_board_w;               // interior chessboard corners (along width)
	int proj_board_h;               // interior chessboard corners (along height)
	int proj_board_w_pixels;        // physical length of chessboard square (width in pixels)
	int proj_board_h_pixels;        // physical length of chessboard square (height in pixels)

	// General options.
	int   mode;                     // structured light reconstruction mode (1 = "ray-plane", 2 = "ray-ray")
	bool  scan_cols;                // enable/disable column scanning
	bool  scan_rows;                // enable/disable row scanning
	int   delay;                    // frame delay between projection and image capture (in ms)
	int   thresh;                   // minimum contrast threshold for decoding (maximum of 255)
	float dist_range[2];            // {minimum, maximum} distance (from camera), otherwise point is rejected
	float dist_reject;              // rejection distance (for outlier removal) if row and column scanning are both enabled (in mm)
	float background_depth_thresh;  // threshold distance for background removal (in mm)	

	// Visualization options.
	bool display;                   // enable/disable display of intermediate results (e.g., image sequence, calibration data, etc.)
	int window_w;                   // camera display window width (height is derived)
	int window_h;                   // camera display window width (derived parameter)
};

// Define structure for structured lighting calibration parameters.
struct slCalib{

	// Camera calibration.
	CvMat* cam_intrinsic;           // camera intrinsic parameter matrix
	CvMat* cam_distortion;          // camera distortion coefficient vector
	CvMat* cam_extrinsic;           // camera extrinsic parameter matrix

	// Projector calibration.
	CvMat* proj_intrinsic;          // projector intrinsic parameter matrix
	CvMat* proj_distortion;         // projector distortion coefficient vector
	CvMat* proj_extrinsic;          // projector extrinsic parameter matrix

	// Projector-camera geometric parameters.
	// Note: All quantities defined in the camera coordinate system.
	CvMat* cam_center;              // camera center of projection
	CvMat* proj_center;             // projector center of projection
	CvMat* cam_rays;                // optical rays for each camera pixel
	CvMat* proj_rays;               // optical rays for each projector pixel
	CvMat* proj_column_planes;      // plane equations describing every projector column
	CvMat* proj_row_planes;         // plane equations describing every projector row

	// Flags to indicate calibration status.
	bool cam_intrinsic_calib;       // flag to indicate state of intrinsic camera calibration
    bool proj_intrinsic_calib;		// flag to indicate state of intrinsic projector calibration
	bool procam_extrinsic_calib;    // flag to indicate state of extrinsic projector-camera calibration

	// Background model (used to segment foreground objects of interest from static background).
	CvMat*    background_depth_map; // background depth map
	IplImage* background_image;     // background image 
	IplImage* background_mask;      // background mask
};

struct reconData
{
	slParams sl_params;
	slCalib sl_calib;
};

struct reconIndirect
{
	reconData data;
};

#endif
