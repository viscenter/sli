/*Pagefinder.h: Header to include for tool to found a bound on the page in an image

*	Use findPageBound(IplImage* src) for simple use. If you need access to the
*	thresholded page image, pass an image where at should be stored as threshOut*	
*/


#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;

/* findPageBound(IplImage* src)
* recturns a rectangle bounding the page within the image
*/
CvRect findPageBound(IplImage* src);



/*
* Returns the same rectangle, and threshOut is changed to point to the thresholded image for further use
*/
CvRect findPageBound(IplImage* src,IplImage*& threshOut);
