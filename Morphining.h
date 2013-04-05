#ifndef H_Morphining
#define H_Morphining

#ifndef FRONATAL
#define FRONATAL 1
#define PROFILELEFT 2
#define PROFILERIGHT 3
#endif
#include "Coordinate.h"
#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace std;
using namespace cv;
//Num_Pair : ´X²Õ¸Ñ
class Morphining
{
  public:
			Morphining(int num_Feature,IplImage *img_in,bool debug = false);
			~Morphining();
			CvMat*   GetAffineMatrix(vector<Coordinate> coord1,vector<Coordinate> coord2,bool debug = false);
			void	 AffineTrsform(IplImage *src,IplImage *dst,CvMat *Matrix,int type = 0,bool debug = false);
			void     DoAffineTrsform(IplImage *src,IplImage *dst,vector<Coordinate> coord1,vector<Coordinate> coord2,double T = 0,int type = 0,bool debug = false);//T:Threshold of SSD's error;
			CvMat*   GetProjectionMatrix(vector<Coordinate> adjust,vector<Coordinate> adjusted,bool debug = false);
			void     DoProjection(IplImage *src,IplImage *dst,vector<Coordinate> adjusted,vector<Coordinate> adjust,double T = 0,bool debug = false);//T:Threshold of SSD's error;
			void	 ProjectionTrsform(IplImage *src,IplImage *dst,CvMat *Matrix,bool debug = false);
			void     PrintMatrix(CvMat *Matrix,int Rows,int Cols,int Channels);
			bool     SSD(Coordinate* coord1,Coordinate* coord2,CvMat *Matrix,double Threshold);

			bool     Point_in_Polygon(Coordinate *v,int x,int y,int numVertex);
			void     ReArrangeCoord(Coordinate *coord);//By degree
			void     RearrangeCoord(Coordinate *srcCoord);
			void	 ShowFeature(Coordinate* featureCoord);
			
  private:
			CvMat	  *U;
			CvMat     *W;
			CvMat     *T;
			CvMat     *MulResult;
			CvMat     *Inv;
			double    *  m;
			double    *  b;
			int       m_numFeature;
			int       m_width,m_height;
			Coordinate *Vertex;
			bool	  m_debug;
};
#endif