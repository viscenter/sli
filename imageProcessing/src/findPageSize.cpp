#include <stdio.h>
#include "pageFinder.h"

int main(int argc, char** argv)
{
	if(argc < 2)
		printf("Usage: findPageSize <Page Image>");
 
    IplImage* src = cvLoadImage(argv[1]);
	CvRect pageBound = findPageBound(src);
	printf("%d %d\n",pageBound.width,pageBound.height);
	return 0;
}
