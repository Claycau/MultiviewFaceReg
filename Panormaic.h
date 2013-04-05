#ifndef H_Panoramic
#define H_Panoramic
//��line�n�hblending
#include "GraphicProcessing.h"
//�����S�x�I��m
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
			Panoramic(IplImage *img,bool debug = false);									//�Ω�ϥΨ�L����function�ɡA�pGetHsvFeature�C
			Panoramic(const char *leftName, const char *centerName,const char *rightName,bool debug = false);//�@��L�ק�Ӥ����W��
			~Panoramic();
			IplImage* StitchFace(const char *leftName, const char *centerName,const char *rightName);//���S�x�I���W��
			IplImage* GetHsvFeature(IplImage* src,int H = 0,int S = 255,int V = 255,int Scale = 51,int Scale_1 = 51,int Scale_2 = 51);
			vector<Coordinate> FindFeatureCoord(IplImage *img);
			void GetCenterFaceCoord(Coordinate* getCoord);						//�Ω��x�s�����H�y���S�x���Ʈw
			void RearrangeCoord(vector<Coordinate>& srcCoord);					//���s�ƦC�S�x�I�����ǡA�ѥ���k�åB�ѤW��U
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