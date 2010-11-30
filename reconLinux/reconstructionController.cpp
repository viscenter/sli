#include <stdlib.h>
#include <stdio.h>

#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "../common/cvCalibrateProCam.h"
#include "../common/cvScanProCam.h"
#include "../common/cvUtilProCam.h"
#include "../common/globals.h"

const char CONFIG_FILE[] = "./config.xml";

bool loadSLConfigXML(slParams *sl_params, slCalib *sl_calib,
    const char *configFile = CONFIG_FILE,
    const char *outputDir = NULL)
{
  // Read structured lighting parameters from configuration file.
  FILE* pFile = fopen(configFile, "r");
  if(pFile != NULL){
    fclose(pFile);
    fprintf(stderr, "Reading configuration file...\n");
    readConfiguration(configFile, sl_params);
  }
  else{
    return false;
  }

  // Allocate storage for calibration parameters.
  int cam_nelems                  = sl_params->cam_w*sl_params->cam_h;
  int proj_nelems                 = sl_params->proj_w*sl_params->proj_h;
  sl_calib->cam_intrinsic_calib   = false;
  sl_calib->proj_intrinsic_calib  = false;
  sl_calib->procam_extrinsic_calib= false;
  sl_calib->cam_intrinsic         = cvCreateMat(3,3,CV_32FC1);
  sl_calib->cam_distortion        = cvCreateMat(5,1,CV_32FC1);
  sl_calib->cam_extrinsic         = cvCreateMat(2, 3, CV_32FC1);
  sl_calib->proj_intrinsic        = cvCreateMat(3, 3, CV_32FC1);
  sl_calib->proj_distortion       = cvCreateMat(5, 1, CV_32FC1);
  sl_calib->proj_extrinsic        = cvCreateMat(2, 3, CV_32FC1);
  sl_calib->cam_center            = cvCreateMat(3, 1, CV_32FC1);
  sl_calib->proj_center           = cvCreateMat(3, 1, CV_32FC1);
  sl_calib->cam_rays              = cvCreateMat(3, cam_nelems, CV_32FC1);
  sl_calib->proj_rays             = cvCreateMat(3, proj_nelems, CV_32FC1);
  sl_calib->proj_column_planes    = cvCreateMat(sl_params->proj_w, 4, CV_32FC1);
  sl_calib->proj_row_planes       = cvCreateMat(sl_params->proj_h, 4, CV_32FC1);

  if(outputDir) strcpy(sl_params->outdir, outputDir);
  
  // Load intrinsic camera calibration parameters (if found).
  char str1[1024], str2[1024];
  sprintf(str1, "%s/calib/cam/cam_intrinsic.xml",  sl_params->outdir);
  sprintf(str2, "%s/calib/cam/cam_distortion.xml", sl_params->outdir);
  if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
    sl_calib->cam_intrinsic  = (CvMat*)cvLoad(str1);
    sl_calib->cam_distortion = (CvMat*)cvLoad(str2);
    sl_calib->cam_intrinsic_calib = true;
    fprintf(stderr, "Loaded previous intrinsic camera calibration.\n");
  }

  // Load intrinsic projector calibration parameters (if found);
  sprintf(str1, "%s/calib/proj/proj_intrinsic.xml",  sl_params->outdir);
  sprintf(str2, "%s/calib/proj/proj_distortion.xml", sl_params->outdir);
  if( ((CvMat*)cvLoad(str1) != 0) && ((CvMat*)cvLoad(str2) != 0) ){
    sl_calib->proj_intrinsic  = (CvMat*)cvLoad(str1);
    sl_calib->proj_distortion = (CvMat*)cvLoad(str2);
    sl_calib->proj_intrinsic_calib = true;
    fprintf(stderr, "Loaded previous intrinsic projector calibration.\n");
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
    fprintf(stderr, "Loaded previous extrinsic projector-camera calibration.\n");
  }
  
  // Initialize background model.
  sl_calib->background_depth_map = cvCreateMat(
      sl_params->cam_h, sl_params->cam_w, CV_32FC1);
  sl_calib->background_image     = cvCreateImage(
                                    cvSize(sl_params->cam_w, sl_params->cam_h),
                                    IPL_DEPTH_8U, 3);
  sl_calib->background_mask      = cvCreateImage(
                                    cvSize(sl_params->cam_w, sl_params->cam_h),
                                    IPL_DEPTH_8U, 1);
  cvSet(sl_calib->background_depth_map, cvScalar(FLT_MAX));
  cvZero(sl_calib->background_image);
  cvSet(sl_calib->background_mask, cvScalar(255));
  
  return true;
}

int filter(const struct dirent* ent)
{
  return strcasecmp(ent->d_name+strlen(ent->d_name)-4, ".tif") == 0;
}

int getLatestImages(const char *dirName,
    IplImage**& imagesBuffer, int numImages)
{
  struct dirent **namelist = NULL;
  std::string baseName(dirName);
  baseName += "/Processed/";
  int num = scandir( baseName.c_str(), &namelist, filter, versionsort );
  printf("Found %d images in '%s'...\n", num, dirName);
  if( num < 0 ) return 0;

  int count = 0;
  if( num >= numImages )
  {
    for( int i=0; i<numImages; ++i )
    {
      imagesBuffer[i] = cvLoadImage(
          (baseName + namelist[num-numImages+i]->d_name).c_str());
      fprintf(stderr, "Loading image '%s' to %d...\n",
          namelist[num-numImages+i]->d_name, i);
      if( imagesBuffer[i] ) ++count;
    }
  }

  for( int i=0; i<num; ++i ) free(namelist[i]);
  free(namelist);

  return count;
}

void reconstructSurface(
    const char* baseName, slParams* sl_params, slCalib* sl_calib)
{
  IplImage** proj_gray_codes = NULL;
  int gray_ncols, gray_nrows;
  int gray_colshift, gray_rowshift;
  
  generateGrayCodes(sl_params->proj_w, sl_params->proj_h, proj_gray_codes, 
  gray_ncols, gray_nrows, gray_colshift, gray_rowshift, 
  sl_params->scan_cols, sl_params->scan_rows);

  IplImage **cam_gray_codes = new IplImage*[22];
  int numImages = getLatestImages(baseName, cam_gray_codes, 22);

  IplImage* gray_decoded_cols = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_16U, 1);
  IplImage* gray_decoded_rows = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_16U, 1);
  IplImage* gray_mask         = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U,  1);
  
  decodeGrayCodes(sl_params->proj_w, sl_params->proj_h,
          cam_gray_codes, 
          gray_decoded_cols, gray_decoded_rows, gray_mask,
          gray_ncols, gray_nrows, 
          gray_colshift, gray_rowshift, 
          sl_params->thresh);
  
  char str[1024], outputDir[1024];
  mkdir(sl_params->outdir, 0755);
  sprintf(outputDir, "3D/%s", baseName);
  //sprintf(outputDir, "%s/%s", sl_params->outdir, baseName);
  mkdir("3D", 0755);
  mkdir(outputDir, 0755);

  // Display and save the correspondences.
  if(sl_params->display)
    displayDecodingResults(gray_decoded_cols, gray_decoded_rows, gray_mask, sl_params);

  // Reconstruct the point cloud and depth map.
  //printf("Reconstructing the point cloud and the depth map...\n");
  CvMat *points    = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
  CvMat *colors    = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);
  CvMat *depth_map = cvCreateMat(sl_params->cam_h, sl_params->cam_w, CV_32FC1);
  CvMat *mask      = cvCreateMat(1, sl_params->cam_h*sl_params->cam_w, CV_32FC1);

  CvMat *resampled_points = cvCreateMat(3, sl_params->cam_h*sl_params->cam_w, CV_32FC1);

  reconstructStructuredLight(sl_params, sl_calib, 
                 cam_gray_codes[0],
                 gray_decoded_cols, gray_decoded_rows, gray_mask,
                 points, colors, depth_map, mask);
  
  downsamplePoints(sl_params, sl_calib, points, mask, resampled_points, depth_map);

  double min_val, max_val;
  cvMinMaxLoc(depth_map, &min_val, &max_val);
  
  // Display and save the depth map.
  if(sl_params->display)
    displayDepthMap(depth_map, gray_mask, sl_params);

    //printf("Saving the depth map...\n");
  IplImage* depth_map_image = cvCreateImage(cvSize(sl_params->cam_w, sl_params->cam_h), IPL_DEPTH_8U, 1);
  for(int r=0; r<sl_params->cam_h; r++){
    for(int c=0; c<sl_params->cam_w; c++){
      char* depth_map_image_data = (char*)(depth_map_image->imageData + r*depth_map_image->widthStep);
      if(mask->data.fl[sl_params->cam_w*r+c])
        depth_map_image_data[c] = 
          255-int(255*(depth_map->data.fl[sl_params->cam_w*r+c]-sl_params->dist_range[0])/
            (sl_params->dist_range[1]-sl_params->dist_range[0]));
      else
        depth_map_image_data[c] = 0;
    }
  }
  CvMat* dist_range = cvCreateMat(1, 2, CV_32FC1);
  cvmSet(dist_range, 0, 0, sl_params->dist_range[0]);
  cvmSet(dist_range, 0, 1, sl_params->dist_range[1]);

  sprintf(str, "%s/depth_map.png", outputDir);
  printf("%s\n",str);
  cvSaveImage(str, depth_map_image);
  sprintf(str, "%s/depth_map_range.xml", outputDir);
  cvSave(str, dist_range);
  cvReleaseImage(&depth_map_image);
  cvReleaseMat(&dist_range);


  // Save the texture map.
  //printf("Saving the texture map...\n");
  sprintf(str, "%s/%s.png", outputDir, baseName);
  cvSaveImage(str, cam_gray_codes[0]);

  // Save the point cloud.
  //printf("Saving the point cloud...\n");
  sprintf(str, "%s/%s.ply", outputDir, baseName);
  //if(savePointsPLY(str, resampled_points, NULL, NULL, mask, sl_params->proj_w, sl_params->proj_h)){
  if(savePointsPLY(str, resampled_points, NULL, NULL, mask, sl_params->cam_w, sl_params->cam_h)){
  //if(savePointsPLY(str, points, NULL, NULL, mask, sl_params->cam_w, sl_params->cam_h)){
    fprintf(stderr, "Saving the reconstructed point cloud failed!\n");
    return;
  }

  sprintf(str,"%s/proj_intrinsic.xml", outputDir);	
  cvSave(str, sl_calib->proj_intrinsic);
  sprintf(str,"%s/proj_distortion.xml", outputDir);
  cvSave(str, sl_calib->proj_distortion);
  sprintf(str,"%s/cam_intrinsic.xml", outputDir);	
  cvSave(str, sl_calib->cam_intrinsic);
  sprintf(str,"%s/cam_distortion.xml", outputDir);
  cvSave(str, sl_calib->cam_distortion);
  sprintf(str, "%s/cam_extrinsic.xml", outputDir);
  cvSave(str, sl_calib->cam_extrinsic);
  sprintf(str, "%s/proj_extrinsic.xml", outputDir);
  cvSave(str, sl_calib->proj_extrinsic);


  // Free allocated resources.
  cvReleaseImage(&gray_decoded_cols);
  cvReleaseImage(&gray_decoded_rows);
  cvReleaseImage(&gray_mask);
  cvReleaseMat(&points);
  cvReleaseMat(&colors);
  cvReleaseMat(&depth_map);
  cvReleaseMat(&mask);
  cvReleaseMat(&resampled_points);
  for(int i=0; i<(gray_ncols+gray_nrows+1); i++)
    cvReleaseImage(&proj_gray_codes[i]);
  delete[] proj_gray_codes;
  for(int i=0; i<2*(gray_ncols+gray_nrows+1); i++)
    cvReleaseImage(&cam_gray_codes[i]);
  delete[] cam_gray_codes;

  return;
}

int main(int argc, char *argv[])
{
  slParams sl_params;
  slCalib sl_calib;

  if( argc < 2 )
  {
    fprintf(stderr, "Usage: %s image_dir [config_file [calib_dir]]\n", argv[0]);
    exit(0);
  }

  if(!loadSLConfigXML(&sl_params, &sl_calib,
        argc>2 ? argv[2] : CONFIG_FILE,
        argc>3 ? argv[3] : NULL))
    fprintf(stderr, "Could not load the Structured Light Configuration XML file!\nPlease correct the file and restart the program.\n");
  else
    fprintf(stderr, "Configuration File Read Successfully!\n");

  printf("outdir: %s\n", sl_params.outdir);
  printf("%d x %d\n", sl_params.cam_w, sl_params.cam_h);
  printf("%d x %d\n", sl_params.proj_w, sl_params.proj_h);
  reconstructSurface(argv[1], &sl_params, &sl_calib);

  cvReleaseMat(&sl_calib.cam_intrinsic);
  cvReleaseMat(&sl_calib.cam_distortion);
  cvReleaseMat(&sl_calib.cam_extrinsic);
  cvReleaseMat(&sl_calib.proj_intrinsic);
  cvReleaseMat(&sl_calib.proj_distortion);
  cvReleaseMat(&sl_calib.proj_extrinsic);
  cvReleaseMat(&sl_calib.cam_center);
  cvReleaseMat(&sl_calib.proj_center);
  cvReleaseMat(&sl_calib.cam_rays);
  cvReleaseMat(&sl_calib.proj_rays);
  cvReleaseMat(&sl_calib.proj_column_planes);
  cvReleaseMat(&sl_calib.proj_row_planes);
  cvReleaseMat(&sl_calib.background_depth_map);
  cvReleaseImage(&sl_calib.background_image);
  cvReleaseImage(&sl_calib.background_mask);

  return 0;
}
