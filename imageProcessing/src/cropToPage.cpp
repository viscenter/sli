#include "pageFinder.h"
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    if(argc < 3 || argc == 4 || argc > 5) {
        cout << "Usage: cropToPage <file to crop> <new file name>[<new width> <new height>]" << endl;
        return 0;
    }
    bool useSize = false;
    int inWidth,inHeight;
    if(argc == 5) { //Size has been supplied
        useSize = true;
        inWidth = atoi(argv[3]);
        inHeight = atoi(argv[4]);
    }

    IplImage* src = cvLoadImage(argv[1]);
    char* newFileName = argv[2];

    CvPoint p1,p2;
    CvRect pageBound = findPageBound(src);
    p1.x = pageBound.x;
    p1.y = pageBound.y;
    p2.x = pageBound.x + pageBound.width;
    p2.y = pageBound.y + pageBound.height;

    if(useSize) {
        if(pageBound.width > inWidth || pageBound.height > inHeight) {//Input size is smaller than size of page
            printf("WARNING: input size (%dX%d) is less than size of page (%dX%d). Page is likely to be cut.\n",inWidth,inHeight,pageBound.width,pageBound.height);
        }
        p1.y -= (inHeight - pageBound.height);
        p1.x -= (inWidth - pageBound.width) / 2;
        p2.x += (inWidth - pageBound.width) / 2;
        if((inWidth - pageBound.width) % 2 != 0) {//Difference is odd so move left over one extra pixel
            if(inWidth - pageBound.width < 0)
                p1.x += 1;
            else
                p1.x -= 1;
        }

        pageBound = cvRect(p1.x,p1.y,inWidth,inHeight);
    }
    IplImage* cropped = cvCreateImage( cvSize(p2.x-p1.x, p2.y-p1.y), src->depth, 3);
    cvSetImageROI(src,pageBound);
    cvCopy(src,cropped);
    cvSaveImage(newFileName,cropped);
    return 0;
}                                  
