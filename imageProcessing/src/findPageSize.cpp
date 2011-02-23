#include <stdio.h>
#include "pageFinder.h"

int main(int argc, char** argv)
{
	if(argc < 2)
		printf("Usage: findPageSize <Page Image>");
 
    IplImage* src = cvLoadImage(argv[1]);
	CvRect pageBound = findPageBound(src);
	printf("xOffset: %d yOffset: %d Width: %d Height: %d\n",pageBound.x,pageBound.y,pageBound.width,pageBound.height);
	return 0;
}
