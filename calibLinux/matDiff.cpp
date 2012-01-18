#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>

#include <dirent.h>
#include <errno.h>
#include <vector>

#include <cv.h>
#include <highgui.h>
#include <netinet/in.h>

using namespace std;

int main(int argc, char* argv[])
{
  if(argc < 3) {
    cerr << argv[0] << " file1.xml file2.xml" << endl;
    return 1;
  }

  CvMat * file[2];
  file[0] = (CvMat*)cvLoad(argv[1]);
  file[1] = (CvMat*)cvLoad(argv[2]);

  for(int i = 0; i < 2; i++) {
    cout << argv[i+1] << ":";
    cout << "\t\t" << file[i]->rows << "\tx\t" << file[i]->cols << "\tmatrix of type "
      << CV_MAT_TYPE(file[i]->type) << " x "
      << CV_MAT_DEPTH(file[i]->type) << " x "
      << CV_MAT_CN(file[i]->type) << endl;
    for(int r = 0; r < file[i]->rows; r++) {
      for(int c = 0; c < file[i]->cols; c++) {
        cout << cvmGet(file[i], r, c) << "\t";
      }
      cout << endl;
    }
    cout << endl;
  }

  if((file[0]->height != file[1]->height) || (file[0]->width != file[1]->width)) {
    cerr << "Dimension mismatch! Can't do further diffing." << endl;
    return 1;
  }
  if(file[0]->type != file[1]->type) {
    cerr << "Type mismatch! Can't do further diffing." << endl;
    return 1;
  }
 
  CvMat * diff = cvCreateMat(file[0]->rows, file[0]->cols, file[0]->type);
  cvAbsDiff(file[0], file[1], diff);

  cout << "Diff:" << endl;
  for(int r = 0; r < diff->rows; r++) {
    for(int c = 0; c < diff->cols; c++) {
      cout << cvmGet(diff, r, c) << "\t";
    }
    cout << endl;
  }
  
  cout << endl << "Sum: " << cvSum(diff).val[0] << endl;

  return 0;
}
