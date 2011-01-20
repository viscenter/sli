#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>

#include <dirent.h>
#include <errno.h>
#include <vector>

#include <cv.h>
#include <highgui.h>
#include <netinet/in.h>

#include "cvCalibrateProCam.h"
#include "../common/cvUtilProCam.h"
#include "../common/globals.h"


char CONFIG_FILE[] = "./config.xml";

using namespace std;

bool calibrate_both = true;
bool verbose = false;

bool extrinsicCalibrationRun(slParams*, slCalib*, string);
bool cameraIntrinsicsRun(slParams*, slCalib*, string); 
bool loadSLConfigXML(slParams*, slCalib*);

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		cerr << "calibLinux picture_directory [-v]" << endl;
		return 1;
	}
	if(argc >= 3 && strcmp(argv[2], "-v") == 0)
	{
		verbose = true;
	}
	slParams sl_params;
	slCalib sl_calib;
	loadSLConfigXML(&sl_params, &sl_calib);
	extrinsicCalibrationRun(&sl_params, &sl_calib, argv[1]);
	return 0;
}

// Converted to Linux
int getFilenames(string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cerr << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

#ifdef __MACH__
int filter(struct dirent* ent)
#else
int filter(const struct dirent* ent)
#endif
{
  return strcasecmp(ent->d_name+strlen(ent->d_name)-4, ".tif") == 0;
}

int getImages3(string dirName, IplImage**& imagesBuffer, int numImages)
{
	struct dirent **namelist = NULL;
	string baseName(dirName);
	int num = scandir( baseName.c_str(), &namelist, filter, alphasort );
	printf("Found %d images in '%s'...\n", num, dirName.c_str());
	if( num < 0 ) return 0;

	int count = 0;
	numImages = (numImages < num)?numImages:num;
	imagesBuffer = new IplImage* [numImages];
	for( int i=0; i<numImages; ++i )
	{
		fprintf(stderr, "Loading image '%s' to %d...\n",
			namelist[num-numImages+i]->d_name, i);
		imagesBuffer[i] = cvLoadImage(
			(baseName + namelist[num-numImages+i]->d_name).c_str());
		if( imagesBuffer[i] ) ++count;
	}

	for( int i=0; i<num; ++i ) free(namelist[i]);
	free(namelist);

	return count;
}

// Converted to Linux
bool cameraIntrinsicsRun(slParams* sl_params, slCalib* sl_calib,string imageDirectory) 
{
	// Reset camera calibration status (will be set again, if successful).
	sl_calib->cam_intrinsic_calib = false;
	sl_calib->proj_intrinsic_calib   = false;
	sl_calib->procam_extrinsic_calib = false;
	
	// Create camera calibration directory (clear previous calibration first).
	char str[1024], calibDir[1024];
	printf("Creating camera calibration directory (overwrites existing data)...\n");
	sprintf(str, "%s/calib", sl_params->outdir);
	sprintf(calibDir, "%s/calib/cam", sl_params->outdir);
	mkdir(str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(calibDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	// Huh?
	sprintf(str, "rm -rf \"%s\"", calibDir);
	system(str);
	if(mkdir(calibDir,  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0){
		cerr << "ERROR: Cannot open output directory!" << endl;
		return false;
	}

	// Prompt user for maximum number of calibration boards.
	
	IplImage** imagesBuffer;
	int n_boards = getImages3(imageDirectory, imagesBuffer, 100);
	
	
	if(n_boards<4){
		cerr << "ERROR: At least two images are required!" << endl;
		for(int i=0; i<n_boards; i++)
			cvReleaseImage(&imagesBuffer[i]);
		if(n_boards)
			delete[] imagesBuffer;
		return false;
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
	//cvCreateTrackbar("Cam. Gain", "camWindow", &sl_params->cam_gain, 100, NULL);
	//HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	//BringWindowToTop(camWindow);
	//cvWaitKey(1);

	// Capture live image stream, until "ESC" is pressed or calibration is complete.
	int successes = 0;
	bool goodFrame;
	IplImage* cam_frame = cvCreateImage(frame_size, imagesBuffer[0]->depth, imagesBuffer[0]->nChannels);
	IplImage* cam_frame_gray = cvCreateImage(frame_size, IPL_DEPTH_8U, 1);
	
	for(int num=0; num<n_boards; num+=2)
	{
		goodFrame = false;
		// Get next available frame.
		cvCopyImage(imagesBuffer[num], cam_frame);
		cvScale(cam_frame, cam_frame, 2.*(sl_params->cam_gain/100.), 0);
		
		// Threshold cam_frame and store in cam_frame_gray
		cvCvtColor(cam_frame, cam_frame_gray, CV_RGB2GRAY);
		cvAdaptiveThreshold(cam_frame_gray, cam_frame_gray,
				255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY,
				(cam_frame_gray->width<cam_frame_gray->height)?cam_frame_gray->width:cam_frame_gray->height);
				
		// Find chessboard corners.
		CvPoint2D32f* corners = new CvPoint2D32f[board_n];
		int corner_count;
		int found = detectChessboard(cam_frame_gray, cam_frame_gray, board_size, corners, &corner_count);

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
		/*if(!found)
		{
			cvDrawChessboardCorners(cam_frame, board_size, corners, corner_count, found);
			cvShowImageResampled("camWindow", cam_frame, sl_params->window_w, sl_params->window_h);
		}*/

		// Free allocated resources.
		delete[] corners;

		// Process user input.
		/*if(!found)
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
		}*/
		cout << "Analyzed:" << (num+2)/2 << " of " << n_boards/2;
		cout << "(" << successes << " successes)" << endl;
	}
	cvReleaseImage(&cam_frame);
	cvReleaseImage(&cam_frame_gray);

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
		//for(int i=0; i<successes; ++i){
		//	sprintf(str,"%s\\%0.2d.png", calibDir, i);
		//	cvSaveImage(str, calibImages[i]);
		//	cvGetRow(rotation_vectors, r, i);
		//	cvRodrigues2(r, R, NULL);
		//	sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
		//	cvSave(str, R);
		//}
		sprintf(str,"%s/cam_intrinsic.xml", calibDir);	
		cvSave(str, sl_calib->cam_intrinsic);
		sprintf(str,"%s/cam_distortion.xml", calibDir);
		cvSave(str, sl_calib->cam_distortion);
		sprintf(str,"%s/cam_rotation_vectors.xml", calibDir);
		cvSave(str, rotation_vectors);
		sprintf(str,"%s/cam_translation_vectors.xml", calibDir);
		cvSave(str, translation_vectors);
		sprintf(str,"%s/config.xml", calibDir);
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
		cerr << "ERROR: Not enough targets were found!" << endl;
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
		return false;
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
	cout << "Camera calibration was successful. (" << successes << "/" << n_boards/2 << ")" << endl;
	return true; 
}

// Converted to Linux
bool extrinsicCalibrationRun(slParams* sl_params, slCalib* sl_calib,string imageDirectory)
	{
		cvNamedWindow("detect");
		 cout << "Starting Calibration..." << endl;
		// Create camera calibration directory (clear previous calibration first).
		char str[1024], calibDir[1024];
		if(calibrate_both){
			printf("Creating camera calibration directory (overwrites existing data)...\n");
			sprintf(str, "%s/calib", sl_params->outdir);
			sprintf(calibDir, "%s/calib/cam", sl_params->outdir);
			mkdir(str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			mkdir(calibDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			//Huh?
			sprintf(str, "rm -rf \"%s\"", calibDir);
			system(str);
			if(mkdir(calibDir,  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0){
				cerr << "ERROR: Cannot open output directory " << calibDir << "!" << endl;
				return false;
			}
		}
		else{
			if(!sl_calib->cam_intrinsic_calib){
				cerr << "ERROR: Camera must be calibrated first!" << endl;
				return false;
			}
		}

		// Create projector calibration directory (clear previous calibration first).
		sprintf(calibDir, "%s/calib/proj", sl_params->outdir);
		sprintf(str, "%s/calib", sl_params->outdir);
		mkdir(str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		mkdir(calibDir,  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		//Huh?
		sprintf(str, "rm -rf \"%s\"", calibDir);
		system(str);
		if(mkdir(calibDir,  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0){
			cerr << "ERROR: Cannot open output directory " << calibDir << "!" << endl;
			return false;
		}
		
		IplImage* proj_chessboard = cvCreateImage(cvSize(sl_params->proj_w, sl_params->proj_h), IPL_DEPTH_8U, 1);
		int proj_border_cols, proj_border_rows;
		if(generateChessboard(sl_params, proj_chessboard, proj_border_cols, proj_border_rows) == -1){
			cerr << "Chessboard Generation failed." << endl;
			return false;
		}
		cvReleaseImage(&proj_chessboard);

		// Load images
		IplImage** imagesBuffer;
		int n_boards = getImages3(imageDirectory.c_str(), imagesBuffer, 100);
		
		if(n_boards<4){
			cerr << "ERROR: At least two images are required!" << endl;
			for(int i=0; i<n_boards; i++)
				cvReleaseImage(&imagesBuffer[i]);
			if(n_boards)
				delete[] imagesBuffer;
			return false;
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
		IplImage* cam_frame_3_gray = cvCreateImage(frame_size, IPL_DEPTH_8U, 1);

		// Capture live image stream, until "ESC" is pressed or calibration is complete.
		int successes = 0;
		bool goodFrame;
		bool skip = false;
		int cvKey = -1;
		bool exitLoop = false;
		for(int num=0; num<n_boards; num+=2)
		{
			goodFrame = false;
			skip = false;
			// Get next available "safe" frame.
			//cvCopyImage(imagesBuffer[num], cam_frame_1);
			cvScale(imagesBuffer[num], cam_frame_1, 2.*(sl_params->cam_gain/100.), 0);
			

			// Transfer cam_frame_1 early so we can threshold it
			cvCvtColor(cam_frame_1, cam_frame_1_gray, CV_RGB2GRAY);
			
			cvErode(cam_frame_1_gray,cam_frame_3_gray,NULL,2);
			cvDilate(cam_frame_3_gray,cam_frame_3_gray,NULL,2);
			
			//TESTING
			//cvShowImageResampled("detect",cam_frame_1_gray, 1024,768);
			//cvWaitKey();
			
			// Perform adaptive thresholding
			// TODO: Should account for diff bit depths
			cvAdaptiveThreshold(cam_frame_3_gray, cam_frame_3_gray,
				255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY,
				(cam_frame_1_gray->width<cam_frame_1_gray->height)?cam_frame_1_gray->width:cam_frame_1_gray->height);
				
			
			// Find camera chessboard corners.
			CvPoint2D32f* cam_corners = new CvPoint2D32f[cam_board_n];
			int cam_corner_count;
			int proj_found = 0;
			int cam_found =	detectChessboard(cam_frame_1_gray, cam_frame_3_gray, cam_board_size, cam_corners, &cam_corner_count);
			
			if(verbose)
			{
				cvDrawChessboardCorners(cam_frame_3_gray, cam_board_size, cam_corners, cam_corner_count, cam_found);
				cvShowImageResampled("detect",cam_frame_3_gray, 1024,768);
				cout << "Press 'c' to Continue or 'i' to Ignore" << endl;
				while(!exitLoop)
				{
					cvKey = cvWaitKey() & 0xFF;
					switch(cvKey)
					{
						case 'i':
							cout << "Ignoring." << endl;
							skip = true;
							exitLoop = true;
							break;
						case 'c':
							cout << "Continuing." << endl;
							exitLoop = true;
							break;
					}
				}
				exitLoop = false;
			}
			// If camera chessboard is found, attempt to detect projector chessboard.
			if((cam_found) && (!skip)){
				//cvCopyImage(imagesBuffer[num+1], cam_frame_2);
				cvScale(imagesBuffer[num+1], cam_frame_2, 2.*(sl_params->proj_gain/100.), 0);

				// Convert frames to grayscale and apply background subtraction.
				// cam_frame_1 done earlier
				cvCvtColor(cam_frame_2, cam_frame_2_gray, CV_RGB2GRAY);
				cvSub(cam_frame_1_gray, cam_frame_2_gray, cam_frame_2_gray);

				// Get min and max
				double min_val, max_val;
				cvMinMaxLoc(cam_frame_2_gray, &min_val, &max_val);
				
				// Threshold
				cvThreshold(cam_frame_2_gray, cam_frame_2_gray, min_val+5, 255, CV_THRESH_BINARY_INV );
				// Invert chessboard image.
				//cvConvertScale(cam_frame_2_gray, cam_frame_2_gray, 
					//-255.0/(max_val-min_val), 255.0+((255.0*min_val)/(max_val-min_val)));

				// Find projector chessboard corners.
				CvPoint2D32f* proj_corners = new CvPoint2D32f[proj_board_n];
				int proj_corner_count;
				proj_found = detectChessboard(cam_frame_2_gray, cam_frame_2_gray, proj_board_size, proj_corners, &proj_corner_count);
				
				if(verbose)
				{
					cvDrawChessboardCorners(cam_frame_2_gray, proj_board_size, proj_corners, proj_corner_count, proj_found);
					cvShowImageResampled("detect",cam_frame_2_gray, 1024,768);
					cout << "Press 'c' to Continue or 'i' to Ignore" << endl;
					while(!exitLoop)
					{
						cvKey = cvWaitKey() & 0xFF;
						switch(cvKey)
						{
							case 'i':
								cout << "Ignoring." << endl;
								skip = true;
								exitLoop = true;
								break;
							case 'c':
								cout << "Continuing." << endl;
								exitLoop = true;
								break;
						}
					}
					exitLoop = false;
				}
				// If chessboard is detected, then update calibration lists.
				if((proj_found) && (!skip)){
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
				else
				{
					//cvCvtColor(cam_frame_2_gray, cam_frame_3, CV_GRAY2RGB);
					//cvDrawChessboardCorners(cam_frame_3, proj_board_size, proj_corners, proj_corner_count, proj_found);
					//cvShowImageResampled("camWindow", cam_frame_3, sl_params->window_w, sl_params->window_h);
					//sl_params->cam_gain -= 5;
					//if(sl_params->cam_gain < 5)
					//{
					//	if(projGainEdit)
					//	{
							skip = true;
					//	}
					//	else
					//	{
					//		projGainEdit = true;
					//		sl_params->proj_gain = 65;
					//		sl_params->cam_gain = 35;
					//	}
					//}
				}


				// Free allocated resources.
				delete[] proj_corners;
			}
			else
			{	
				// Camera chessboard not found, display current camera tracking results.
				//cvDrawChessboardCorners(cam_frame_1, cam_board_size, cam_corners, cam_corner_count, cam_found);
				//cvShowImageResampled("camWindow", cam_frame_1, sl_params->window_w, sl_params->window_h);
				//sl_params->cam_gain -= 5;
				//if(sl_params->cam_gain < 5)
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
				cout << "Analyzed:" << (num+2)/2 << " of " << n_boards/2;
				cout << "(" << successes << " successes)" << endl;
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
				
				sprintf(calibDir, "%s/calib/cam", sl_params->outdir);
				CvMat* R = cvCreateMat(3, 3, CV_32FC1);
				CvMat* r = cvCreateMat(1, 3, CV_32FC1);
				/*for(int i=0; i<successes; ++i){
					sprintf(str,"%s\\%0.2d.png", calibDir, i);
					cvSaveImage(str, cam_calibImages[i]);
					cvGetRow(cam_rotation_vectors, r, i);
					cvRodrigues2(r, R, NULL);
					sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
					//cvSave(str, R);
				}*/
				sprintf(str,"%s/cam_intrinsic.xml", calibDir);	
				cvSave(str, sl_calib->cam_intrinsic);
				sprintf(str,"%s/cam_distortion.xml", calibDir);
				cvSave(str, sl_calib->cam_distortion);
				sprintf(str,"%s/cam_rotation_vectors.xml", calibDir);
				cvSave(str, cam_rotation_vectors);
				sprintf(str,"%s/cam_translation_vectors.xml", calibDir);
				cvSave(str, cam_translation_vectors);
				cvReleaseMat(&R);
				cvReleaseMat(&r);
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
			
			sprintf(calibDir, "%s/calib/proj", sl_params->outdir);
			CvMat* R = cvCreateMat(3, 3, CV_32FC1);
			CvMat* r = cvCreateMat(1, 3, CV_32FC1);
			/*for(int i=0; i<successes; ++i){
				sprintf(str,"%s\\%0.2d.png", calibDir, i);
				cvSaveImage(str, proj_calibImages[i]);
				sprintf(str,"%s\\%0.2db.png", calibDir, i);
				cvSaveImage(str, cam_calibImages[i]);
				cvGetRow(proj_rotation_vectors, r, i);
				cvRodrigues2(r, R, NULL);
				sprintf(str,"%s\\proj_rotation_matrix_%0.2d.xml", calibDir, i);
				//cvSave(str, R);
			}*/
			sprintf(str,"%s/proj_intrinsic.xml", calibDir);	
			cvSave(str, sl_calib->proj_intrinsic);
			sprintf(str,"%s/proj_distortion.xml", calibDir);
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

			sprintf(str,"%s/proj_rotation_vector.xml", calibDir);
			cvSave(str, proj_rotation_vector);
			sprintf(str,"%s/proj_translation_vector.xml", calibDir);
			cvSave(str, proj_translation_vector);
			sprintf(str,"%s/proj_rotation_vectors.xml", calibDir);
			cvSave(str, proj_rotation_vectors);
			sprintf(str,"%s/proj_translation_vectors.xml", calibDir);
			cvSave(str, proj_translation_vectors);

			// Save the camera calibration parameters (in case camera is recalibrated).
			sprintf(calibDir, "%s/calib/proj", sl_params->outdir);
			/*for(int i=0; i<successes; ++i){
				cvGetRow(cam_rotation_vectors, r, i);
				cvRodrigues2(r, R, NULL);
				sprintf(str,"%s\\cam_rotation_matrix_%0.2d.xml", calibDir, i);
				//cvSave(str, R);
			}*/
			sprintf(str,"%s/cam_intrinsic.xml", calibDir);	
			cvSave(str, sl_calib->cam_intrinsic);
			sprintf(str,"%s/cam_distortion.xml", calibDir);
			cvSave(str, sl_calib->cam_distortion);
			sprintf(str,"%s/cam_rotation_vectors.xml", calibDir);
			cvSave(str, cam_rotation_vectors);
			sprintf(str,"%s/cam_translation_vectors.xml", calibDir);
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
			
			sprintf(str, "%s/cam_extrinsic.xml", calibDir);
			cvSave(str, sl_calib->cam_extrinsic);
			for(int i=0; i<3; i++)
				CV_MAT_ELEM(*sl_calib->proj_extrinsic, float, 0, i) = (float)cvmGet(proj_rotation_vector, 0, i);
			for(int i=0; i<3; i++)
				CV_MAT_ELEM(*sl_calib->proj_extrinsic, float, 1, i) = (float)cvmGet(proj_translation_vector, i, 0);
			sprintf(str, "%s/proj_extrinsic.xml", calibDir);
			cvSave(str, sl_calib->proj_extrinsic);
			sprintf(str,"%s/config.xml", calibDir);
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
			cvReleaseMat(&r);
			cvReleaseMat(&cam_object_points_00);
			cvReleaseMat(&cam_image_points_00);
			cvReleaseMat(&cam_rotation_vector_00);
			cvReleaseMat(&cam_translation_vector_00);
			cvReleaseMat(&proj_rotation_matrix);
			cvReleaseMat(&proj_translation_vector);
			cvReleaseMat(&proj_rotation_vector);
		}
		else{
			cerr << "ERROR: At least two detected chessboards are required!" << endl;
			cvReleaseMat(&cam_image_points);
			cvReleaseMat(&cam_object_points);
			cvReleaseMat(&cam_point_counts);
			cvReleaseMat(&proj_image_points);
			cvReleaseMat(&proj_point_counts);
			cvReleaseImage(&proj_chessboard);
			cvReleaseImage(&cam_frame_1);
			cvReleaseImage(&cam_frame_2);
			cvReleaseImage(&cam_frame_1_gray);
			cvReleaseImage(&cam_frame_2_gray);
			cvReleaseImage(&cam_frame_3_gray);
	cvDestroyWindow("detect");
			for(int i=0; i<n_boards; i++)
			{
				cvReleaseImage(&imagesBuffer[i]);
				cvReleaseImage(&cam_calibImages[i]);
				cvReleaseImage(&proj_calibImages[i]);
			}
			delete[] cam_calibImages;
			delete[] proj_calibImages;
			delete[] imagesBuffer;
			return false;
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
		cvReleaseImage(&cam_frame_1_gray);
		cvReleaseImage(&cam_frame_2_gray);
		cvReleaseImage(&cam_frame_3_gray);
	cvDestroyWindow("detect");
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
		cout << "Calibrated!" << endl;
		cout << "Projector calibration was successful. " << "(" << successes << "/" <<n_boards/2 <<")" << endl;
		sl_calib->proj_intrinsic_calib   = true;
		
		if(sl_calib->procam_extrinsic_calib)
		{
			cout << "Projector at: ";
			char temp[40];
			sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[0] / 25.4 ); 
			cout << temp << ", ";
			sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[1] / 25.4 ); 
			cout << temp << ", ";
			sprintf(temp, "%.2g", sl_calib->proj_center->data.fl[2] / 25.4 ); 
			cout << temp << " inches" << endl;
		}
		return true;
	 }
bool loadSLConfigXML(slParams* sl_params, slCalib* sl_calib)
{
	char* configFile = CONFIG_FILE;
	
	// Read structured lighting parameters from configuration file.
	FILE* pFile = fopen(configFile, "r");
	if(pFile != NULL){
		fclose(pFile);
		cout << "Reading configuration file..." << endl ;
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
	sprintf(str1, "%s/calib/cam/cam_intrinsic.xml",  sl_params->outdir);
	sprintf(str2, "%s/calib/cam/cam_distortion.xml", sl_params->outdir);
	if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
		sl_calib->cam_intrinsic  = (CvMat*)cvLoad(str1);
		sl_calib->cam_distortion = (CvMat*)cvLoad(str2);
		sl_calib->cam_intrinsic_calib = true;
		cout << "Loaded previous intrinsic camera calibration." << endl;
	}

	// Load intrinsic projector calibration parameters (if found);
	sprintf(str1, "%s/calib/proj/proj_intrinsic.xml",  sl_params->outdir);
	sprintf(str2, "%s/calib/proj/proj_distortion.xml", sl_params->outdir);
	if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
		sl_calib->proj_intrinsic  = (CvMat*)cvLoad(str1);
		sl_calib->proj_distortion = (CvMat*)cvLoad(str2);
		sl_calib->proj_intrinsic_calib = true;
		cout << "Loaded previous intrinsic projector calibration." << endl;
	}
	
	// Load extrinsic projector-camera parameters (if found).
	sprintf(str1, "%s/calib/proj/cam_extrinsic.xml",  sl_params->outdir);
	sprintf(str2, "%s/calib/proj/proj_extrinsic.xml", sl_params->outdir);
	if( (sl_calib->cam_intrinsic_calib && sl_calib->proj_intrinsic_calib) &&
		( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ) ){
		sl_calib->cam_extrinsic  = (CvMat*)cvLoad(str1);
		sl_calib->proj_extrinsic = (CvMat*)cvLoad(str2);
		sl_calib->procam_extrinsic_calib = true;
		evaluateProCamGeometry(sl_params, sl_calib);
		cout << "Loaded previous extrinsic projector-camera calibration." << endl;
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
	// Run test button?
	/*
	private: void extrinsicStartBtn_Click(System::Object^  sender, System::EventArgs^  e) 
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
					
				cvGetRow(sl_calib->proj_extrinsic, proj_rotation, 0);
				for(int i=0; i<3; i++)
					cvmSet(proj_translation, i, 0, cvmGet(sl_calib->proj_extrinsic, 1, i));
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
*/
