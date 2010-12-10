
#include <stdio.h>
#include <string.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <vector>

int main(int argc, char *argv[])
{
  char * obj_file = argc > 1 ? argv[1] : NULL;
  char * img_file = argc > 2 ? argv[2] : NULL;
  char * mtl_file = argc > 3 ? argv[3] : NULL;

  FILE *file = argc > 1 ? fopen(obj_file, "r") : stdin;

  char buffer[1024];
  std::vector<CvPoint3D32f> vertices;
  bool first = true;

  while( file && fgets(buffer, sizeof(buffer), file) )
  {
    CvPoint3D32f v;
    if( sscanf(buffer, "v %f %f %f", &v.x, &v.y, &v.z ) == 3 )
      vertices.push_back(v);
    if( buffer[0] == 'f' )
    {
      if( first )
      {
        first = false;
        CvMat *intr = (CvMat*)cvLoad("calib/proj/cam_intrinsic.xml");
        CvMat *dist = (CvMat*)cvLoad("calib/proj/cam_distortion.xml");
        CvMat *rot = cvCreateMat( 3, 1, CV_32FC1 );
        CvMat *tra = cvCreateMat( 3, 1, CV_32FC1 );
        cvSetZero(rot); cvSetZero(tra);

        CvMat points = cvMat( vertices.size(), 3, CV_32FC1, &vertices[0] );
        CvMat *text = cvCreateMat( vertices.size(), 2, CV_32FC1 );
        cvProjectPoints2( &points, rot, tra, intr, dist, text );

        for( int i=0; i<vertices.size(); ++i )
        {
          float x = text->data.fl[i*2]/1804.;
          float y = 1.-text->data.fl[i*2+1]/1353.;
          if( x < 0 ) x = 0; if( y < 0 ) y = 0;
          if( x > 1 ) x = 1; if( y > 1 ) y = 1;
          printf("vt %f %f\n", x, y );
        }

        if( img_file )
        {
          printf("mtllib %s\n", mtl_file);
          printf("usemtl default\n\n");

          //char mtl_file[1024];
          //snprintf(mtl_file, sizeof(mtl_file), "%s.mtl", img_file );
          FILE *mtl = fopen(mtl_file, "w");
          if( mtl )
          {
            fprintf(mtl, "newmtl default\n");
            fprintf(mtl, "Ka 0.5 0.5 0.5\n");
            fprintf(mtl, "Kd 0.5 0.5 0.5\n");
            fprintf(mtl, "Ks 0.5 0.5 0.5\n");
            fprintf(mtl, "Tf 0.5 0.5 0.5\n");
            fprintf(mtl, "illum 1\nd 1\nNs 6\nsharpness 60\nNi 1\n");
            if( img_file)
              fprintf(mtl, "map_Kd %s\n", img_file);
            fclose(mtl);
          }
        }

        cvReleaseMat( &text );
        cvReleaseMat( &intr ); cvReleaseMat( &dist );
        cvReleaseMat( &rot ); cvReleaseMat( &tra );
      }

      int x, y, z, i, j, k;
      if( sscanf( buffer, "f %d//%d %d//%d %d//%d",
            &x, &i, &y, &j, &z, &k ) == 6 )
      {
        printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n",
            x, x, i, y, y, j, z, z, k );
      }
      else if( sscanf(buffer, "f %d %d %d",
	    &x, &y, &z ) == 3) 
      {
     	printf("f %d/%d %d/%d %d/%d\n", x,x,y,y,z,z);
      }
      else
        printf("%s", buffer);
    }
    else
      printf("%s", buffer);
  }
  if(file) fclose(file);

  return 0;
}

