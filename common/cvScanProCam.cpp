// cvScanProCam.cpp: Functions for structured light scanning.
//
// Overview:
//   This file implements the functions for structured lighting. The current implementation
//   supports a single projector-camera pair. The projector-camera system must be calibrated 
//   prior to running the scanning function. A 3D point cloud and depth map are recovered by
//   optical triangulation. Three scanning modes are implemented, including: (1) encoding only 
//   the projector columns, (2) encoding only the projector rows, (3) encoding both rows and 
//   columns. Two reconstruction methods are implemented, including: (1) "ray-plane" 
//   triangulation and (2) "ray-ray" triangulation. In the former, each optical ray from the 
//   camera is intersected with the corresponding projector column and/or row. In the later,
//   the corresponding optical rays from the camera and projector are intersected; in this 
//   case, the 3D point is assigned as the closest point to the two (generally skewed) rays.
//   
// Details:
//   Please read the SIGGRAPH 2009 course notes for additional details.
//
//     Douglas Lanman and Gabriel Taubin
//     "Build Your Own 3D Scanner: 3D Photography for Beginners"
//     ACM SIGGRAPH 2009 Course Notes
//
// Author:
//   Douglas Lanman
//   Brown University
//   July 2009

#include "stdafx.h"
#include "globals.h"
#include "cvScanProCam.h"
#include "cvUtilProCam.h"
#include <map>

using namespace std;

// Generate Gray codes.
int generateGrayCodes(int width, int height, 
					  IplImage**& gray_codes, 
					  int& n_cols, int& n_rows,
					  int& col_shift, int& row_shift, 
					  bool sl_scan_cols, bool sl_scan_rows){

	// Determine number of required codes and row/column offsets.
	if(sl_scan_cols){
		n_cols = (int)ceil(log2(width));
		col_shift = (int)floor((pow(2.0,n_cols)-width)/2);
	}
	else{
		n_cols = 0;
		col_shift = 0;
	}
	if(sl_scan_rows){
		n_rows = (int)ceil(log2(height));
		row_shift = (int)floor((pow(2.0,n_rows)-height)/2);
	}
	else{
		n_rows = 0;
		row_shift = 0;
	}	

	// Allocate Gray codes.
	gray_codes = new IplImage* [n_cols+n_rows+1];
	for(int i=0; i<(n_cols+n_rows+1); i++)
		gray_codes[i] = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
	int step = gray_codes[0]->widthStep/sizeof(uchar);

	// Define first code as a white image.
	cvSet(gray_codes[0], cvScalar(255));

	// Define Gray codes for projector columns.
	for(int c=0; c<width; c++){
		for(int i=0; i<n_cols; i++){
			uchar* data = (uchar*)gray_codes[i+1]->imageData;
			if(i>0)
				data[c] = (((c+col_shift) >> (n_cols-i-1)) & 1)^(((c+col_shift) >> (n_cols-i)) & 1);
			else
				data[c] = (((c+col_shift) >> (n_cols-i-1)) & 1);
			data[c] *= 255;
			for(int r=1; r<height; r++)
				data[r*step+c] = data[c];	
		}
	}

	// Define Gray codes for projector rows.
	for(int r=0; r<height; r++){
		for(int i=0; i<n_rows; i++){
			uchar* data = (uchar*)gray_codes[i+n_cols+1]->imageData;
			if(i>0)
				data[r*step] = (((r+row_shift) >> (n_rows-i-1)) & 1)^(((r+row_shift) >> (n_rows-i)) & 1);
			else
				data[r*step] = (((r+row_shift) >> (n_rows-i-1)) & 1);
			data[r*step] *= 255;
			for(int c=1; c<width; c++)
				data[r*step+c] = data[r*step];	
		}
	}

	// Return without errors.
	return 0;
}

// Decode Gray codes.
int decodeGrayCodes(int proj_width, int proj_height,
					IplImage**& gray_codes, 
					IplImage*& decoded_cols,
					IplImage*& decoded_rows,
					IplImage*& mask,
					int& n_cols, int& n_rows,
					int& col_shift, int& row_shift, 
					int sl_thresh){

	// Extract width and height of images.
	int cam_width  = gray_codes[0]->width;
	int cam_height = gray_codes[0]->height;

	// Allocate temporary variables.
	IplImage* gray_1      = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);
	IplImage* gray_2      = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);
	IplImage* bit_plane_1 = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);
	IplImage* bit_plane_2 = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);
	IplImage* temp        = cvCreateImage(cvSize(cam_width, cam_height), IPL_DEPTH_8U,  1);

	// Initialize image mask (indicates reconstructed pixels).
	cvSet(mask, cvScalar(0));

	// Decode Gray codes for projector columns.
	cvZero(decoded_cols);
	for(int i=0; i<n_cols; i++){

		// Decode bit-plane and update mask.
		cvCvtColor(gray_codes[2*(i+1)],   gray_1, CV_RGB2GRAY);
		cvCvtColor(gray_codes[2*(i+1)+1], gray_2, CV_RGB2GRAY);
		cvAbsDiff(gray_1, gray_2, temp);
		cvCmpS(temp, sl_thresh, temp, CV_CMP_GE);
		cvOr(temp, mask, mask);
		cvCmp(gray_1, gray_2, bit_plane_2, CV_CMP_GE);

		// Convert from gray code to decimal value.
		if(i>0)
			cvXor(bit_plane_1, bit_plane_2, bit_plane_1);
		else
			cvCopyImage(bit_plane_2, bit_plane_1);
		cvAddS(decoded_cols, cvScalar(pow(2.0,n_cols-i-1)), decoded_cols, bit_plane_1);
	}
	cvSubS(decoded_cols, cvScalar(col_shift), decoded_cols);

	// Decode Gray codes for projector rows.
	cvZero(decoded_rows);
	for(int i=0; i<n_rows; i++){

		// Decode bit-plane and update mask.
		cvCvtColor(gray_codes[2*(i+n_cols+1)],   gray_1, CV_RGB2GRAY);
		cvCvtColor(gray_codes[2*(i+n_cols+1)+1], gray_2, CV_RGB2GRAY);
		cvAbsDiff(gray_1, gray_2, temp);
		cvCmpS(temp, sl_thresh, temp, CV_CMP_GE);
		cvOr(temp, mask, mask);
		cvCmp(gray_1, gray_2, bit_plane_2, CV_CMP_GE);

		// Convert from gray code to decimal value.
		if(i>0)
			cvXor(bit_plane_1, bit_plane_2, bit_plane_1);
		else
			cvCopyImage(bit_plane_2, bit_plane_1);
		cvAddS(decoded_rows, cvScalar(pow(2.0,n_rows-i-1)), decoded_rows, bit_plane_1);
	}
	cvSubS(decoded_rows, cvScalar(row_shift), decoded_rows);

	// Eliminate invalid column/row estimates.
    // Note: This will exclude pixels if either the column or row is missing or erroneous.
	cvCmpS(decoded_cols, proj_width-1,  temp, CV_CMP_LE);
	cvAnd(temp, mask, mask);
	cvCmpS(decoded_cols, 0,  temp, CV_CMP_GE);
	cvAnd(temp, mask, mask);
	cvCmpS(decoded_rows, proj_height-1, temp, CV_CMP_LE);
	cvAnd(temp, mask, mask);
	cvCmpS(decoded_rows, 0,  temp, CV_CMP_GE);
	cvAnd(temp, mask, mask);
	cvNot(mask, temp);
	cvSet(decoded_cols, cvScalar(NULL), temp);
	cvSet(decoded_rows, cvScalar(NULL), temp);

	// Free allocated resources.
	cvReleaseImage(&gray_1);
	cvReleaseImage(&gray_2);
	cvReleaseImage(&bit_plane_1);
	cvReleaseImage(&bit_plane_2);
	cvReleaseImage(&temp);

	// Return without errors.
	return 0;
}
// Display a structured lighting decoding result (i.e., projector column/row to camera pixel correspondences).
int displayDecodingResults(IplImage*& decoded_cols, 
						   IplImage*& decoded_rows, 
						   IplImage*& mask,
						   struct slParams* sl_params){

	// Create a window to display correspondences.
	cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
#ifdef WIN32
	HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	BringWindowToTop(camWindow);
#endif
	cvWaitKey(1);

	// Allocate image arrays.
	IplImage* temp_1 = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 1);
	IplImage* temp_2 = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 3);
	
	// Display decoded projector columns.
	if(sl_params->scan_cols){
		cvConvertScale(decoded_cols, temp_1, 255.0/sl_params->proj_w, 0);
		colorizeWinter(temp_1, temp_2, mask);
		cvShowImageResampled("camWindow", temp_2, sl_params->window_w, sl_params->window_h);
		printf("Displaying the decoded columns; press any key (in 'camWindow') to continue.\n");
		cvWaitKey(0);
	}

	// Display decoded projector rows.
	if(sl_params->scan_rows){
		cvConvertScale(decoded_rows, temp_1, 255.0/sl_params->proj_h, 0);
		colorizeWinter(temp_1, temp_2, mask);
		cvShowImageResampled("camWindow", temp_2, sl_params->window_w, sl_params->window_h);
		printf("Displaying the decoded rows; Press any key (in 'camWindow') to continue.\n");
		cvWaitKey(0);
	}

	// Free allocated resources.
	cvReleaseImage(&temp_1);
	cvReleaseImage(&temp_2);
	
	// Return without errors.
	cvDestroyWindow("camWindow");
	return 0;
}

// Display a depth map.
int displayDepthMap(CvMat*& depth_map,
					IplImage*& mask,
				    struct slParams* sl_params){

	// Create a window to display depth map.
	cvNamedWindow("camWindow", CV_WINDOW_AUTOSIZE);
#ifdef WIN32
	HWND camWindow = (HWND)cvGetWindowHandle("camWindow");
	BringWindowToTop(camWindow);
#endif
	cvWaitKey(1);

	// Allocate image arrays.
	IplImage* temp_1 = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 1);
	IplImage* temp_2 = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 3);

	// Create depth map image (scaled to distance range).
	for(int r=0; r<sl_params->cam_h; r++){
		for(int c=0; c<sl_params->cam_w; c++){
			uchar* temp_1_data = (uchar*)(temp_1->imageData + r*temp_1->widthStep);
			uchar* mask_data   = (uchar*)(mask->imageData + r*mask->widthStep);
			if(mask_data[c])
				temp_1_data[c] = 
					255-int(255.0*(depth_map->data.fl[sl_params->cam_w*r+c]-sl_params->dist_range[0])/
						(sl_params->dist_range[1]-sl_params->dist_range[0]));
			else
				temp_1_data[c] = 0;
		}
	}
	
	// Display depth map.
	colorizeWinter(temp_1, temp_2, mask);
	cvShowImageResampled("camWindow", temp_2, sl_params->window_w, sl_params->window_h);
	printf("Displaying the depth map; press any key (in 'camWindow') to continue.\n");
	cvWaitKey(0);

	// Release allocated resources.
	cvReleaseImage(&temp_1);
	cvReleaseImage(&temp_2);

	// Return without errors.
	cvDestroyWindow("camWindow");
	return 0;
}

// Reconstruct the point cloud and the depth map from a structured light sequence.
int reconstructStructuredLight(struct slParams* sl_params, 
					           struct slCalib* sl_calib,
							   IplImage*& texture_image,
							   IplImage*& gray_decoded_cols, 
							   IplImage*& gray_decoded_rows, 
						       IplImage*& gray_mask,
							   CvMat*&    points,
							   CvMat*&    colors,
							   CvMat*&    depth_map,
							   CvMat*&    mask){
	
	// Define pointers to various image data elements (for fast pixel access).
	int cam_nelems                 = sl_params->cam_w*sl_params->cam_h;
	int proj_nelems                = sl_params->proj_w*sl_params->proj_h;
	uchar*  background_mask_data   = (uchar*)sl_calib->background_mask->imageData;
	int     background_mask_step   = sl_calib->background_mask->widthStep/sizeof(uchar);
	uchar*  gray_mask_data         = (uchar*)gray_mask->imageData;
	int     gray_mask_step         = gray_mask->widthStep/sizeof(uchar);
	ushort* gray_decoded_cols_data = (ushort*)gray_decoded_cols->imageData;
	int     gray_decoded_cols_step = gray_decoded_cols->widthStep/sizeof(ushort);
	ushort* gray_decoded_rows_data = (ushort*)gray_decoded_rows->imageData;
	int     gray_decoded_rows_step = gray_decoded_rows->widthStep/sizeof(ushort);

	// Create a temporary copy of the background depth map.
	CvMat* background_depth_map = cvCloneMat(sl_calib->background_depth_map);

	// By default, disable all pixels.
	cvZero(mask);

	// Reconstruct point cloud and depth map.
	for(int r=0; r<sl_params->cam_h; r++){
		for(int c=0; c<sl_params->cam_w; c++){

			// Reconstruct current point, if mask is non-zero.
			if(gray_mask_data[r*gray_mask_step+c]){

				// Reconstruct using either "ray-plane" or "ray-ray" triangulation.
				if(sl_params->mode == 1){

					// Allocate storage for row/column reconstructed points and depths.
					float point_cols[3], point_rows[3];
					float depth_cols, depth_rows;
				
					// Intersect camera ray with corresponding projector column.
					if(sl_params->scan_cols){
						float q[3], v[3], w[4];
						int rc = (sl_params->cam_w)*r+c;
						for(int i=0; i<3; i++){
							q[i] = sl_calib->cam_center->data.fl[i];
							v[i] = sl_calib->cam_rays->data.fl[rc+cam_nelems*i];
						}
						int corresponding_column = gray_decoded_cols_data[r*gray_decoded_cols_step+c];
						for(int i=0; i<4; i++)
							w[i] = sl_calib->proj_column_planes->data.fl[4*corresponding_column+i];
						intersectLineWithPlane3D(q, v, w, point_cols, depth_cols);
					}

					// Intersect camera ray with corresponding projector row.
					if(sl_params->scan_rows){
						float q[3], v[3], w[4];
						int rc = (sl_params->cam_w)*r+c;
						for(int i=0; i<3; i++){
							q[i] = sl_calib->cam_center->data.fl[i];
							v[i] = sl_calib->cam_rays->data.fl[rc+cam_nelems*i];
						}
						int corresponding_row = gray_decoded_rows_data[r*gray_decoded_rows_step+c];
						for(int i=0; i<4; i++)
							w[i] = sl_calib->proj_row_planes->data.fl[4*corresponding_row+i];
						intersectLineWithPlane3D(q, v, w, point_rows, depth_rows);
					}

					// Average points of intersection (if row and column scanning are both enabled).
					// Note: Eliminate any points that differ between row and column reconstructions.
					if( sl_params->scan_cols && sl_params->scan_rows){
						if(abs(depth_cols-depth_rows) < sl_params->dist_reject){
							depth_map->data.fl[sl_params->cam_w*r+c] = (depth_cols+depth_rows)/2;
							for(int i=0; i<3; i++)
								points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = (point_cols[i]+point_rows[i])/2;
						}
						else
							gray_mask_data[r*gray_mask_step+c] = 0;
					}
					else if(sl_params->scan_cols){
						depth_map->data.fl[sl_params->cam_w*r+c] = depth_cols;
						for(int i=0; i<3; i++)
							points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = point_cols[i];
					}
					else if(sl_params->scan_rows){
						depth_map->data.fl[sl_params->cam_w*r+c] = depth_rows;
						for(int i=0; i<3; i++)
							points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = point_rows[i];
					}
					else
						gray_mask_data[r*gray_mask_step+c] = 0;
				}
				else{

					// Reconstruct surface using "ray-ray" triangulation.
					int corresponding_column = gray_decoded_cols_data[r*gray_decoded_cols_step+c];
					int corresponding_row    = gray_decoded_rows_data[r*gray_decoded_rows_step+c];
					float q1[3], q2[3], v1[3], v2[3], point[3], depth = 0;
					int rc_cam  = (sl_params->cam_w)*r+c;
					int rc_proj = (sl_params->proj_w)*corresponding_row+corresponding_column;
					for(int i=0; i<3; i++){
						q1[i] = sl_calib->cam_center->data.fl[i];
						q2[i] = sl_calib->proj_center->data.fl[i];
						v1[i] = sl_calib->cam_rays->data.fl[rc_cam+cam_nelems*i];
						v2[i] = sl_calib->proj_rays->data.fl[rc_proj+proj_nelems*i];
					}
					intersectLineWithLine3D(q1, v1, q2, v2, point);
					for(int i=0; i<3; i++)
						depth += v1[i]*(point[i]-q1[i]);
					depth_map->data.fl[rc_cam] = depth;
					for(int i=0; i<3; i++)
						points->data.fl[rc_cam+cam_nelems*i] = point[i];
				}

				// Assign color using provided texture image.
				// Note: Color channels are ordered as RGB, rather than OpenCV's default BGR.
				uchar* texture_image_data = (uchar*)(texture_image->imageData + r*texture_image->widthStep);
				for(int i=0; i<3; i++)
					colors->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = (float)texture_image_data[3*c+(2-i)]/(float)255.0;

				// Update valid pixel mask (e.g., points will only be saved if valid).
				mask->data.fl[sl_params->cam_w*r+c] = 1;

				// Reject any points outside near/far clipping planes.
				if(depth_map->data.fl[sl_params->cam_w*r+c] < sl_params->dist_range[0] ||
				   depth_map->data.fl[sl_params->cam_w*r+c] > sl_params->dist_range[1]){
					gray_mask_data[r*gray_mask_step+c] = 0;
					mask->data.fl[sl_params->cam_w*r+c] = 0;
					depth_map->data.fl[sl_params->cam_w*r+c] = 0;
					for(int i=0; i<3; i++)
						points->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = 0;
					for(int i=0; i<3; i++)
						colors->data.fl[sl_params->cam_w*r+c+cam_nelems*i] = 0;
				}
			}
		}
	}

	// Release allocated resources.
	cvReleaseMat(&background_depth_map);

	// Return without errors.
	return 0;
}

// downsample reconstructed scene into points visible from camera/projector plane
void downsamplePoints(struct slParams* sl_params, struct slCalib* sl_calib, CvMat* orig_points, CvMat*& mask, CvMat*& resample_points, CvMat*& depth_map)
{
	//depth_map = NULL;
	int cam_nelems = sl_params->cam_w*sl_params->cam_h;
	int proj_nelems = sl_params->proj_w*sl_params->proj_h;
	CvMat* proj_rotation    = cvCreateMat(1, 3, CV_32FC1);
	CvMat* proj_translation = cvCreateMat(3, 1, CV_32FC1);
	CvMat* reproj_points = cvCreateMat(cam_nelems, 2, CV_32FC1);
	CvMat* proj_points = cvCreateMat(3, proj_nelems, CV_32FC1);
	CvMat* proj_mask = cvCreateMat(1, proj_nelems, CV_32FC1);
	CvMat* bins = cvCreateMat(10, proj_nelems, CV_32FC1);
	CvMat* counts = cvCreateMat(1, proj_nelems, CV_32FC1);
	cvZero(counts);

	for(int i=0; i<3; i++)
	{
		cvmSet(proj_rotation, 0, i, cvmGet(sl_calib->proj_extrinsic, 0, i));
		cvmSet(proj_translation, i, 0, cvmGet(sl_calib->proj_extrinsic, 1, i));
	}

  printf("orig_points:\t%d x %d x %d\n", CV_MAT_CN(orig_points->type), orig_points->rows, orig_points->cols);
  printf("reproj_points:\t%d x %d x %d\n", CV_MAT_CN(reproj_points->type), reproj_points->rows, reproj_points->cols);

  // project reconstructed 3D points onto 2D camera image plane
  // orig_points: cam_nelems*3
  // reproj_points: cam_nelems*2
	cvProjectPoints2(orig_points, proj_rotation, proj_translation, sl_calib->proj_intrinsic, sl_calib->proj_distortion, reproj_points);

  // transpose reproj_points from cam_nelems*2 -> 2*cam_nelems
  CvMat* temp_transpose = cvCreateMat(2, cam_nelems, CV_32FC1); 
  cvTranspose(reproj_points, temp_transpose);
  cvReleaseMat(&reproj_points);
  reproj_points = temp_transpose;

  // transpose orig_points from cam_nelems*3 -> 3*cam_nelems
  temp_transpose = cvCreateMat(3, cam_nelems, CV_32FC1); 
  cvTranspose(orig_points, temp_transpose);
  cvReleaseMat(&orig_points);
  orig_points = temp_transpose;
  
  // copy reproj_points into bins and counts
  // orig_points: 3*cam_nelems
  // reproj_points: 2*cam_nelems
	int x,y, newCount;
	for(int r=0; r<sl_params->cam_h; r++)
	{
		for(int c=0; c<sl_params->cam_w; c++)
		{
			if(mask->data.fl[c+r*sl_params->cam_w] != 0)
			{
				x = cvRound(CV_MAT_ELEM(*reproj_points, float, 0, c+r*sl_params->cam_w));
				y = cvRound(CV_MAT_ELEM(*reproj_points, float, 1, c+r*sl_params->cam_w));
				if((x >= 0 && x < sl_params->proj_w) && (y >=0 && y < sl_params->proj_h) && (counts->data.fl[x + y*sl_params->proj_w] < 10))
				{
					newCount = (int)counts->data.fl[x + y*sl_params->proj_w];
					while(newCount > 0)
					{
						if(orig_points->data.fl[c+r*sl_params->cam_w+cam_nelems*2] < orig_points->data.fl[(int)bins->data.fl[x + y*sl_params->proj_w + (newCount-1)*proj_nelems]+cam_nelems*2])
							bins->data.fl[x + y*sl_params->proj_w + newCount*proj_nelems] = bins->data.fl[x + y*sl_params->proj_w + (newCount-1)*proj_nelems];
						else
							break;
						newCount--;
					}
					bins->data.fl[x + y*sl_params->proj_w + newCount*proj_nelems] = c+r*sl_params->cam_w;
					counts->data.fl[x + y*sl_params->proj_w] = counts->data.fl[x + y*sl_params->proj_w]+1;
				}
			}
		}
	}
	cvReleaseMat(&reproj_points);

	cvZero(proj_points);

  // copy orig_points into proj_points using bins and counts
  // orig_points: 3*cam_nelems
  // proj_points: 3*proj_nelems
	for(int r=0; r<sl_params->proj_h; r++)
	{
		for(int c=0; c<sl_params->proj_w; c++)
		{
			int theIndex = c+r*sl_params->proj_w;
			/*for (int count = 0; count < counts->data.fl[theIndex]; count++)
			{
				proj_points->data.fl[theIndex] += orig_points->data.fl[(int)bins->data.fl[theIndex + count*proj_nelems]];
				proj_points->data.fl[theIndex+proj_nelems] += orig_points->data.fl[(int)bins->data.fl[theIndex + count*proj_nelems]+cam_nelems];
				proj_points->data.fl[theIndex+2*proj_nelems] += orig_points->data.fl[(int)bins->data.fl[theIndex + count*proj_nelems]+cam_nelems*2];
			}*/
			
			if(counts->data.fl[theIndex])
			{
				int count = counts->data.fl[theIndex] / 2;
				proj_mask->data.fl[theIndex] = 1;
				
				proj_points->data.fl[theIndex] = orig_points->data.fl[(int)bins->data.fl[theIndex + count*proj_nelems]];
				proj_points->data.fl[theIndex+proj_nelems] = orig_points->data.fl[(int)bins->data.fl[theIndex + count*proj_nelems]+cam_nelems];
				proj_points->data.fl[theIndex+2*proj_nelems] = orig_points->data.fl[(int)bins->data.fl[theIndex + count*proj_nelems]+cam_nelems*2];
				
				if(count % 2 && count > 1)
				{
					proj_points->data.fl[theIndex] = (proj_points->data.fl[theIndex] + orig_points->data.fl[(int)bins->data.fl[theIndex + (count+1)*proj_nelems]])/ 2.0f;
					proj_points->data.fl[theIndex+proj_nelems] = (proj_points->data.fl[theIndex+proj_nelems] + orig_points->data.fl[(int)bins->data.fl[theIndex + (count+1)*proj_nelems]+cam_nelems])/2.0f;
					proj_points->data.fl[theIndex+2*proj_nelems] = (proj_points->data.fl[theIndex+2*proj_nelems] + orig_points->data.fl[(int)bins->data.fl[theIndex + (count+1)*proj_nelems]+cam_nelems*2])/2.0f;
				}
				
			//	proj_points->data.fl[theIndex] /= counts->data.fl[theIndex];
			//	proj_points->data.fl[theIndex+proj_nelems] /= counts->data.fl[theIndex];
			//	proj_points->data.fl[theIndex+2*proj_nelems] /= counts->data.fl[theIndex];
			}
			else
			{
				proj_mask->data.fl[theIndex] = 0;
			}
		}
	}
  
  // transpose proj_points from 3*proj_nelems -> proj_nelems*3 
  temp_transpose = cvCreateMat(proj_nelems, 3, CV_32FC1); 
  cvTranspose(proj_points, temp_transpose);
  cvReleaseMat(&proj_points);
  proj_points = temp_transpose;

	cvZero(proj_rotation);
	cvZero(proj_translation);
	reproj_points = cvCreateMat(proj_nelems, 2, CV_32FC1);

  // project proj_points into camera
  // proj_points: proj_nelems*3
  // reproj_points: proj_nelems*2
	cvProjectPoints2(proj_points, proj_rotation, proj_translation, sl_calib->cam_intrinsic, sl_calib->cam_distortion, reproj_points);

  // transpose reproj_points from proj_nelems*2 -> 2*proj_nelems
  temp_transpose = cvCreateMat(2, proj_nelems, CV_32FC1);
  cvTranspose(reproj_points, temp_transpose);
  cvReleaseMat(&reproj_points);
  reproj_points = temp_transpose;
	
	cvZero(mask);

  // copy reproj_points into resample_points
  // reproj_points: 2*proj_nelems
  // resample_points: 3*cam_nelems
	for(int r=0; r<sl_params->proj_h; r++)
	{
		for(int c=0; c<sl_params->proj_w; c++)
		{
			if(proj_mask->data.fl[c+r*sl_params->proj_w] != 0)
			{
				x = cvRound(CV_MAT_ELEM(*reproj_points, float, 0, c+r*sl_params->proj_w));
				y = cvRound(CV_MAT_ELEM(*reproj_points, float, 1, c+r*sl_params->proj_w));
				
				if((x >= 0 && x < sl_params->cam_w) && (y >=0 && y < sl_params->cam_h))
				{
					resample_points->data.fl[x+y*sl_params->cam_w] = proj_points->data.fl[c+r*sl_params->proj_w];
					resample_points->data.fl[x+y*sl_params->cam_w+cam_nelems] = proj_points->data.fl[c+r*sl_params->proj_w+proj_nelems];
					resample_points->data.fl[x+y*sl_params->cam_w+2*cam_nelems] = proj_points->data.fl[c+r*sl_params->proj_w+2*proj_nelems];
					//if(depth_map)
					//	depth_map->data.fl[x+y*sl_params->cam_w] = proj_points->data.fl[c+r*sl_params->proj_w+2*proj_nelems];
					mask->data.fl[x+y*sl_params->cam_w] = 1;
				}
				else // out of camera bounds, but camera mask already zeroed
				{
					// mask->data.fl[x+y*sl_params->cam_w] = 0;
				}
			}
		}
	}
	
	cvReleaseMat(&proj_rotation);
	cvReleaseMat(&proj_translation);
	cvReleaseMat(&reproj_points);
	cvReleaseMat(&proj_points);
	cvReleaseMat(&proj_mask);
	cvReleaseMat(&bins);
	cvReleaseMat(&counts);
}
