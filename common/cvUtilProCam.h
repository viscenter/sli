// cvUtilProCam.h: Header file defining auxiliary functions for structured lighting.
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

#ifndef PROCAM_UTIL
#define PROCAM_UTIL

// Calculate the base 2 logarithm.
double log2(double x);

// Fit a hyperplane to a set of ND points.
void cvFitPlane(const CvMat* points, float* plane);

// Find intersection between a 3D plane and a 3D line.
void intersectLineWithPlane3D(const float* q, const float* v, const float* w, float* p, float& depth);

// Find closest point to two 3D lines.
void intersectLineWithLine3D(const float* q1, const float* v1, const float* q2, const float* v2, float* p);

// Shade a grayscale image using the "winter" colormap (similar to Matlab's).  
void colorizeWinter(IplImage* src, IplImage*& dst, IplImage* mask);

// Show an image, resampled to desired size.
void cvShowImageResampled(const char* name, IplImage* image, int width, int height);

// Save a PLY-formatted point cloud.
int savePointsPLY(char* filename, CvMat* points, CvMat* normals, CvMat* colors, CvMat* mask, int n_cols, int n_rows);

// Save XML-formatted configuration file.
void writeConfiguration(const char* filename, struct slParams* sl_params);

// Read XML-formatted configuration file.
void readConfiguration(const char* filename, struct slParams* sl_params);

#endif
