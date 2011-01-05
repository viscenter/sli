// cvCalibrateProCam.h: Header file for projector-camera calibration.
//
// Overview:
//   This file defines the functions for projector-camera calibration. Camera calibration is
//   implemented using basic OpenCV functions for chessboard detection and homography-based
//   calibration, following the method of Zhang [ICCV 2009]. Projector calibration is performed
//   using a two-image sequence. First, a physical (i.e., printed) chessboard pattern is
//   automatically detected. Afterwards, a virtual chessboard is projected. The first image is
//   subtracted from the second. Once again, the built-in OpenCV functions for chessboard 
//   detection are used to find the virtual chessboard corners. A prior camera calibration
//   allows 3D coordinates of each virtual chessboard corner to be reconstructed. Finally, the
//   complete set of 2D-to-3D correspondences, mapping 2D projector pixels to 3D points on the
//   calibration plane, is used to evaluate the intrinsic projector calibration. A single 
//   image pair, comprising a static printed chessboard with a projected virtual chessboard
//   visible on its surface, is used to estimate the projector-camera extrinsic calibration.
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
#ifndef CALIBRATE_PROCAM
#define CALIBRATE_PROCAM

// Generate a chessboard pattern for projector calibration.
int generateChessboard(struct slParams* sl_params, IplImage*& board, int& border_cols, int& border_rows);

// Detect chessboard corners (with subpixel refinement).
// Note: Returns 1 if chessboard is found, 0 otherwise.
int detectChessboard(IplImage* frame, CvSize board_size, CvPoint2D32f* corners, int* corner_count CV_DEFAULT(NULL));

// Evaluate geometry of projector-camera optical rays and planes.
int evaluateProCamGeometry(struct slParams* sl_params, struct slCalib* sl_calib);


void recalibrateExtrinsics(struct slParams* sl_params, struct slCalib* sl_calib, int n_boards, CvMat* cam_image_points, CvMat* cam_object_points, CvMat* cam_point_counts,
						   CvMat* proj_image_points, CvMat* proj_object_points, CvMat* proj_point_counts, CvMat* proj_rotation_vectors, CvMat* proj_translation_vectors);
#endif