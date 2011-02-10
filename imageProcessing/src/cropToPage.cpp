#include "pageFinder.h"
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//Initialize flags for options
	int c;
	char* outFile;
	char* inFile;
    int inWidth,inHeight;
	bool f_overwrite = false;
	bool f_height = false;
	bool f_width = false;
	bool f_outfile = false;

	while((c = getopt(argc,argv,"ro:w:h:")) != -1) {
		switch(c) {
			case 'r':
				f_overwrite = true;
				break;
			case 'o':
				f_outfile = true;
				outFile = optarg;
				break;
			case 'w':
				f_width = true;
				inWidth = atoi(optarg);
				break;
			case 'h':
				f_height = true;
				inHeight = atoi(optarg);
				break;
		}
				
	}
	if(optind == argc) {
		cout << "Usage: cropToPage <Input File> [options] \n";
		cout << "Options:\n";
		cout << "	-o <File> -- Supply output file\n";
		cout << "	-w <Width> -- Supply width for new image\n";
		cout << "	-h <Height> -- Supply height for new image\n";
		cout << "	-r -- Overwrite input image with new image\n";
	}

	else {
		inFile = argv[optind];
	}

	if(f_outfile && f_overwrite)
		printf("Output file supplied. Ignoring overwrite.\n");

	
	if(!f_outfile) {
		if(f_overwrite)
			outFile = inFile;
		else { //Set to deafult output file [inFile]_crop.png
			char str[1024];

			size_t length = strlen(inFile)-4;

			strncpy(str,inFile,length);
			str[(int)length] = '\0';
			strcat(str,"_crop.png");

			outFile = str;
		}
	}
	
    

    IplImage* src = cvLoadImage(inFile);

    CvPoint p1,p2;
    CvRect pageBound = findPageBound(src);
    p1.x = pageBound.x;
    p1.y = pageBound.y;
    p2.x = pageBound.x + pageBound.width;
    p2.y = pageBound.y + pageBound.height;

    if(f_width) {
        if(pageBound.width > inWidth) {//Input width is smaller than size of page
            printf("WARNING: input width (%d) is less than size of page (%d). Page is likely to be cut.\n",inWidth,pageBound.width);
        }
        p1.x -= (inWidth - pageBound.width) / 2;
        p2.x += (inWidth - pageBound.width) / 2;
        if((inWidth - pageBound.width) % 2 != 0) {//Difference is odd so move left over one extra pixel
            if(inWidth - pageBound.width < 0)
                p1.x += 1;
            else
                p1.x -= 1;
        }
        pageBound = cvRect(p1.x,p1.y,inWidth,pageBound.height);
    }

    if(f_height) {
        if(pageBound.height > inHeight) {//Input height is smaller than size of page
            printf("WARNING: input height (%d) is less than size of page (%d). Page is likely to be cut.\n",inHeight,pageBound.height);
        }

		int difference = inHeight - pageBound.height;
		int add=1;
		if(difference < 0) {
			difference = -difference;
			add = -1;
		}
		int remaining = difference;

		//Bottom bound is likely near bottom of image, so check to make sure we dont exceed the image height during expansion
		while(p2.y < src->height-1 && remaining > difference/2) {
			p2.y += add;
			remaining--;
		}
		p1.y -= add*remaining;

        pageBound = cvRect(p1.x,p1.y,pageBound.width,inHeight);
    }
    IplImage* cropped = cvCreateImage( cvSize(p2.x-p1.x, p2.y-p1.y), src->depth, 3);
    cvSetImageROI(src,pageBound);
    cvCopy(src,cropped);
	printf("Saving image: %s\n",outFile);
    cvSaveImage(outFile,cropped);
    return 0;
}                                  //Set to deafult output file cropOut.png
