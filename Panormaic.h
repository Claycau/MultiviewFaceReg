#ifndef H_Panoramic
#define H_Panoramic
//找line要去blending
#include "GraphicProcessing.h"
//紀錄特徵點位置
#include "Coordinate.h"
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;

class Panoramic
{
	public:
			Panoramic(IplImage *img,bool debug = false);									//用於使用其他內部function時，如GetHsvFeature。
			Panoramic(const char *leftName, const char *centerName,const char *rightName,bool debug = false);//一般無修改照片之名稱
			~Panoramic();
			IplImage* StitchFace(const char *leftName, const char *centerName,const char *rightName);//有特徵點之名稱
			IplImage* GetHsvFeature(IplImage* src,int H = 0,int S = 255,int V = 255,int Scale = 51,int Scale_1 = 51,int Scale_2 = 51);
			vector<Coordinate> FindFeatureCoord(IplImage *img);
			void GetCenterFaceCoord(Coordinate* getCoord);						//用於儲存中間人臉的特徵於資料庫
			void RearrangeCoord(vector<Coordinate>& srcCoord);					//重新排列特徵點的順序，由左到右並且由上到下
			void ShowFeature(vector<Coordinate> featureCoord);
	private:
			void DoAffineTrsform(IplImage *src,IplImage *dst,vector<Coordinate> adjustedCoord,vector<Coordinate> adjustCoord);//T:Threshold of SSD's error;
			void AffineTransform(IplImage *src,IplImage *dst,CvMat *Matrix);
			void PrintMatrix(CvMat *Matrix,int Rows,int Cols,int Channels=1);
			void ShowStitch(IplImage *profile,IplImage *center);
			bool SSD(Coordinate* coord1,Coordinate* coord2,CvMat *Matrix,double Threshold);
			double LinearBlending(double T,double Position,double Alpha);
			CvMat* GetAffineMatrix(vector<Coordinate> adjustCoord,vector<Coordinate> adjustedCoord);	
	private:
			Coordinate m_centerCood[12];
			bool   m_do_sttich;
			bool   m_debug;
			int    m_height,m_width;
			int	   m_numFeature;
			int	   m_lineT;//threshold for line postion and accelerate
			int    m_sample;
			double m_slope;
			double m_intercept;
			double m_saveSlope[2];
			double m_saveIntercept[2];
			IplImage *m_leftImg,*m_centerImg,*m_rightImg;
			IplImage *m_PanoramicFace;
			CvMat	 *m_affineMatrix;
};
#endif