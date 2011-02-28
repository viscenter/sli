#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <stdio.h>
#include "../../imageProcessing/src/pageFinder.h"

using namespace cv;
using namespace std;

const float Z_BUFF = 25;

typedef unsigned char uchar;

int main(int argc, char* argv[])
{
	if(argc < 3) {
		cout << "Usage: applyMask [obj file to mask] [image file for mask] " << endl;
		return 0;
	}

	vector<CvPoint3D32f> vertices;
	vector<CvPoint3D32f> normals;

	ifstream inFile;
	inFile.open(argv[1]);

	if(!inFile) {
		cout << "Could not open object file" << endl;
		return 0;
	}

	string buffer;
	while(getline(inFile,buffer)) {//Read in vertices
		CvPoint3D32f v;
		CvPoint3D32f vn;
    		if( sscanf(buffer.c_str(), "v %f %f %f", &v.x, &v.y, &v.z ) == 3 )
      			vertices.push_back(v);
		if(sscanf(buffer.c_str(), "vn %f %f %f", &vn.x, &vn.y, &vn.z ) == 3 )
			normals.push_back(vn);
	}

	inFile.close();

	IplImage* src = cvLoadImage(argv[2]);
	
	IplImage* mask = src; //Initilaize mask to a non-null value
	CvRect pageBound = findPageBound(src,mask); //get page bound rectangle as well as an image mask of the page
	CvPoint p1,p2;
	p1.x = pageBound.x;
	p1.y = pageBound.y;
	p2.x = pageBound.x + pageBound.width;
	p2.y = pageBound.y + pageBound.height;


	//cout<< p1.x << ", " << p1.y << endl;
	//cout << p2.x << ", " << p2.y << endl;

	//cvRectangle(hThreshSrc,p1,p2,cvScalarAll(100)); 

	//cvShowImage("Contours",hThreshSrc);
	//cvWaitKey();

	//cvSaveImage("src.png",src);
	//cvSaveImage("h.png",hThresh);
	//cvSaveImage("s.png",s);
	//cvSaveImage("v.png",v);i
	//cvSaveImage("cont.png",hThreshSrc);
	//cvReleaseImage(&src);
	/*cvReleaseImage(&hsv);
	cvReleaseImage(&h);
	cvReleaseImage(&s);
	cvReleaseImage(&hThresh);*/
	//cvReleaseImage(&hThreshSrc);
	//cvReleaseImage(&v);

	//Assume that nearly all of the points in the middle of the vertices are		 on the page plane

	CvMat *intr = (CvMat*)cvLoad("calib/proj/cam_intrinsic.xml");
        CvMat *dist = (CvMat*)cvLoad("calib/proj/cam_distortion.xml");
        CvMat *rot = cvCreateMat( 3, 1, CV_32FC1 );
        CvMat *tra = cvCreateMat( 3, 1, CV_32FC1 );
        cvSetZero(rot); cvSetZero(tra);

	//Generate image coordinates
        CvMat points = cvMat( vertices.size(), 3, CV_32FC1, &vertices[0] );
        CvMat *imgPts = cvCreateMat( vertices.size(), 2, CV_32FC1 );
        cvProjectPoints2( &points, rot, tra, intr, dist, imgPts );

        for( int i=0; i<vertices.size(); ++i )
        {
	  //Get image coord ratios for each vertex
          float x_rat = imgPts->data.fl[i*2]/1804.;
          float y_rat = imgPts->data.fl[i*2+1]/1353.;
          if( x_rat < 0 ) x_rat = 0; if( y_rat < 0 ) y_rat = 0;
          if( x_rat > 1 ) x_rat = 1; if( y_rat > 1 ) y_rat = 1;

	  int x = x_rat*src->width;
	  int y = y_rat*src->height;

	  int xMask = x/8; //Account for pyrDown of mask image
	  int yMask = y/8;

	  //Get a pointer to the right row of the image
	  uchar* yPtr = (uchar*)(mask->imageData + yMask * mask->widthStep);
	  //Get the value of the xth pixel in the row
	  int pixVal = (int)yPtr[xMask];

	  bool inRect = (x >= p1.x && x <= p2.x && y>= p1.y && y <= p2.y);


	  if(inRect && pixVal != 0/* && inZPlane*/) {
		  CvPoint3D32f v = vertices[i];
		  printf("v %f %f %f\n",v.x,v.y,v.z);
	  }

          //printf("vt %f %f\n", x, y );
        }

	cvReleaseImage(&src);
	cvReleaseImage(&mask);
	return 0;
}
