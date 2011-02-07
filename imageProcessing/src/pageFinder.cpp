#include "pageFinder.h"
#include <stdio.h>

CvRect findPageBound(IplImage* src)
{
	IplImage* hsv = cvCreateImage(cvGetSize(src),src->depth,3);
	cvCvtColor(src,hsv,CV_BGR2HSV);

	IplImage* h = cvCreateImage(cvGetSize(hsv),hsv->depth,1);
	IplImage* s = cvCreateImage(cvGetSize(hsv),hsv->depth,1);
	IplImage* v = cvCreateImage(cvGetSize(hsv),hsv->depth,1);

	cvSplit(hsv,h,s,v,NULL);

	cvSmooth(h,h);
	cvDilate(h,h,NULL,5);
	cvErode(h,h,NULL,15);
	cvDilate(h,h,NULL,10);
	//cvSaveImage("h.png",h);

	IplImage* hThresh = cvCreateImage(cvGetSize(h),h->depth,1);

	cvThreshold(h,hThresh,60,100,CV_THRESH_BINARY_INV);
	IplImage* hThreshSrc = cvCreateImage(cvGetSize(h),h->depth,1);

	cvCopy(hThresh,hThreshSrc);

//	cvSaveImage("hThresh.png",hThresh);

	CvMemStorage* sto = cvCreateMemStorage(0);
	CvSeq* contour;

	CvContourScanner scan = cvStartFindContours(hThresh,sto);

	//cvNamedWindow("Contours",1);
	contour = cvFindNextContour(scan);
	CvSeq* lCont = NULL;
	double lLength = 0;
	while(contour != NULL)
	{
		double newLength = cvContourPerimeter(contour);
		if(newLength > lLength) {
			lCont = contour;
			lLength = newLength;
		}

		contour = cvFindNextContour(scan);
	}
		
	//cout << lLength << endl;

	//cvDrawContours(hThreshSrc,lCont,cvScalarAll(255),cvScalarAll(255),-1);
	CvRect pageBound = cvBoundingRect(lCont);
	cvReleaseImage(&hsv);
	cvReleaseImage(&h);
	cvReleaseImage(&s);
	cvReleaseImage(&v);
	cvReleaseImage(&hThresh);

	return pageBound;
}
