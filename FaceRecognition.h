#ifndef FaceRecognition_H
#define FaceRecognition_H
#include "Panormaic.h"
#include "Recognition.h"
#include "Morphining.h"
#include "ResultFeature.h"
#include "Coordinate.h"
#include "POHE.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <cv.h>
#include <string>
#include <vector>
#ifdef X86_RELEASE
#pragma comment(lib,"pohevc10x86v1.0.lib")
#endif
#ifdef X86_DEBUG
#pragma comment(lib,"pohevc10x86v1.0d.lib")
#endif
using namespace std;

#define WIDTHR   2
#define HEIGHTR  2
#define N_UNEHENCE		0
#define N_EHENCE		1
#define EAR_UNEHENCE	2
#define EAR_EHENCE		3
class FaceRecognition
{
	public:
		FaceRecognition(int recognition_type = 0,bool debug = false);
		void StartRegistration(bool debug = false);
		void StartRecognition();
		void StartRecognition(int index,vector<vector<double>>&pre,vector<vector<double>>&recall,bool debug = false);
		void InitSetting(const char* filename,bool debug = false);
	private:
		int GetFaceInf(Coordinate *coord1,Coordinate *coord2);
		IplImage* Enhence(IplImage* src,int w,int h);
		void Enhence(string filename,int w,int h);
		void ConstructPanoramicFace(bool debug = false);
		void ConstructFeature();
		void NormalizeTestImg(int IndexPicture);
		IplImage* save_warp(IplImage* Img,int person);
		void PCA();
		bool InitRecog(int IndexPicture);
		bool InitRegister();
		bool SetWarp();
		void get_pohe_size(int &pohe_w,int &pohe_h,const IplImage* normalize_img);
		IplImage* Procrustes(IplImage *Img,Coordinate *centerCoord,int Mode = 0);	
	private:
		vector<vector<Coordinate>> m_TraningCoord;
		vector<Coordinate> m_testCoord;
		IplImage *m_PanoramicFace;
		IplImage *m_normalizePanoramicFace;
		IplImage *m_TestColorImage;//¬°¤Fprewarping
		IplImage *m_TestGrayImage;//
		IplImage *m_TestImage;
		double m_threshold;
		char *m_resultFile;
		int m_pcaDimension;
		int m_numRegister;
		int m_numSample;
		int m_height;
		int m_width;
		int m_numBlock;
		int m_numSelectBlock;
		int m_startIndex;
		int m_FaceInf;
		int m_Reg_Type;
		bool m_debug;
};
#endif