// cvCalibrateProCam.cpp: Functions for projector-camera calibration.
//
// Overview:
//   This file implements the functions for projector-camera calibration. Camera calibration is
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

#include "stdafx.h"
#include "globals.h"
#include "cvCalibrateProCam.h"
#include "cvUtilProCam.h"

// Generate a chessboard pattern for projector calibration.
int generateChessboard(struct slParams* sl_params, IplImage*& board, int& border_cols, int& border_rows){

	// Calculate chessboard border.
	border_cols = (int)floor((board->width -(sl_params->proj_board_w+1)*sl_params->proj_board_w_pixels)/2.0);
	border_rows = (int)floor((board->height-(sl_params->proj_board_h+1)*sl_params->proj_board_h_pixels)/2.0);

	// Check for chessboard errors.
	if( (border_cols < 0) || (border_rows < 0) ){
		printf("ERROR: Cannot create chessboard with user-requested dimensions!\n");
		return -1;
	}

	// Initialize chessboard with white image.
	cvSet(board, cvScalar(255));

	// Create odd black squares.
	uchar* data = (uchar*)board->imageData;
	int step = board->widthStep/sizeof(uchar);
	for(int r=0; r<(sl_params->proj_board_h+1); r+=2)
		for(int c=0; c<(sl_params->proj_board_w+1); c+=2)
			for(int i=(r*sl_params->proj_board_h_pixels+border_rows); 
				i<((r+1)*sl_params->proj_board_h_pixels+border_rows); i++)
				for(int j=(c*sl_params->proj_board_w_pixels+border_cols); 
					j<((c+1)*sl_params->proj_board_w_pixels+border_cols); j++)
					data[i*step+j] = 0;

	// Create even black squares.
	for(int r=1; r<sl_params->proj_board_h; r+=2)
		for(int c=1; c<sl_params->proj_board_w; c+=2)
			for(int i=(r*sl_params->proj_board_h_pixels+border_rows); 
				i<((r+1)*sl_params->proj_board_h_pixels+border_rows); i++)
				for(int j=(c*sl_params->proj_board_w_pixels+border_cols); 
					j<((c+1)*sl_params->proj_board_w_pixels+border_cols); j++)
					data[i*step+j] = 0;

	// Return without errors.
	return 0;
}

// Detect chessboard corners (with subpixel refinement).
// Note: Returns 1 if chessboard is found, 0 otherwise.
int detectChessboard(IplImage* frame, CvSize board_size,
                     CvPoint2D32f* corners,
                     int* corner_count){

	// Find chessboard corners.
	int found = cvFindChessboardCorners(
		frame, board_size, corners, corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

	// Refine chessboard corners.
	IplImage* gray_frame = cvCreateImage(cvGetSize(frame), frame->depth, 1);
	if(frame->nChannels > 1)
		cvCvtColor(frame, gray_frame, CV_BGR2GRAY);
	else
		cvCopyImage(frame, gray_frame);
	cvFindCornerSubPix(gray_frame, corners, *corner_count, 
		cvSize(11,11), cvSize(-1,-1), 
		cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));

	// Release allocated resources.
	cvReleaseImage(&gray_frame);

	// Return without errors (i.e., chessboard was found).						 
	return found;
}

// Evaluate geometry of projector-camera optical rays and planes.
int evaluateProCamGeometry(struct slParams* sl_params, struct slCalib* sl_calib){

	// Check for input errors (no intrinsic or extrinsic calibration, etc.).
	if(!sl_calib->cam_intrinsic_calib || !sl_calib->proj_intrinsic_calib || !sl_calib->procam_extrinsic_calib){
		printf("ERROR: Intrinsic and extrinsic projector-camera calibration must be completed first!\n");
		printf("Projector-camera geometry calculations were not successful and must be repeated.\n");
		return -1;
	}

	// Extract extrinsic calibration parameters.
	CvMat* r                = cvCreateMat(1, 3, CV_32FC1);
	CvMat* cam_rotation     = cvCreateMat(3, 3, CV_32FC1);
	CvMat* cam_translation  = cvCreateMat(3, 1, CV_32FC1);
	CvMat* proj_rotation    = cvCreateMat(3, 3, CV_32FC1);
	CvMat* proj_translation = cvCreateMat(3, 1, CV_32FC1);
	cvGetRow(sl_calib->cam_extrinsic, r, 0);
	cvRodrigues2(r, cam_rotation, NULL);
	for(int i=0; i<3; i++)
		cvmSet(cam_translation, i, 0, cvmGet(sl_calib->cam_extrinsic, 1, i));
	cvGetRow(sl_calib->proj_extrinsic, r, 0);
	cvRodrigues2(r, proj_rotation, NULL);
	for(int i=0; i<3; i++)
		cvmSet(proj_translation, i, 0, cvmGet(sl_calib->proj_extrinsic, 1, i));
	cvReleaseMat(&r);

	// Determine centers of projection.
	// Note: All positions are in coordinate system of the first camera.
	cvSet(sl_calib->cam_center, cvScalar(0));
	cvGEMM(proj_rotation, proj_translation, -1, NULL, 0, sl_calib->proj_center, CV_GEMM_A_T);
	//cvGEMM(cam_rotation, sl_calib->proj_center, 1, cam_translation, 1, sl_calib->proj_center, 0);

	// Pre-compute optical rays for each camera pixel.
	int    cam_nelems        = sl_params->cam_w*sl_params->cam_h;
	CvMat* cam_dist_points   = cvCreateMat(cam_nelems, 1, CV_32FC2);
	CvMat* cam_undist_points = cvCreateMat(cam_nelems, 1, CV_32FC2);
	for(int r=0; r<sl_params->cam_h; r++)
		for(int c=0; c<sl_params->cam_w; c++)
			cvSet1D(cam_dist_points, (sl_params->cam_w)*r+c, cvScalar(float(c), float(r)));
	cvUndistortPoints(cam_dist_points, cam_undist_points, sl_calib->cam_intrinsic, sl_calib->cam_distortion, NULL, NULL);
	for(int i=0; i<cam_nelems; i++){
		CvScalar pd = cvGet1D(cam_undist_points, i);
		float norm = (float)sqrt(pow(pd.val[0],2)+pow(pd.val[1],2)+1.0);
		sl_calib->cam_rays->data.fl[i]              = (float)pd.val[0]/norm;
		sl_calib->cam_rays->data.fl[i+  cam_nelems] = (float)pd.val[1]/norm;
		sl_calib->cam_rays->data.fl[i+2*cam_nelems] = (float)1.0/norm;
	}
	cvReleaseMat(&cam_dist_points);
	cvReleaseMat(&cam_undist_points);

	// Pre-compute optical rays for each projector pixel.
	int    proj_nelems        = sl_params->proj_w*sl_params->proj_h;
	CvMat* proj_dist_points   = cvCreateMat(proj_nelems, 1, CV_32FC2);
	CvMat* proj_undist_points = cvCreateMat(proj_nelems, 1, CV_32FC2);
	for(int r=0; r<sl_params->proj_h; r++)
		for(int c=0; c<sl_params->proj_w; c++)
			cvSet1D(proj_dist_points, (sl_params->proj_w)*r+c, cvScalar(float(c), float(r)));
	cvUndistortPoints(proj_dist_points, proj_undist_points, sl_calib->proj_intrinsic, sl_calib->proj_distortion, NULL, NULL);
	for(int i=0; i<proj_nelems; i++){
		CvScalar pd = cvGet1D(proj_undist_points, i);
		float norm = (float)sqrt(pow(pd.val[0],2)+pow(pd.val[1],2)+1.0);
		sl_calib->proj_rays->data.fl[i]               = (float)pd.val[0]/norm;
		sl_calib->proj_rays->data.fl[i+  proj_nelems] = (float)pd.val[1]/norm;
		sl_calib->proj_rays->data.fl[i+2*proj_nelems] = (float)1.0/norm;
	}
	cvReleaseMat(&proj_dist_points);
	cvReleaseMat(&proj_undist_points);

	// Rotate projector optical rays into the camera coordinate system.
	CvMat* R = cvCreateMat(3, 3, CV_32FC1);
	cvTranspose(proj_rotation, R);
	//cvGEMM(cam_rotation, proj_rotation, 1, NULL, 0, R, CV_GEMM_B_T);
	cvGEMM(R, sl_calib->proj_rays, 1, NULL, 0, sl_calib->proj_rays, 0);
	cvReleaseMat(&R);

	// Evaluate scale factor (to assist in plane-fitting).
	float scale = 0;
	for(int i=0; i<3; i++)
		scale += pow((float)sl_calib->proj_center->data.fl[i],(float)2.0);
	scale = sqrt(scale);

	// Estimate plane equations describing every projector column.
    // Note: Resulting coefficient vector is in camera coordinate system.
	for(int c=0; c<sl_params->proj_w; c++){
		CvMat* points = cvCreateMat(sl_params->proj_h+1, 3, CV_32FC1);
		for(int ro=0; ro<sl_params->proj_h; ro++){
			int ri = (sl_params->proj_w)*ro+c;
			for(int i=0; i<3; i++)
				points->data.fl[3*ro+i] = 
					sl_calib->proj_center->data.fl[i] + scale*sl_calib->proj_rays->data.fl[ri+proj_nelems*i];
		}
		for(int i=0; i<3; i++)
			points->data.fl[3*sl_params->proj_h+i] = sl_calib->proj_center->data.fl[i];
		float plane[4];
		cvFitPlane(points, plane);
		for(int i=0; i<4; i++)
			sl_calib->proj_column_planes->data.fl[4*c+i] = plane[i];
		cvReleaseMat(&points);
	}

	// Estimate plane equations describing every projector row.
    // Note: Resulting coefficient vector is in camera coordinate system.
	for(int r=0; r<sl_params->proj_h; r++){
		CvMat* points = cvCreateMat(sl_params->proj_w+1, 3, CV_32FC1);
		for(int co=0; co<sl_params->proj_w; co++){
			int ri = (sl_params->proj_w)*r+co;
			for(int i=0; i<3; i++)
				points->data.fl[3*co+i] = 
					sl_calib->proj_center->data.fl[i] + scale*sl_calib->proj_rays->data.fl[ri+proj_nelems*i];
		}
		for(int i=0; i<3; i++)
			points->data.fl[3*sl_params->proj_w+i] = sl_calib->proj_center->data.fl[i];
		float plane[4];
		cvFitPlane(points, plane);
		for(int i=0; i<4; i++)
			sl_calib->proj_row_planes->data.fl[4*r+i] = plane[i];
		cvReleaseMat(&points);
	}
	
	// Release allocated resources.
	cvReleaseMat(&cam_rotation);
	cvReleaseMat(&cam_translation);
	cvReleaseMat(&proj_rotation);
	cvReleaseMat(&proj_translation);

	// Return without errors.
	return 0;
}

static int dbCmp( const void* _a, const void* _b )
{
    double a = *(const double*)_a;
    double b = *(const double*)_b;

    return (a > b) - (a < b);
}

void recalibrateExtrinsics(struct slParams* sl_params, struct slCalib* sl_calib, int n_boards, CvMat* cam_image_points, CvMat* cam_object_points, CvMat* cam_point_counts,
						   CvMat* proj_image_points, CvMat* proj_object_points, CvMat* proj_point_counts, CvMat* proj_rotation_vectors, CvMat* proj_translation_vectors)
{
	
	int cam_board_n = sl_params->cam_board_w*sl_params->cam_board_h;
	int proj_board_n = sl_params->proj_board_w*sl_params->proj_board_h;

	const int NINTRINSIC = 9;
	CvMat* npoints = 0;
	CvMat* err = 0;
	CvMat* J_LR = 0;
	CvMat* Je = 0;
	CvMat* Ji = 0;
	CvMat* imagePoints[2] = {0,0};
	CvMat* cam_objectPoints = 0;
	CvMat* proj_objectPoints = 0;
	CvMat* RT0 = 0;
	CvLevMarq solver;
	int nparams = 6*(n_boards+1);
	double A[2][9], dk[2][5]={{0,0,0,0,0},{0,0,0,0,0}}, rlr[9];
	CvMat K[2], Dist[2], om_LR, T_LR;
	CvMat R_LR = cvMat(3, 3, CV_64F, rlr);
	int i, p, ni = 0, ofs = 0;
	
	err = cvCreateMat( cam_board_n*2, 1, CV_64F );
	Je = cvCreateMat( cam_board_n*2, 6, CV_64F );
	J_LR = cvCreateMat( cam_board_n*2, 6, CV_64F );
	Ji = cvCreateMat( cam_board_n*2, NINTRINSIC, CV_64F );
	cvZero( Ji );


	cam_objectPoints = cvCreateMat( cam_object_points->rows, cam_object_points->cols, CV_64FC(CV_MAT_CN(cam_object_points->type)));
    cvConvert( cam_object_points, cam_objectPoints );
    cvReshape( cam_objectPoints, cam_objectPoints, 3, 1 );

	proj_objectPoints = cvCreateMat( proj_object_points->rows, proj_object_points->cols, CV_64FC(CV_MAT_CN(proj_object_points->type)));
    cvConvert( proj_object_points, proj_objectPoints );
    cvReshape( proj_objectPoints, proj_objectPoints, 3, 1 );

    for(int k = 0; k < 2; k++ )
    {
        const CvMat* points = k == 0 ? cam_image_points : proj_image_points;
        const CvMat* cameraMatrix = k == 0 ? sl_calib->cam_intrinsic : sl_calib->proj_intrinsic;
        const CvMat* distCoeffs = k == 0 ? sl_calib->cam_distortion : sl_calib->proj_distortion;

        int cn = CV_MAT_CN(cam_image_points->type);
        /*CV_ASSERT( (CV_MAT_DEPTH(cam_image_points->type) == CV_32F ||
                CV_MAT_DEPTH(cam_image_points->type) == CV_64F) &&
               ((cam_image_points->rows == pointsTotal && cam_image_points->cols*cn == 2) ||
                (cam_image_points->rows == 1 && cam_image_points->cols == pointsTotal && cn == 2)) );*/

        K[k] = cvMat(3,3,CV_64F,A[k]);
        Dist[k] = cvMat(1,5,CV_64F,dk[k]);

        imagePoints[k] = cvCreateMat( points->rows, points->cols, CV_64FC(CV_MAT_CN(points->type)));
        cvConvert( points, imagePoints[k] );
        cvReshape( imagePoints[k], imagePoints[k], 2, 1 );

        cvConvert( cameraMatrix, &K[k] );

        CvMat tdist = cvMat( distCoeffs->rows, distCoeffs->cols,
        CV_MAKETYPE(CV_64F,CV_MAT_CN(distCoeffs->type)), Dist[k].data.db );
        cvConvert( distCoeffs, &tdist );
    }


	RT0 = cvCreateMat( 6, n_boards, CV_64F );
	solver.init( nparams, 0, cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5));

	for( i = ofs = 0; i < n_boards; ofs += ni, i++ )
    {
       
        CvMat objpt_i, imgpt_i;
        double _om[2][3], r[2][9], t[2][3];
        CvMat om[2], R[2], T[2];

		ni = cam_board_n;
		objpt_i = cvMat(1, ni, CV_64FC3, cam_objectPoints->data.db + ofs*3);
        imgpt_i = cvMat(1, ni, CV_64FC2, imagePoints[0]->data.db + ofs*2);
        om[0] = cvMat(3, 1, CV_64F, _om[0]);
        R[0] = cvMat(3, 3, CV_64F, r[0]);
        T[0] = cvMat(3, 1, CV_64F, t[0]);

        cvFindExtrinsicCameraParams2( &objpt_i, &imgpt_i, &K[0], &Dist[0], &om[0], &T[0] );
        cvRodrigues2( &om[0], &R[0] );
        // save initial om_left and T_left
        solver.param->data.db[(i+1)*6] = _om[0][0];
        solver.param->data.db[(i+1)*6 + 1] = _om[0][1];
        solver.param->data.db[(i+1)*6 + 2] = _om[0][2];
        solver.param->data.db[(i+1)*6 + 3] = t[0][0];
        solver.param->data.db[(i+1)*6 + 4] = t[0][1];
        solver.param->data.db[(i+1)*6 + 5] = t[0][2];

		ni = proj_board_n;
		objpt_i = cvMat(1, ni, CV_64FC3, proj_objectPoints->data.db + ofs*3);
        imgpt_i = cvMat(1, ni, CV_64FC2, imagePoints[1]->data.db + ofs*2);
        om[1] = cvMat(3, 1, CV_64F, _om[1]);
        R[1] = cvMat(3, 3, CV_64F, r[1]);
        T[1] = cvMat(3, 1, CV_64F, t[1]);

        cvFindExtrinsicCameraParams2( &objpt_i, &imgpt_i, &K[1], &Dist[1], &om[1], &T[1] );
        cvRodrigues2( &om[1], &R[1] );

        cvGEMM( &R[1], &R[0], 1, 0, 0, &R[0], CV_GEMM_B_T );
        cvGEMM( &R[0], &T[0], -1, &T[1], 1, &T[1] );
        cvRodrigues2( &R[0], &T[0] );
        RT0->data.db[i] = t[0][0];
        RT0->data.db[i + n_boards] = t[0][1];
        RT0->data.db[i + n_boards*2] = t[0][2];
        RT0->data.db[i + n_boards*3] = t[1][0];
        RT0->data.db[i + n_boards*4] = t[1][1];
        RT0->data.db[i + n_boards*5] = t[1][2];
    }

    // find the medians and save the first 6 parameters
    for( i = 0; i < 6; i++ )
    {
        qsort( RT0->data.db + i*n_boards, n_boards, CV_ELEM_SIZE(RT0->type), dbCmp );
        solver.param->data.db[i] = n_boards % 2 != 0 ? RT0->data.db[i*n_boards + n_boards/2] :
            (RT0->data.db[i*n_boards + n_boards/2 - 1] + RT0->data.db[i*n_boards + n_boards/2])*0.5;
    }

    om_LR = cvMat(3, 1, CV_64F, solver.param->data.db);
    T_LR = cvMat(3, 1, CV_64F, solver.param->data.db + 3);

    while(true)
    {
        const CvMat* param = 0;
        CvMat tmpimagePoints;
        CvMat *JtJ = 0, *JtErr = 0;
        double* errNorm = 0;
        double _omR[3], _tR[3];
        double _dr3dr1[9], _dr3dr2[9], /*_dt3dr1[9],*/ _dt3dr2[9], _dt3dt1[9], _dt3dt2[9];
        CvMat dr3dr1 = cvMat(3, 3, CV_64F, _dr3dr1);
        CvMat dr3dr2 = cvMat(3, 3, CV_64F, _dr3dr2);
        //CvMat dt3dr1 = cvMat(3, 3, CV_64F, _dt3dr1);
        CvMat dt3dr2 = cvMat(3, 3, CV_64F, _dt3dr2);
        CvMat dt3dt1 = cvMat(3, 3, CV_64F, _dt3dt1);
        CvMat dt3dt2 = cvMat(3, 3, CV_64F, _dt3dt2);
        CvMat om[2], T[2];
        CvMat dpdrot_hdr, dpdt_hdr, dpdf_hdr, dpdc_hdr, dpdk_hdr;
        CvMat *dpdrot = &dpdrot_hdr, *dpdt = &dpdt_hdr, *dpdf = 0, *dpdc = 0, *dpdk = 0;
		CvMat objpt_i, imgpt_i;

        if( !solver.updateAlt( param, JtJ, JtErr, errNorm ))
            break;

        cvRodrigues2( &om_LR, &R_LR );
        om[1] = cvMat(3,1,CV_64F,_omR);
        T[1] = cvMat(3,1,CV_64F,_tR);

        for( i = ofs = 0; i < n_boards; ofs += ni, i++ )
        {
            ni = cam_board_n;
            CvMat _part;

            om[0] = cvMat(3,1,CV_64F,solver.param->data.db+(i+1)*6);
            T[0] = cvMat(3,1,CV_64F,solver.param->data.db+(i+1)*6+3);

            if( JtJ || JtErr )
                cvComposeRT( &om[0], &T[0], &om_LR, &T_LR, &om[1], &T[1], &dr3dr1, 0,
                             &dr3dr2, 0, 0, &dt3dt1, &dt3dr2, &dt3dt2 );
            else
                cvComposeRT( &om[0], &T[0], &om_LR, &T_LR, &om[1], &T[1] );

            
            err->rows = Je->rows = J_LR->rows = Ji->rows = ni*2;
            cvReshape( err, &tmpimagePoints, 2, 1 );

            cvGetCols( Ji, &dpdf_hdr, 0, 2 );
            cvGetCols( Ji, &dpdc_hdr, 2, 4 );
            cvGetCols( Ji, &dpdk_hdr, 4, NINTRINSIC );
            cvGetCols( Je, &dpdrot_hdr, 0, 3 );
            cvGetCols( Je, &dpdt_hdr, 3, 6 );

            double maxErr, l2err;
			objpt_i = cvMat(1, ni, CV_64FC3, cam_objectPoints->data.db + ofs*3);
            imgpt_i = cvMat(1, ni, CV_64FC2, imagePoints[0]->data.db + ofs*2);

            if( JtJ || JtErr )
                cvProjectPoints2( &objpt_i, &om[0], &T[0], &K[0], &Dist[0],
                        &tmpimagePoints, dpdrot, dpdt, dpdf, dpdc, dpdk, 0);
            else
                cvProjectPoints2( &objpt_i, &om[0], &T[0], &K[0], &Dist[0], &tmpimagePoints );
            cvSub( &tmpimagePoints, &imgpt_i, &tmpimagePoints );

            l2err = cvNorm( &tmpimagePoints, 0, CV_L2 );
            maxErr = cvNorm( &tmpimagePoints, 0, CV_C );

            if( JtJ || JtErr )
            {
                int iofs = (n_boards+1)*6, eofs = (i+1)*6;
                assert( JtJ && JtErr );

                cvGetSubRect( JtJ, &_part, cvRect(eofs, eofs, 6, 6) );
                cvGEMM( Je, Je, 1, &_part, 1, &_part, CV_GEMM_A_T );

                cvGetRows( JtErr, &_part, eofs, eofs + 6 );
                cvGEMM( Je, err, 1, &_part, 1, &_part, CV_GEMM_A_T );

            }

            if( errNorm )
                *errNorm += l2err*l2err;


			
			//projector Time
			objpt_i = cvMat(1, ni, CV_64FC3, proj_objectPoints->data.db + ofs*3);
            imgpt_i = cvMat(1, ni, CV_64FC2, imagePoints[1]->data.db + ofs*2);

            if( JtJ || JtErr )
                cvProjectPoints2( &objpt_i, &om[1], &T[1], &K[1], &Dist[1],
                        &tmpimagePoints, dpdrot, dpdt, dpdf, dpdc, dpdk, 0);
            else
                cvProjectPoints2( &objpt_i, &om[1], &T[1], &K[1], &Dist[1], &tmpimagePoints );
            cvSub( &tmpimagePoints, &imgpt_i, &tmpimagePoints );

            l2err = cvNorm( &tmpimagePoints, 0, CV_L2 );
            maxErr = cvNorm( &tmpimagePoints, 0, CV_C );

            if( JtJ || JtErr )
            {
                int iofs = (n_boards+1)*6 + NINTRINSIC, eofs = (i+1)*6;
                assert( JtJ && JtErr );

                for( p = 0; p < ni*2; p++ )
                {
                    CvMat de3dr3 = cvMat( 1, 3, CV_64F, Je->data.ptr + Je->step*p );
                    CvMat de3dt3 = cvMat( 1, 3, CV_64F, de3dr3.data.db + 3 );
                    CvMat de3dr2 = cvMat( 1, 3, CV_64F, J_LR->data.ptr + J_LR->step*p );
                    CvMat de3dt2 = cvMat( 1, 3, CV_64F, de3dr2.data.db + 3 );
                    double _de3dr1[3], _de3dt1[3];
                    CvMat de3dr1 = cvMat( 1, 3, CV_64F, _de3dr1 );
                    CvMat de3dt1 = cvMat( 1, 3, CV_64F, _de3dt1 );

                    cvMatMul( &de3dr3, &dr3dr1, &de3dr1 );
                    cvMatMul( &de3dt3, &dt3dt1, &de3dt1 );

                    cvMatMul( &de3dr3, &dr3dr2, &de3dr2 );
                    cvMatMulAdd( &de3dt3, &dt3dr2, &de3dr2, &de3dr2 );

                    cvMatMul( &de3dt3, &dt3dt2, &de3dt2 );

                    cvCopy( &de3dr1, &de3dr3 );
                    cvCopy( &de3dt1, &de3dt3 );
                }

                cvGetSubRect( JtJ, &_part, cvRect(0, 0, 6, 6) );
                cvGEMM( J_LR, J_LR, 1, &_part, 1, &_part, CV_GEMM_A_T );

                cvGetSubRect( JtJ, &_part, cvRect(eofs, 0, 6, 6) );
                cvGEMM( J_LR, Je, 1, 0, 0, &_part, CV_GEMM_A_T );

                cvGetRows( JtErr, &_part, 0, 6 );
                cvGEMM( J_LR, err, 1, &_part, 1, &_part, CV_GEMM_A_T );

                cvGetSubRect( JtJ, &_part, cvRect(eofs, eofs, 6, 6) );
                cvGEMM( Je, Je, 1, &_part, 1, &_part, CV_GEMM_A_T );

                cvGetRows( JtErr, &_part, eofs, eofs + 6 );
                cvGEMM( Je, err, 1, &_part, 1, &_part, CV_GEMM_A_T );

            }

            if( errNorm )
                *errNorm += l2err*l2err;
        }
    }

    cvRodrigues2( &om_LR, &R_LR );
    if( proj_rotation_vectors->rows == 1 || proj_rotation_vectors->cols == 1 )
        cvConvert( &om_LR, proj_rotation_vectors );
    else
        cvConvert( &R_LR, proj_rotation_vectors );
    cvConvert( &T_LR, proj_translation_vectors );

    cvReleaseMat( &err );
    cvReleaseMat( &J_LR );
    cvReleaseMat( &Je );
    cvReleaseMat( &Ji );
    cvReleaseMat( &RT0 );
	}