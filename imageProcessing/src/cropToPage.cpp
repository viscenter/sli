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
    int inWidth,inHeight,xOffset,yOffset;
	bool f_overwrite = false;
	bool f_height = false;
	bool f_width = false;
	bool f_outfile = false;
	bool f_xoff= false;
	bool f_yoff = false;
	bool f_useoff = false;
	bool f_visual = false;

	int border = 0;

	while((c = getopt(argc,argv,"ro:w:h:x:y:vb:")) != -1) {
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
			case 'x':
				f_xoff = true;
				xOffset = atoi(optarg);
				break;
			case 'y':
				f_yoff = true;
				yOffset = atoi(optarg);
			case 'v':
				f_visual = true;
				break;
			case 'b':
				border = atoi(optarg);
		}
				
	}
	if(optind == argc) {
		cout << "Usage: cropToPage <Input File> [options] \n";
		cout << "Options:\n";
		cout << "	-o <File> -- Supply output file\n";
		cout << "	-w <Width> -- Supply width for new image\n";
		cout << "	-h <Height> -- Supply height for new image\n";
		cout << "	-x <xOffset>-- Supply x offset for page bound\n";
		cout << "	-y <yOffset>-- Supply y offest for page bound\n";
		cout << "	-r -- Overwrite input image with new image\n";
		cout << "	-v -- Visualize page determination\n";
		cout << "	-b <Border>-- Add padding around page size\n";
		return 0;
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
	
    if(f_xoff && f_yoff) {
		if(f_width && f_height)
			f_useoff = true;
		else
			printf("Must supply width and height when using offset. Using pageFinder.\n");
	}

	else if(f_xoff || f_yoff)
		printf("Must supply both x and y offsets. Using pageFinder to get offset\n");

    IplImage* src = cvLoadImage(inFile);
	CvRect pageBound;

	if(f_useoff) 
		pageBound = cvRect(xOffset,yOffset,inWidth,inHeight);
	else {
	    CvPoint p1,p2;
  	    pageBound = findPageBound(src,f_visual);
	    p1.x = pageBound.x;
	    p1.y = pageBound.y;
	    p2.x = pageBound.x + pageBound.width;
	    p2.y = pageBound.y + pageBound.height;
	
	    if(f_width) {
	        if(pageBound.width > inWidth) {//Input width is smaller than size of page
	            printf("WARNING: input width (%d) is less than size of page (%d). Page is likely to be cut.\n",inWidth,pageBound.width);
	        }
	        int shift = (inWidth - pageBound.width) / 2;
	        if((inWidth - pageBound.width) % 2 != 0) {//Difference is odd so move left over one extra pixel
	            if(inWidth - pageBound.width < 0)
	                shift += 1;
	            else
	                shift -= 1;
	        }
	        if(pageBound.x - shift >= 0) {
				if((pageBound.x - shift) + inWidth >= src->width)
					pageBound.x = src->width - inWidth;
				else
					pageBound.x -= shift;
			}
			else
				pageBound.x = 0;

			pageBound.width = inWidth;
	    }
	
	    if(f_height) {
	        if(pageBound.height > inHeight) {//Input height is smaller than size of page
	            printf("WARNING: input height (%d) is less than size of page (%d). Page is likely to be cut.\n",inHeight,pageBound.height);
	        }
	        int shift = (inHeight - pageBound.height) / 2;
	        if((inHeight - pageBound.height) % 2 != 0) {//Difference is odd so move up/down one extra pixel
	            if(inHeight - pageBound.height < 0)
	                shift += 1;
	            else
	                shift -= 1;
	        }
	        if(pageBound.y - shift >= 0) {
				if((pageBound.y - shift) + inHeight >= src->height)
					pageBound.y = src->height - inHeight;
				else
					pageBound.y -= shift;
			}
			else
				pageBound.y = 0;

			pageBound.height = inHeight;
	
	    }
		if(border) {
			pageBound.x -= border;
			pageBound.width += 2*border;
			pageBound.y -= border;
			pageBound.height += 2*border;

			if(pageBound.x < 0)
				pageBound.x = 0;
			if(pageBound.y < 0)
				pageBound.y = 0;
			if(pageBound.x + pageBound.width >= src->width)
				pageBound.width = src->width-pageBound.x;
			if(pageBound.y + pageBound.height >= src->height)
				pageBound.height = src->height-pageBound.y;
		}

	}
    cvSetImageROI(src,pageBound);
    IplImage* cropped = cvCreateImage( cvGetSize(src), src->depth, 3);
    cvCopy(src,cropped);
	printf("OriginalWidth: %d OriginalHeigth: %d NewWidth: %d NewHeight: %d xOffset: %d yOffset: %d\n",
			src->width,src->height,cropped->width,cropped->height,pageBound.x,pageBound.y);
	printf("Saving image: %s\n",outFile);

    cvSaveImage(outFile,cropped);
	cvReleaseImage(&src);
	cvReleaseImage(&cropped);
    return 0;
} 
