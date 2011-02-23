#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	ifstream inFile;
	inFile.open(argv[1]);
	string buffer;

	if(argc < 8) {
		printf("USAGE: ./reTexture <obj file> <original width> <original height> <new width> <new height><x offset> <y offset>  >  <new obj>\n");
		return 0;
	}

	int orWidth = atoi(argv[2]);
	int orHeight = atoi(argv[3]);
	int newWidth = atoi(argv[4]);
	int newHeight = atoi(argv[5]);
	int xOffset = atoi(argv[6]);
	int yOffset = atoi(argv[7]);
	
	while(getline(inFile,buffer)) {
		CvPoint2D32f v;
		if( sscanf(buffer.c_str(),"vt %f %f",&v.x,&v.y) == 2) {
			int x = v.x * orWidth;
			int y = (1.-v.y) * orHeight;
			int newX = x-xOffset;
			int newY = y-yOffset;
			float xRat = (float)newX / (float)newWidth;
			float yRat = 1.-((float)newY / (float)newHeight);
			if(xRat < 0)
				xRat = 0;
			if(xRat > 1)
				xRat = 1;
			if(yRat < 0)
				yRat = 0;
			if(yRat > 1)
				yRat = 1;

			printf("vt %f %f\n",xRat,yRat);

		}
		else 
			printf("%s\n",buffer.c_str());
	}

	inFile.close();
	return 0;
}


