// cvUtilProCam.h: Implementations of auxiliary functions for structured lighting.
//
// Overview:
//   This file defines auxiliary functions for implementing structured lighting. Functions
//   include basic operations, such as the base-2 logarithm, as well as geometric algorithms, 
//   including fitting planes to 3D points and intersecting lines with other lines and planes.
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
#include "cvUtilProCam.h"

// Calculate the base 2 logarithm.
double log2(double x)
{
    return log(x)/log(2.0);
}

// Fit a hyperplane to a set of ND points.
// Note: Input points must be in the form of an NxM matrix, where M is the dimensionality.
//       This function finds the best-fit plane P, in the least-squares
//       sense, between the points (X,Y,Z). The resulting plane P is described
//       by the coefficient vector W, where W(1)*X + W(2)*Y +W(3)*Z = W(3), for
//       (X,Y,Z) on the plane P.
void cvFitPlane(const CvMat* points, float* plane){

	// Estimate geometric centroid.
	int nrows = points->rows;
	int ncols = points->cols;
	int type  = points->type;
	CvMat* centroid = cvCreateMat(1, ncols, type);
	cvSet(centroid, cvScalar(0));
	for(int c=0; c<ncols; c++){
		for(int r=0; r<nrows; r++)
			centroid->data.fl[c] += points->data.fl[ncols*r+c];
		centroid->data.fl[c] /= nrows;
	}
	
	// Subtract geometric centroid from each point.
	CvMat* points2 = cvCreateMat(nrows, ncols, type);
	for(int r=0; r<nrows; r++)
		for(int c=0; c<ncols; c++)
			points2->data.fl[ncols*r+c] = points->data.fl[ncols*r+c] - centroid->data.fl[c];
	
	// Evaluate SVD of covariance matrix.
	CvMat* A = cvCreateMat(ncols, ncols, type);
	CvMat* W = cvCreateMat(ncols, ncols, type);
	CvMat* V = cvCreateMat(ncols, ncols, type);
	cvGEMM(points2, points, 1, NULL, 0, A, CV_GEMM_A_T); 
	cvSVD(A, W, NULL, V, CV_SVD_V_T);

	// Assign plane coefficients by singular vector corresponding to smallest singular value.
	plane[ncols] = 0;
	for(int c=0; c<ncols; c++){
		plane[c] = V->data.fl[ncols*(ncols-1)+c];
		plane[ncols] += plane[c]*centroid->data.fl[c];
	}

	// Release allocated resources.
	cvReleaseMat(&centroid);
	cvReleaseMat(&points2);
	cvReleaseMat(&A);
	cvReleaseMat(&W);
	cvReleaseMat(&V);
}

// Find intersection between a 3D plane and a 3D line.
// Note: Finds the point of intersection of a line in parametric form 
//       (i.e., containing a point Q and spanned by the vector V, with 
//       a plane W defined in implicit form. Note, this function does 
//       not handle certain "degenerate" cases, since they do not occur
//       in practice with the structured lighting configuration.
void intersectLineWithPlane3D(const float* q, 
							  const float* v, 
							  const float* w,
							  float* p, 
							  float& depth){

	// Evaluate inner products.
	float n_dot_q = 0, n_dot_v = 0;
	for(int i=0; i<3; i++){
		n_dot_q += w[i]*q[i];
		n_dot_v += w[i]*v[i];
	}

	// Evaluate point of intersection P.
	depth = (w[3]-n_dot_q)/n_dot_v;
	for(int i=0; i<3; i++)
		p[i] = q[i] + depth*v[i];
}

// Find closest point to two 3D lines.
// Note: Finds the closest 3D point between two 3D lines defined in parametric
///      form (i.e., containing a point Q and spanned by the vector V). Note, 
//       this function does not handle certain "degenerate" cases, since they 
//       do not occur in practice with the structured lighting configuration.
void intersectLineWithLine3D(const float* q1, 
							 const float* v1, 
							 const float* q2,
							 const float* v2,
							 float* p){

	// Define intermediate quantities.
	float q12[3], v1_dot_v1 = 0, v2_dot_v2 = 0, v1_dot_v2 = 0, q12_dot_v1 = 0, q12_dot_v2 = 0;
	for(int i=0; i<3; i++){
		q12[i]      =  q1[i]-q2[i];
		v1_dot_v1  +=  v1[i]*v1[i];
		v2_dot_v2  +=  v2[i]*v2[i];
		v1_dot_v2  +=  v1[i]*v2[i];
		q12_dot_v1 += q12[i]*v1[i];
		q12_dot_v2 += q12[i]*v2[i];
	}

	// Calculate scale factors.
	float s, t, denom;
	denom = v1_dot_v1*v2_dot_v2 - v1_dot_v2*v1_dot_v2;
	s =  (v1_dot_v2/denom)*q12_dot_v2 - (v2_dot_v2/denom)*q12_dot_v1;
	t = -(v1_dot_v2/denom)*q12_dot_v1 + (v1_dot_v1/denom)*q12_dot_v2;

	// Evaluate closest point.
	for(int i=0; i<3; i++)
		p[i] = ( (q1[i]+s*v1[i]) + (q2[i]+t*v2[i]) )/2;
}

// Shade a grayscale image using the "winter" colormap (similar to Matlab's). 
void colorizeWinter(IplImage* src, IplImage*& dst, IplImage* mask){

	// Create an increasing linear-ramp in the green channel.
	cvSetZero(dst);
	cvMerge(NULL, src, NULL, NULL, dst);

	// Create a decreasing linear-ramp in the blue channel.
	IplImage* blue = cvCloneImage(src);
	cvSubRS(src, cvScalar(255.0), blue, mask);
	cvMerge(blue, NULL, NULL, NULL, dst);
	
	// Release allocated resources.
	cvReleaseImage(&blue);
}

// Show an image, resampled to desired size.
void cvShowImageResampled(const char* name, 
						  IplImage* image, 
						  int width, 
						  int height){

	// Allocate resampled image.
	IplImage* resampled_image = 
		cvCreateImage(cvSize(width, height), image->depth, image->nChannels);

	// Resize image.
	cvResize(image, resampled_image, CV_INTER_LINEAR);

	// Display resampled image.
	cvShowImage(name, resampled_image);

	// Release allocated resources.
	cvReleaseImage(&resampled_image);
}

// Save a PLY-formatted point cloud.
int savePointsPLY(char* filename, 
				   CvMat* points,
				   CvMat* normals,
				   CvMat* colors,
				   CvMat* mask,
				   int n_cols,
				   int n_rows){

	// Open output file and create header.
	FILE* pFile = fopen(filename, "w");

	//char filename2[1024];
	//strcpy(filename2, filename);
	//strcat(filename2, ".texture");
	//FILE* pFile2 = fopen(filename2, "w");

	int cam_nelems = n_cols*n_rows;
		
	int numPoints = 0;
	for(int c=0; c<points->cols; c++)
			if(mask == NULL || mask->data.fl[c] != 0)
				numPoints++;

	fprintf(pFile, "ply\nformat ascii 1.0\n");
	fprintf(pFile, "element vertex %d\nproperty float x\nproperty float y\nproperty float z\nproperty float u\nproperty float v\n", numPoints);
	//fprintf(pFile, "property float nx\n property float ny\n property float nz\n");
	fprintf(pFile, "end_header\n");

	if(points != NULL){
		for(int row=0; row<n_rows; row++)
		{
			for(int col=0; col<n_cols; col++)
			{
				if(mask == NULL || mask->data.fl[col+row*n_cols] != 0){
					for(int r=0; r<points->rows; r++)
						fprintf(pFile, "%f ", points->data.fl[col+row*n_cols + r*cam_nelems]);
					fprintf(pFile, "%f %f\n", (float)col/(float)n_cols, (float)row/(float)n_rows);
					//for(int r=0; r<normals->rows; r++)
						//fprintf(pFile, "%f ", -normals->data.fl[c + normals->cols*r]);
					//fprintf(pFile, "\n");
					//fprintf(pFile2, "%f %f\n", (float)col/(float)n_cols, (float)row/(float)n_rows);
				}
			}
		}
	 }

	if(fclose(pFile) != 0){
		printf("ERROR: Cannot close PLY file!\n");
		return -1;
	}

	// Return without errors.
	return 0;
}

// Save XML-formatted configuration file.
void writeConfiguration(const char* filename, struct slParams* sl_params){

	// Create file storage for XML-formatted configuration file.
	CvFileStorage* fs = cvOpenFileStorage(filename, 0, CV_STORAGE_WRITE);
	
	// Write output directory and object (or sequence) name.
	cvStartWriteStruct(fs, "output", CV_NODE_MAP);
	cvWriteString(fs, "output_directory",          sl_params->outdir, 1);
	cvWriteInt(fs,    "save_intermediate_results", sl_params->save);
	cvEndWriteStruct(fs);
	
	// Write camera parameters.
	cvStartWriteStruct(fs, "camera", CV_NODE_MAP);
	cvWriteInt(fs, "width",                           sl_params->cam_w);
	cvWriteInt(fs, "height",                          sl_params->cam_h);
	cvEndWriteStruct(fs);

	// Write projector parameters.
	cvStartWriteStruct(fs, "projector", CV_NODE_MAP);
	cvWriteInt(fs, "width",            sl_params->proj_w);
	cvWriteInt(fs, "height",           sl_params->proj_h);
	cvWriteInt(fs, "invert_projector", sl_params->proj_invert);
	cvEndWriteStruct(fs);

	// Write camera and projector gain parameters.
	cvStartWriteStruct(fs, "gain", CV_NODE_MAP);
	cvWriteInt(fs, "camera_gain",    sl_params->cam_gain);
	cvWriteInt(fs, "projector_gain", sl_params->proj_gain);
	cvEndWriteStruct(fs);

	// Write distortion model parameters.
	cvStartWriteStruct(fs, "distortion_model", CV_NODE_MAP);
	cvWriteInt(fs, "enable_tangential_camera",          sl_params->cam_dist_model[0]);
	cvWriteInt(fs, "enable_6th_order_radial_camera",    sl_params->cam_dist_model[1]);
	cvWriteInt(fs, "enable_tangential_projector",       sl_params->proj_dist_model[0]);
	cvWriteInt(fs, "enable_6th_order_radial_projector", sl_params->proj_dist_model[1]);
	cvEndWriteStruct(fs);

	// Write camera calibration chessboard parameters.
	cvStartWriteStruct(fs, "camera_chessboard", CV_NODE_MAP);
	cvWriteInt(fs,  "interior_horizontal_corners",  sl_params->cam_board_w);
	cvWriteInt(fs,  "interior_vertical_corners",    sl_params->cam_board_h);
	cvWriteReal(fs, "square_width_mm",              sl_params->cam_board_w_mm);
	cvWriteReal(fs, "square_height_mm",             sl_params->cam_board_h_mm);
	cvEndWriteStruct(fs);

	// Write projector calibration chessboard parameters.
	cvStartWriteStruct(fs, "projector_chessboard", CV_NODE_MAP);
	cvWriteInt(fs, "interior_horizontal_corners",  sl_params->proj_board_w);
	cvWriteInt(fs, "interior_vertical_corners",    sl_params->proj_board_h);
	cvWriteInt(fs, "square_width_pixels",          sl_params->proj_board_w_pixels);
	cvWriteInt(fs, "square_height_pixels",         sl_params->proj_board_h_pixels);
	cvEndWriteStruct(fs);

	// Write scanning and reconstruction parameters.
	cvStartWriteStruct(fs, "scanning_and_reconstruction", CV_NODE_MAP);
	cvWriteInt(fs,  "mode",                           sl_params->mode);
	cvWriteInt(fs,  "reconstruct_columns",            sl_params->scan_cols);
	cvWriteInt(fs,  "reconstruct_rows",               sl_params->scan_rows);
	cvWriteInt(fs,  "frame_delay_ms",                 sl_params->delay);
	cvWriteInt(fs,  "minimum_contrast_threshold",     sl_params->thresh);
	cvWriteReal(fs, "minimum_distance_mm",            sl_params->dist_range[0]);
	cvWriteReal(fs, "maximum_distance_mm",            sl_params->dist_range[1]);
	cvWriteReal(fs, "maximum_distance_variation_mm",  sl_params->dist_reject);
	cvWriteReal(fs, "minimum_background_distance_mm", sl_params->background_depth_thresh);
	cvEndWriteStruct(fs);

	// Write visualization options.
	cvStartWriteStruct(fs, "visualization", CV_NODE_MAP);
	cvWriteInt(fs, "display_intermediate_results", sl_params->display);
	cvWriteInt(fs, "display_window_width_pixels",  sl_params->window_w);
	cvEndWriteStruct(fs);

	// Close file storage for XML-formatted configuration file.
	cvReleaseFileStorage(&fs);
}

// Read XML-formatted configuration file.
void readConfiguration(const char* filename, struct slParams* sl_params){

	// Open file storage for XML-formatted configuration file.
	CvFileStorage* fs = cvOpenFileStorage(filename, 0, CV_STORAGE_READ);

	// Read output directory and object (or sequence) name.
	CvFileNode* m = cvGetFileNodeByName(fs, 0, "output");
	strcpy(sl_params->outdir, cvReadStringByName(fs, m, "output_directory", "./output"));
	sl_params->save = (cvReadIntByName(fs, m, "save_intermediate_results", 0) != 0);

	// Read camera parameters.
	m = cvGetFileNodeByName(fs, 0, "camera");
	sl_params->cam_w         =  cvReadIntByName(fs, m, "width",                          1804);
	sl_params->cam_h         =  cvReadIntByName(fs, m, "height",                         1353);

	// Read projector parameters.
	m = cvGetFileNodeByName(fs, 0, "projector");
	sl_params->proj_w      =  cvReadIntByName(fs, m, "width",            800);
	sl_params->proj_h      =  cvReadIntByName(fs, m, "height",            600);
	sl_params->proj_invert = (cvReadIntByName(fs, m, "invert_projector",    0) != 0);

	// Read camera and projector gain parameters.
	m = cvGetFileNodeByName(fs, 0, "gain");
	sl_params->cam_gain  = cvReadIntByName(fs, m, "camera_gain",     50);
	sl_params->proj_gain = cvReadIntByName(fs, m, "projector_gain",  50);
	
	// Read distortion model parameters.
	m = cvGetFileNodeByName(fs, 0, "distortion_model");
	sl_params->cam_dist_model[0]  = (cvReadIntByName(fs, m, "enable_tangential_camera",          0) != 0);
	sl_params->cam_dist_model[1]  = (cvReadIntByName(fs, m, "enable_6th_order_radial_camera",    0) != 0);
	sl_params->proj_dist_model[0] = (cvReadIntByName(fs, m, "enable_tangential_projector",       0) != 0);
	sl_params->proj_dist_model[1] = (cvReadIntByName(fs, m, "enable_6th_order_radial_projector", 0) != 0);

	// Read camera calibration chessboard parameters.
	m = cvGetFileNodeByName(fs, 0, "camera_chessboard");
	sl_params->cam_board_w    =        cvReadIntByName(fs,  m, "interior_horizontal_corners",    8);
	sl_params->cam_board_h    =        cvReadIntByName(fs,  m, "interior_vertical_corners",      6);
	sl_params->cam_board_w_mm = (float)cvReadRealByName(fs, m, "square_width_mm",             30.0);
	sl_params->cam_board_h_mm = (float)cvReadRealByName(fs, m, "square_height_mm",            30.0);

	// Read projector calibration chessboard parameters.
	m = cvGetFileNodeByName(fs, 0, "projector_chessboard");
	sl_params->proj_board_w        = cvReadIntByName(fs,  m, "interior_horizontal_corners",  8);
	sl_params->proj_board_h        = cvReadIntByName(fs,  m, "interior_vertical_corners",    6);
	sl_params->proj_board_w_pixels = cvReadIntByName(fs, m, "square_width_pixels",          75);
	sl_params->proj_board_h_pixels = cvReadIntByName(fs, m, "square_height_pixels",         75);
	
	// Read scanning and reconstruction parameters.
	m = cvGetFileNodeByName(fs, 0, "scanning_and_reconstruction");
	sl_params->mode                    =         cvReadIntByName(fs,  m, "mode",                               2);
	sl_params->scan_cols               =        (cvReadIntByName(fs,  m, "reconstruct_columns",                1) != 0);
	sl_params->scan_rows               =        (cvReadIntByName(fs,  m, "reconstruct_rows",                   1) != 0);
	sl_params->delay                   =         cvReadIntByName(fs,  m, "frame_delay_ms",                   200);
	sl_params->thresh                  =         cvReadIntByName(fs,  m, "minimum_contrast_threshold",        32);
	sl_params->dist_range[0]           = (float) cvReadRealByName(fs, m, "minimum_distance_mm",              0.0);
	sl_params->dist_range[1]           = (float) cvReadRealByName(fs, m, "maximum_distance_mm",            1.0e4);
	sl_params->dist_reject             = (float) cvReadRealByName(fs, m, "maximum_distance_variation_mm",   10.0);
	sl_params->background_depth_thresh = (float) cvReadRealByName(fs, m, "minimum_background_distance_mm",  20.0);

	// Read visualization options.
	m = cvGetFileNodeByName(fs, 0, "visualization");
	sl_params->display  = (cvReadIntByName(fs, m, "display_intermediate_results",   1) != 0);
	sl_params->window_w =  cvReadIntByName(fs, m, "display_window_width_pixels",  640);

	// Enable both row and column scanning, if "ray-ray" reconstruction mode is enabled.
	if(sl_params->mode == 2){
		sl_params->scan_cols = true;
		sl_params->scan_rows = true;
	}

	// Set camera visualization window dimensions.
	sl_params->window_h = (int)ceil((float)sl_params->window_w*((float)sl_params->cam_h/(float)sl_params->cam_w));

	// Close file storage for XML-formatted configuration file.
	cvReleaseFileStorage(&fs);
}
