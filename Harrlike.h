#ifndef H_Harrlike
#define H_Harrlike

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include "Coordinate.h"
#include <math.h>

using namespace std;
using namespace cv;

class Harrlike
{
	public:
			Harrlike(IplImage *img,
					 Coordinate *adjustedCoord,
					 String Cascade_Name,
					 int sizeX,
					 int sizeY,
					 int Do_pyramids = 0,
					 int color = 0);
			Harrlike(const char *filename,String Cascade_Name,int do_pyramids,int color);
			~Harrlike();
			IplImage* DetectSaveROI(  );
			void SaveROI(IplImage *Img,CvSeq *faces);
			IplImage* Translation(IplImage *Img,int x , int y);
			IplImage* NormalizeHeight(IplImage *Img,double m);

	private:
			void ResizeImage(IplImage* img,CvSize new_size);//resize image 
			void Draw_Rectangle(CvSeq *faces,double scale);

	private:
			CvHaarClassifierCascade* cascade;
			Coordinate m_TraningCoord[12];
			IplImage *m_srcImage;
			IplImage *m_OriginalImage;
			IplImage *m_normalizeImage;
			double scale;
			bool m_do;
			int m_numMenber;
			int m_PyrDown;
			int m_justSet;
			int m_Color;
			int m_width;
			int m_height;
};
#endif