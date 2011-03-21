#include "pageFinder.h"
#include <stdio.h>
#include <vector>

using namespace std;

struct Line {
	CvPoint p1,p2;
};


CvRect findPageBound(IplImage* src)
{
	IplImage* null = NULL;
	return findPageBound(src,null,false);
}

CvRect findPageBound(IplImage* src,bool visual)
{
	IplImage* null = NULL;
	return findPageBound(src,null,visual);
}

CvRect findPageBound(IplImage* src,IplImage*& threshOut)
{
	return findPageBound(src,threshOut,false);
}


CvRect findPageBound(IplImage* src,IplImage*& threshOut,bool visual)
{
	IplImage* half = cvCreateImage(cvSize(src->width/2,src->height/2),src->depth,3);
	IplImage* quar = cvCreateImage(cvSize(half->width/2,half->height/2),src->depth,3);
	IplImage* eight = cvCreateImage(cvSize(quar->width/2,quar->height/2),quar->depth,3);
	cvPyrDown(src,half);
	cvPyrDown(half,quar);
	cvPyrDown(quar,eight);

	cvNamedWindow("PageFinder");
	if(visual) {
		cvShowImage("PageFinder",eight);
		cvWaitKey();
	}

	//cvPyrDown(eight,sixt);
	//cvPyrDown(sixt,thirt);
	IplImage* hsv = cvCreateImage(cvGetSize(eight),src->depth,3);
	cvCvtColor(eight,hsv,CV_BGR2HSV);

	IplImage* h = cvCreateImage(cvGetSize(hsv),hsv->depth,1);
	IplImage* s = cvCreateImage(cvGetSize(hsv),hsv->depth,1);
	IplImage* v = cvCreateImage(cvGetSize(hsv),hsv->depth,1);

	IplImage* b = cvCreateImage(cvGetSize(hsv),hsv->depth,1);
   	IplImage* g = cvCreateImage(cvGetSize(hsv),hsv->depth,1);
   	IplImage* r = cvCreateImage(cvGetSize(hsv),hsv->depth,1);

	cvSplit(hsv,h,s,v,NULL);
	cvSplit(eight,b,g,r,NULL);

	cvSmooth(h,h);
	cvDilate(h,h,NULL,3);
	cvErode(h,h,NULL,6);
	cvDilate(h,h,NULL,3);

	cvSmooth(s,s);
	cvDilate(s,s,NULL,2);
   	cvErode(s,s,NULL,4);
   	cvDilate(s,s,NULL,2);


	IplImage* sThresh = cvCreateImage(cvGetSize(h),h->depth,1);
	IplImage* sThresh2 = cvCreateImage(cvGetSize(h),h->depth,1);
	IplImage* hThresh = cvCreateImage(cvGetSize(h),h->depth,1);


	IplImage* bThresh = cvCreateImage(cvGetSize(h),h->depth,1);
	IplImage* gThresh = cvCreateImage(cvGetSize(h),h->depth,1);
	IplImage* rThresh = cvCreateImage(cvGetSize(h),h->depth,1);

	CvRect top = cvRect(0,0,s->width,s->width/4);
	cvSetImageROI(s,top);
	cvSetImageROI(sThresh,top);
	cvSetImageROI(sThresh2,top);
	cvSetImageROI(b,top);
	cvSetImageROI(g,top);
	cvSetImageROI(r,top);
	cvSetImageROI(bThresh,top);
	cvSetImageROI(gThresh,top);
	cvSetImageROI(rThresh,top);

   	cvThreshold(s,sThresh,150,100,CV_THRESH_BINARY);
   	cvThreshold(h,hThresh,60,100,CV_THRESH_BINARY_INV);
	if(visual) {
		cvShowImage("PageFinder",hThresh);
		cvWaitKey();
	}

   	cvThreshold(s,sThresh2,5,100,CV_THRESH_BINARY_INV);

   	cvThreshold(b,bThresh,20,100,CV_THRESH_BINARY_INV);
   	cvThreshold(g,gThresh,20,100,CV_THRESH_BINARY_INV);
   	cvThreshold(r,rThresh,20,100,CV_THRESH_BINARY_INV);

	cvResetImageROI(s);
	cvResetImageROI(sThresh);
	cvResetImageROI(sThresh2);
	cvResetImageROI(bThresh);
	cvResetImageROI(gThresh);
	cvResetImageROI(rThresh);

	IplImage* mask = cvCreateImage(cvGetSize(h),h->depth,1);
	IplImage* smask = cvCreateImage(cvGetSize(h),h->depth,1);
	IplImage* bgrmask = cvCreateImage(cvGetSize(h),h->depth,1);
	cvOr(sThresh,sThresh2,smask);
	cvAnd(bThresh,gThresh,bgrmask);
	cvAnd(rThresh,bgrmask,bgrmask);
	cvOr(smask,bgrmask,smask);

	if(visual) {
		cvShowImage("PageFinder",sThresh);
		cvWaitKey();
		cvShowImage("PageFinder",sThresh2);
		cvWaitKey();
		cvShowImage("PageFinder",bgrmask);
		cvWaitKey();
		cvShowImage("PageFinder",smask);
		cvWaitKey();
	}

	cvSub(hThresh,smask,mask);					
	cvErode(mask,mask);
	cvDilate(mask,mask);

	if(visual) {
		cvShowImage("PageFinder",hThresh);
		cvWaitKey();
		cvShowImage("PageFinder",mask);
		cvWaitKey();
	}

	IplImage* maskSrc = cvCreateImage(cvGetSize(mask),mask->depth,1);

	cvCopy(mask,maskSrc);

	cvReleaseImage(&b);
	cvReleaseImage(&g);
	cvReleaseImage(&r);
	cvReleaseImage(&bThresh);
	cvReleaseImage(&gThresh);
	cvReleaseImage(&rThresh);
	cvReleaseImage(&bgrmask);
	cvReleaseImage(&sThresh);
	cvReleaseImage(&sThresh2);
	cvReleaseImage(&smask);
	

//	cvSaveImage("hThresh.png",hThresh);

	CvSeq* contour;
	CvMemStorage* sto = cvCreateMemStorage(0);

	CvContourScanner scan = cvStartFindContours(mask,sto);

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


	cvSetZero(mask);
	cvDrawContours(mask,lCont,cvScalarAll(255),cvScalarAll(255),-1);
	
	if(visual) {
		cvShowImage("PageFinder",mask);
		cvWaitKey();
	}

	/*CvSeq* cvx = cvConvexHull2(lCont,sto,CV_CLOCKWISE,1);
	for(int i=0;i<cvx->total;i++) {
		CvPoint* line = (CvPoint*)cvGetSeqElem(cvx,i);
		cvLine(hThresh,line[0],line[1],CV_RGB(255,255,255),3,8);
	}*/
	CvSeq *lines = cvHoughLines2( mask, sto, CV_HOUGH_PROBABILISTIC, 1,2* CV_PI/180, 20, 25, 10 );
	vector< vector<Line> > mLines;
	for(int i=0;i<lines->total;i++) {
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
		if( fabs(line[0].x-line[1].x) < 5 ) {
			//cvLine( eight, line[0], line[1], CV_RGB(255,255,255), 3, 8 );
			if(mLines.size() == 0) {
				vector<Line> temp;
				Line l;
				l.p1 = line[0];
				l.p2 = line[1];
				temp.push_back(l);
				mLines.push_back(temp);
			}
			else {
				bool added = false;
				for(int i=0;i<(int)mLines.size();i++) {
					if(abs(line[0].x - mLines[i][mLines[i].size()-1].p2.x) < 30) {
						Line l;
						l.p1 = line[0];
						l.p2 = line[1];
						mLines[i].push_back(l);
						added = true;
						break;
					}
				}
	
				if(!added) {
					vector<Line> temp;
					Line l;
					l.p1 = line[0];
					l.p2 = line[1];
					temp.push_back(l);
					mLines.push_back(temp);
				}
			}
		}
	}

	int one = -1;
	int two = -1;
	int oneVal = 0;
	int twoVal = 0;

	//Get two largest groups of lines, assuming they are the page lines
	for(int i=0;i<(int)mLines.size();i++) {
		int size = (int)mLines[i].size();
		if(size > oneVal) {
			twoVal = oneVal;
			oneVal = size;
			two = one;
			one = i;
		}
		else if(size > twoVal) {
			twoVal = size;
			two = i;
		}
	}


	/*/printf("%d groups\n",(int)mLines.size());
	for(int j=0;j<(int)mLines[one].size();j++) {
		cvLine(eight, mLines[one][j].p1, mLines[one][j].p2, CV_RGB(255,255,255),3,8);
	}
	cvShowImage("PageFinder",eight);
	cvWaitKey();
	for(int j=0;j<(int)mLines[two].size();j++) {
		cvLine(eight, mLines[two][j].p1, mLines[two][j].p2, CV_RGB(255,255,255),3,8);
	}
	cvShowImage("PageFinder",eight);
	cvWaitKey();*/

	double leftSlope,rightSlope;
	double total1 = 0;
	CvPoint top1 = mLines[one][0].p1;
	CvPoint top2 = mLines[two][0].p1;
	CvPoint bot1 = top1;
	CvPoint bot2 = top2;
	for(int i=0;i<(int)mLines[one].size();i++) {
		Line l = mLines[one][i];
		if(l.p1.y < top1.y)
			top1 = l.p1;
		if(l.p2.y < top1.y)
			top1 = l.p2;
		if(l.p1.y > bot1.y)
			bot1 = l.p1;
	}
	double total2 = 0;
	for(int i=0;i<(int)mLines[two].size();i++) {
		Line l = mLines[two][i];
		total2 += (double)(l.p2.y - l.p1.y)/(double)(l.p2.x-l.p1.x);
		if(l.p1.y < top2.y)
			top2 = l.p1;
		if(l.p2.y < top2.y)
			top2 = l.p2;
		if(l.p1.y > bot2.y)
			bot2 = l.p1;
		if(l.p2.y > bot2.y)
			bot2 = l.p2;
	}


	CvPoint topLeft,botLeft,topRight,botRight;
	if(mLines[one][0].p1.x < mLines[two][0].p1.x) {
		leftSlope = (double)(bot1.y - top1.y)/(double)(bot1.x-top1.x);
		rightSlope = (double)(bot2.y - top2.y)/(double)(bot2.x-top2.x);
		//printf("%f total2 %f size\n",total2,(double)mLines[two].size());
		topLeft = top1;
		botLeft = bot1;
		topRight = top2;
		botRight = bot2;
	}
	else {
		rightSlope = (double)(bot1.y - top1.y)/(double)(bot1.x-top1.x);
		leftSlope = (double)(bot2.y - top2.y)/(double)(bot2.x-top2.x);
		//printf("%f total2 %f size\n",total2,(double)mLines[two].size());
		topLeft = top2;
		botLeft = bot2;
		topRight = top1;
		botRight = bot1;
	}

	topLeft.x -= 10;
	botLeft.x -= 10;
	topRight.x += 10;
	botRight.x += 10;



	CvPoint intPointLeft;
	bool isInt = false;
	for(int i=topLeft.y;i<eight->height;i++){
		int newx = ((i-topLeft.y) * (1/leftSlope))+topLeft.x;
		unsigned char* yPtr = (unsigned char*)(maskSrc->imageData + i*maskSrc->widthStep);
		int pixVal = (int)yPtr[newx];
		if(pixVal != 0 && !isInt) {
			intPointLeft = cvPoint(newx,i);
			isInt=true;
		}
		if(pixVal == 0 && isInt) //Skip over random intersections and look for last one (with perspective change)
			isInt =false;
	
	}
	isInt = false;
	CvPoint intPointRight;
	for(int i=topRight.y;i<eight->height;i++){
		int newx = ((i-topRight.y) * (1/rightSlope))+topRight.x;
		unsigned char* yPtr = (unsigned char*)(maskSrc->imageData + i*maskSrc->widthStep);
		int pixVal = (int)yPtr[newx];
		if(pixVal != 0 && !isInt) {
			intPointRight = cvPoint(newx,i);
			isInt = true;
		}
		if(pixVal == 0 && isInt)
			isInt = false;
	}

	CvRect pageBound = cvBoundingRect(lCont);


	int xTopLeft = ((pageBound.y-topLeft.y) * (1/leftSlope))+topLeft.x;
	int xTopRight = ((pageBound.y-topRight.y) * (1/rightSlope))+topRight.x;

	if(visual) {
		cvLine(eight,topLeft,botLeft,CV_RGB(255,255,255),1,8);
		cvLine(eight,topRight,botRight,CV_RGB(255,255,255),1,8);
		cvLine(maskSrc,topLeft,botLeft,CV_RGB(255,255,255),1,8);
		cvLine(maskSrc,topRight,botRight,CV_RGB(255,255,255),1,8);

		cvShowImage("PageFinder",eight);
		cvWaitKey();
		cvLine(eight,cvPoint(0,pageBound.y),cvPoint(eight->width-1,pageBound.y),CV_RGB(255,255,255),1,8);

		cvShowImage("PageFinder",eight);
		cvWaitKey();
	}
	if(intPointLeft.x < xTopLeft)
		pageBound.x = intPointLeft.x;
	else
		pageBound.x = xTopLeft;
	
	if(intPointRight.x > xTopRight)
		pageBound.width = intPointRight.x - pageBound.x;
	else
		pageBound.width = xTopRight - pageBound.x;

	if(intPointLeft.y > intPointRight.y)
		pageBound.height = intPointLeft.y - pageBound.y;
	else
		pageBound.height = intPointRight.y - pageBound.y;

	CvPoint p1,p2;
	p1.x = pageBound.x;
	p1.y = pageBound.y;
	p2.x = p1.x + pageBound.width;
	p2.y = p1.y + pageBound.height;

	if(visual) {
		cvRectangle(eight,p1,p2,CV_RGB(255,255,255),1);
		cvRectangle(maskSrc,p1,p2,CV_RGB(255,255,255),1);
		cvShowImage("PageFinder",eight);
		cvWaitKey();
	}
	pageBound.x *= 8;
	pageBound.y *= 8;
	pageBound.width *= 8;
	pageBound.height *= 8;
	cvReleaseImage(&hsv);
	cvReleaseImage(&h);
	cvReleaseImage(&s);
	cvReleaseImage(&v);
	cvReleaseImage(&hThresh);
	cvReleaseImage(&half);
	cvReleaseImage(&quar);
	cvReleaseImage(&eight);

	if(threshOut != NULL) 
		threshOut = maskSrc;
	else 
		cvReleaseImage(&maskSrc);

	return pageBound;
}
