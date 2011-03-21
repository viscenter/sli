/*Pagefinder.h: Header to include for tool to found a bound on the page in an image

*	Use findPageBound(IplImage* src) for simple use. If you need access to the
*	thresholded page image, pass an image where at should be stored as threshOut*	
*/


#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;

/* findPageBound(IplImage* src)
* recturns a rectangle bounding the page within the image, no visualization
*/
CvRect findPageBound(IplImage* src);

//Returns page bound and option for visualization of process
CvRect findPageBound(IplImage* src,bool visual);

//Returns page bound and threshOut points to thresholded image, no visualization
CvRect findPageBound(IplImage* src,IplImage*& threshOut);


/*
* Returns the same rectangle, and threshOut is changed to point to the thresholded image for further use, option for visualization
*/
CvRect findPageBound(IplImage* src,IplImage*& threshOut,bool visual);
