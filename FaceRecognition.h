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
class FaceRecognition
{
	public:
		FaceRecognition(bool debug = false);
		void StartRegistration(bool debug = false);
		void StartRecognition();
		void StartRecognition(int index,bool debug = false);
		void InitSetting(const char* filename,bool debug = false);
	private:
		int GetFaceInf(Coordinate *coord1,Coordinate *coord2);
		IplImage* Enhence(IplImage* src);
		void Enhence(string filename);
		void ConstructPanoramicFace(bool debug = false);
		void ConstructFeature();
		void NormalizeTestImg(int IndexPicture);
		//void PreWarping(int person);
		void save_warp(IplImage* Img,int person);
		void PCA();
		bool InitRecog(int IndexPicture);
		bool InitRegister();
		bool SetWarp();
		//IplImage* NorDatabaseImg(IplImage *panoramic_face,Coordinate *centerCoord);
		IplImage* Procrustes(IplImage *Img,Coordinate *centerCoord,int charMode = 0);	
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
		bool m_debug;
};
#endif