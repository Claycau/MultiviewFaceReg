#ifndef H_Recognition
#define H_Recognition

#include <cv.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <highgui.h>//GUI介面的函式庫
#include <cxcore.h>//電腦視覺函式庫
#include <cvaux.h>//電腦視覺函式庫
#include "PCA.h"
#include "RecognitionMethod.h"
#include "ResultFeature.h"
using namespace std;
using namespace cv;

#ifndef  PI
#define     PI				 3.141592656
#endif
#ifndef FRONATAL
#define FRONATAL     1
#define PROFILELEFT  2
#define PROFILERIGHT 3
#endif

typedef  vector<int>  Dim1;
typedef  vector<Dim1> Dim2;
typedef  vector<Dim2> Dim3;
typedef  vector<Dim3> Dim4;
typedef  vector<Dim4> Dim5;

#define  SMALL_BLOCK_SIZE     2
#define  BIG_BLOCK_SIZE       5
#define  START_M  1
#define  ADDM     BIG_BLOCK_SIZE

#define  Gax				 15
#define	 Gay				 15
#define  Gradius			 Gay/2	
/*			DoType			*/
#define MAG_TYPE			 0
#define PHASE_TYPE			 1
#define Test_MAG_TYPE		 2
#define Test_PHASE_TYPE		 3
#define PCA_TYPE			 4
/*			DoType			*/

#define freq_start 0.3
#define freq_end 0.7
#define freq_add 0.1

class Recognition:public RecognitionMethod
{
	public:
			Recognition(const char* filename,int projection,int numWidthBlock,int numHeightBlock,int numSample,int numRegister
						,char* resultFile,double threshold,int FaceInf,bool debug = false);
			~Recognition();
			
			void CreateGaborImg(int PhaScale,double FreqScale);
			void CreateHistogram(IplImage* Img,int TypeImg,int scale);
			void ConcatenatingHistogram(int DoType);
			void Construct_Test_Date();
			void FeatureDimReduction(int Total_Of_Register);
			void LoadDatabase();
			void DatabaseConstrcutFeature();
			void PCA(int TypeImg,int Number_Of_Register);
			void ShowBlockImg(int numSelectBlock,ResultReg Result,vector<vector<int>> &highestM);
			void TestingProjecction();
			void DeleteBlackBlock(const IplImage* no_pohe_warp,double percentblock);
			//type = 1 is for profile face to record the result otherwise is for frontal face
			void RecordFaseTure(ResultReg Result,vector<double>&pre,vector<double>&recall,int type = 0,double str_th = 0,double end_th = 1,double th_gap = 0.025,bool debug = false);
			vector<double> ComputeSimilarity();
			IplImage* GetFeatureImg(int choose);//1.LBP 2.LGXP
			void iniHist();
			void AutoSimilarityBetween(int numSelectBlock,double threshold,
										   ResultReg &result,vector<vector<int>> &highestM);
	private:
			void GaborFilter(double theta,double f);
			void LBP();
			void LGXP();		
	private:
			Dim5	  m_Histogram;
			vector<vector<double>>m_Test_ProjectionSpace;
			vector<vector<double>>m_Register_ProjectionSpace;
			vector<double>PIE;
			vector<double>GaborMag;      // 1D array to hold the Gabor Magnitude Image
			vector<double>GaborPha;      // 1D array to hold the Gabor Phase Image
			vector <double>m_sumRate;
			vector<char> m_no_black_block;
			IplImage* Image_in;
			IplImage* ImgMag;        // 1D array to output Magnitude of Gabor Image
			IplImage* ImgPhase;      // 1D array to output Phase of Gabor Image
			IplImage* ImgLBP;
			IplImage* ImgLGXP;
			
			double    ReGaborfun[Gay][Gax]; 
			double    ImGaborfun[Gay][Gax];
			bool	  m_debug;
			int       m_StartM;
			int       m_AddM;
			int       m_SetM;
			int		  m_width;
			int       m_height;
			int		  m_endM;
			int       m_nameNum;
			int       m_numSample;
			int       m_numRegister;//總共人數
			int		  m_projection ;
			int		  m_numOfM;
			int		  m_numOfFeature;
			int		  His_div;
			int		  m_numblock_w;
			int		  m_numblock_h;
			
};
#endif