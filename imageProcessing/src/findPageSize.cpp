#include <stdio.h>
#include <unistd.h>
#include "pageFinder.h"

int main(int argc, char** argv)
{

	int c;
	char* inFile;
	bool f_visual = false;
	while((c = getopt(argc,argv,"v")) != -1) {
		switch(c) {
			case 'v':
				f_visual = true;
				break;
		}
	}

	if(optind == argc) {
		printf("Usage: findPageSize <Page Image> [options]\n");
		printf("Options:\n");
		printf("	-v -- visualize page finding process\n");
	}
	else 
		inFile = argv[optind];
				
    IplImage* src = cvLoadImage(inFile);
	CvRect pageBound = findPageBound(src,f_visual);
	printf("xOffset: %d yOffset: %d Width: %d Height: %d\n",pageBound.x,pageBound.y,pageBound.width,pageBound.height);
	return 0;
}
