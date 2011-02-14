/***********************************************************
  Code to remove ink from page by inpainting.

  First, the image is thresholded to isolate the page.
  Then, the contour of the page is selected and a Hough
  line detector is applied to find nearly horizontal lines.
  Thresholded components that intersect these lines are
  assumed to be ink, while other marks are preserved.
  The contours of the inked letters are then inpainted,
  leaving only a blank page behind.
 ***********************************************************/

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <stdio.h>

int main( int argc, char *argv[] )
{
  if( argc < 2 )
  {
    fprintf( stderr, "Usage: %s image\n", argv[0] );
    exit(-1);
  }

  IplImage * orig = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
  if( orig )
  {
    //cvEqualizeHist( orig, orig );
    IplImage *half = cvCreateImage( cvSize(orig->width/2,orig->height/2),
        IPL_DEPTH_8U, 1 );
    IplImage *quart = cvCreateImage( cvSize(half->width/2,half->height/2),
        IPL_DEPTH_8U, 1 );
    IplImage *eight = cvCreateImage( cvSize(quart->width/2,quart->height/2),
        IPL_DEPTH_8U, 1 );
    //IplImage *image = cvCreateImage( cvSize(half->width/4,half->height/4),
    //    IPL_DEPTH_8U, 1 );
    cvPyrDown( orig, half );
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    //IplImage *orig = cvCloneImage(image);
    IplImage *image = cvCloneImage(orig);
    cvSmooth(image, image);

    cvNamedWindow( "orig" );
    cvShowImage( "orig", eight );
    cvNamedWindow( "threshold" );

    //cvThreshold( orig, image, 50, 255, CV_THRESH_BINARY );
    //cvPyrDown( image, half );
    //cvPyrDown( half, quart );
    //cvPyrDown( quart, eight );
    //cvShowImage( "threshold", eight );
    //cvWaitKey(0);

    cvAdaptiveThreshold( orig, image, 255,
        CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 25, 5 );
    cvPyrDown( image, half );
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    //cvShowImage( "threshold", eight );
    //cvWaitKey(0);

    IplImage *out = cvCloneImage(orig);
    cvNot( image, image );
    //cvErode( image, image );
    //cvErode( image, image );
    //cvDilate( image, image );
    //cvDilate( image, image );
    //cvDilate( image, image );
    //cvDilate( image, image );
    cvPyrDown( image, half );
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    cvShowImage( "threshold", eight );
    cvWaitKey(500);

    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *objects = NULL;
    cvFindContours( image, storage, &objects,
        sizeof(CvContour), CV_RETR_LIST );
    cvSetZero( out );
    for( CvSeq *next = objects; next; next = next->h_next )
    {
      CvRect rect = cvBoundingRect( next, 0 );
      if( fabs( cvContourArea(next) ) > 200 &&
          rect.height < 300 && rect.height > 50 &&
          rect.width > 25 && rect.width < 800 )
      {
        cvDrawContours( out, next, CV_RGB(255,255,255), CV_RGB(255,255,255),
            0, CV_FILLED );
      }
    }
    cvSaveImage( "out.png", out );
    cvPyrDown( out, half );
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    cvShowImage( "threshold", eight );
    cvWaitKey(0);

    /*CvSeq *lines = cvHoughLines2( eight, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 100, 0, 0 );

    for( int i = 0; i < MIN(lines->total,100); i++ )
    {
        float* line = (float*)cvGetSeqElem(lines,i);
        float rho = line[0];
        float theta = line[1];
        CvPoint pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        cvLine( eight, pt1, pt2, CV_RGB(127,127,127), 3, 8 );
    }*/
    CvSeq *lines = cvHoughLines2( eight, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 50, 50, 10 );
    int count = 0;
    for( int i = 0; i < lines->total; i++ )
    {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        if( fabs(line[0].y-line[1].y) < 10 )
        {
          cvLine( eight, line[0], line[1], CV_RGB(i/4+75,i/4+75,i/4+75), 3, 8 );
          ++count;
        }
    }
    printf( "count = %d\n", count );
    cvShowImage( "threshold", eight );
    cvWaitKey(0);

    cvSetZero( out );
    for( CvSeq *next = objects; next; next = next->h_next )
    {
      bool found = false;
      CvRect rect = cvBoundingRect( next, 0 );
      if( fabs( cvContourArea(next) ) > 200 &&
          rect.height < 300 && rect.height > 50 && rect.width > 25 )
      {
        for( int i = 0; i < lines->total; i++ )
        {
            CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
            if( fabs(line[0].y-line[1].y) < 20 )
            {
              CvPoint2D32f line0 = cvPoint2D32f( line[0].x*8, line[0].y*8 );
              CvPoint2D32f line1 = cvPoint2D32f( line[1].x*8, line[1].y*8 );
              if( ( ( rect.x > line0.x && rect.x < line1.x ) ||
                    ( rect.x > line1.x && rect.x < line0.x ) ||
                    ( rect.x+rect.width > line0.x &&
                      rect.x+rect.width < line1.x ) ||
                    ( rect.x+rect.width > line1.x &&
                      rect.x+rect.width < line0.x ) ) &&
                  ( ( rect.y < line0.y && rect.y+rect.height > line0.y ) ||
                    ( rect.y < line1.y && rect.y+rect.height > line1.y ) ) )
              {
                cvDrawContours( out, next, CV_RGB(255,255,255),
                    CV_RGB(255,255,255), 0, CV_FILLED );
                break;
              }
              else if( cvPointPolygonTest( next, line0, 0 ) > 0 ||
                  cvPointPolygonTest( next, line1, 0 ) > 0 )
              {
                cvDrawContours( out, next, CV_RGB(255,255,255),
                    CV_RGB(255,255,255), 0, CV_FILLED );
                break;
              }

            }
        }
      }
    }
    cvSaveImage( "out.png", out );
    cvPyrDown( out, half );
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    cvShowImage( "threshold", eight );
    cvWaitKey(0);

    IplImage *thresh = cvCloneImage(orig);
    cvAnd(orig,out,thresh);
    cvPyrDown(thresh,half);
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    cvShowImage( "threshold", eight );
    cvWaitKey(0);
    cvThreshold(thresh,thresh,100,255.,CV_THRESH_BINARY);
    cvPyrDown(thresh,half);
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    cvShowImage( "threshold", eight );
    cvWaitKey(0);
    cvXor(out,thresh,out);
    cvPyrDown(out,half);
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    cvShowImage( "threshold", eight );
    cvWaitKey(0);
    cvReleaseImage(&thresh);

    cvDilate(out,out);
    cvPyrDown(out,half);
    cvPyrDown( half, quart );
    cvPyrDown( quart, eight );
    cvShowImage( "threshold", eight );
    cvWaitKey(0);
    cvSaveImage( "out-dilate.png", out );

    cvPyrDown( out, half );
    IplImage *q = cvCloneImage(quart);
    cvPyrDown( half, q );
    //cvPyrDown( orig, half );
    //cvPyrDown( half, quart );
    //IplImage * orig = cvLoadImage( argv[1] );
    IplImage *ohalf = cvCreateImage( cvSize(orig->width/2,orig->height/2),
        IPL_DEPTH_8U, 1 );
    IplImage *oquart = cvCreateImage( cvSize(ohalf->width/2,ohalf->height/2),
        IPL_DEPTH_8U, 1 );
    cvPyrDown( orig, ohalf );
    cvPyrDown( ohalf, oquart );
    //image = cvCloneImage(oquart);
    image = cvCloneImage(ohalf);
    //cvInpaint( oquart, q, image, 15, CV_INPAINT_TELEA );
    cvInpaint( ohalf, half, image, 15, CV_INPAINT_TELEA );
    cvSaveImage( "out-13.png", image );
    //cvPyrDown( image, half );
    //cvPyrDown( image, quart );
    cvPyrDown( image, quart );
    cvPyrDown( quart, eight );
    cvShowImage( "threshold", eight );
    cvWaitKey(0);

    cvReleaseImage(&image);
    image = cvLoadImage(argv[1]);
    cvInpaint( image, out, image, 15, CV_INPAINT_TELEA );
    cvSaveImage( "out-color.jpg", image );

    cvReleaseImage( &out );
    //cvReleaseImage( &orig );
    cvReleaseImage( &image );
    cvReleaseImage( &quart );
    cvReleaseImage( &half );
    cvReleaseImage( &orig );
  }
  return 0;
}

