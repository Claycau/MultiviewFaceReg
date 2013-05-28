#include "Recognition.h"

char *Name_Concatenating	=	new char [50];

void NameinFile_1(int DoType)
{
	for(int i = 0;i < 50;i++)Name_Concatenating[i] = 0;

	if(DoType == MAG_TYPE)		
	sprintf(Name_Concatenating	, "..\\Database\\ConcatenatingHisMAG_total.xls");

	else if(DoType == PHASE_TYPE)		
	sprintf(Name_Concatenating	, "..\\Database\\ConcatenatingHisPHASE_total.xls");

	else if(DoType == Test_MAG_TYPE)		
	sprintf(Name_Concatenating	, "..\\Database\\testConcatenatingHisMAG_total.xls");

	else if(DoType == Test_PHASE_TYPE)		
	sprintf(Name_Concatenating	, "..\\Database\\testConcatenatingHisPHASE_total.xls");

	else if(DoType == PCA_TYPE)
	sprintf(Name_Concatenating	, "..\\Database\\testConcatenatingHisPHASE_total.xls");

}

Recognition::Recognition(const char* filename,int projection,int numWidthBlock,int numHeightBlock,int numSample,int numRegister
						,char* resultFile,double threshold,int FaceInf,bool debug)
			: RecognitionMethod(resultFile,numRegister,numSample,threshold)
{   
	Image_in	=	cvLoadImage(filename,1);
	m_height	=	Image_in->height;
	m_width		=	Image_in->width;

	ImgMag		=	cvCreateImage(cvSize(m_width,m_height),8,1);
	ImgPhase	=	cvCreateImage(cvSize(m_width,m_height),8,1);
	ImgLBP		=	cvCreateImage(cvSize(m_width,m_height),8,1);
	ImgLGXP		=	cvCreateImage(cvSize(m_width,m_height),8,1);
	PIE			=   vector<double>(m_height*m_width);           // 1D array to hold the processed Image
	GaborMag	=   vector<double>(m_height*m_width); 
	GaborPha	=   vector<double>(m_height*m_width); 
	m_sumRate   =   vector<double>(numSample*numRegister); 
	His_div     =		64;
	m_numOfFeature  =	40 * 4 * 256 / His_div;
	m_projection  =		projection;
	m_numblock_h  =		numHeightBlock;
	m_numblock_w  =		numWidthBlock;
	m_numOfM      =		numWidthBlock * numHeightBlock;
	m_numSample   =     numSample;
	m_numRegister =     numRegister;
	m_debug		  =		debug;
	m_endM		  =		m_numOfM;
	m_Histogram	  =		Dim5(2,Dim4(40,Dim3(m_numOfM,Dim2(4,Dim1(m_numOfFeature)))));
	m_no_black_block		= vector<char>(m_numOfM);
	m_Test_ProjectionSpace  = vector<vector<double>>(2*m_numOfM*m_numSample,m_projection);

	for(int i = 0;i < m_numOfM;i++) m_no_black_block[i] = 't';
	int buf;
	int length = strlen(filename);
	char *filenameOut = new char[length];
	strcpy(filenameOut,filename);
	
	cout << "gabor size = " << Gax << endl;
	//char trans to int
	//為了BLOCXSHOW裡，輸出儲存之名子
	m_nameNum = 0;
	for(int i = 0;i < length;i++)
	{
		if(i > 14 && i < length-4)
		{
			buf         = -48;
			buf		   += filenameOut[i];
			if((i-15) == 0)
			{
				m_nameNum  += buf*pow(10.,(i-15));
			}
			else
			{
				m_nameNum  *= 10;
				m_nameNum  += buf;
			}
		}
	}
}

Recognition::~Recognition()
{
	cvReleaseImage(&ImgLBP);
	cvReleaseImage(&ImgLGXP);
	cvReleaseImage(&Image_in);
	cvReleaseImage(&ImgMag);
	cvReleaseImage(&ImgPhase);
}
//GaborFilter()
//Generate ReGabor and ImGabor
void Recognition::GaborFilter(double theta,double f)
{
	double space_freq = (4.5*1.0)/(Gax*1.0) * f;
	double sigma = Gax*0.35;
	
	for(int j = -Gay/2;j <= Gay/2;j++)
	{
		for(int i = -Gax/2;i <= Gax/2;i++)
		{
			double theta_parameter = -i*cos(theta) + j*sin(theta);
			double gauss_parameter = -((i*i+j*j)/(2*sigma*sigma));
			int posx = i + Gradius;
			int posy = j + Gradius;
			ImGaborfun[posy][posx] = exp(gauss_parameter)*sin(2*PI*space_freq*theta_parameter); 
			ReGaborfun[posy][posx] = exp(gauss_parameter)*cos(2*PI*space_freq*theta_parameter);
		}
	}
}
//CreateGaborImg()
//To create the GaborMag and GaborPha
void Recognition::CreateGaborImg(int PhaScale,double FreqScale)
{
	vector<double>ImImg(m_height*m_width);
	double  ReValue , ImValue;
	double  maxM = -256,minM = 256,
		    maxP = -256,minP = 256;
	cvZero(ImgMag);
	cvZero(ImgPhase);
	GaborFilter( PhaScale , FreqScale );

	for(int j = 0;j < m_height;j++)
	{
		for(int i = 0;i < m_width;i++)
		{
			ReValue = 0.;
			ImValue = 0.;
			
			for(int y = -Gradius; y <= Gradius; y++)
			{
				for(int x = -Gradius; x <= Gradius; x++)
				{
					if((j+y) > -1 && (i+x) > -1  && (j+y) < m_height && (i+x) < m_width)
					{
						ReValue += PIE[(j+y)*m_width+i+x] * ReGaborfun[Gradius+y][Gradius+x];
						ImValue += PIE[(j+y)*m_width+i+x] * ImGaborfun[Gradius+y][Gradius+x];
					}
				}
			}
			GaborMag[j*m_width+i] = sqrt(ReValue*ReValue + ImValue*ImValue);
			GaborPha[j*m_width+i] = atan(ImValue / ReValue);

			maxM = (maxM < GaborMag[j*m_width+i]) ? GaborMag[j*m_width+i] : maxM;
			maxP = (maxP < GaborPha[j*m_width+i]) ? GaborPha[j*m_width+i] : maxP;
			minM = (minM > GaborMag[j*m_width+i]) ? GaborMag[j*m_width+i] : minM;
			minP = (minP > GaborPha[j*m_width+i]) ? GaborPha[j*m_width+i] : minP;
		}
	}

	double NewMag = 0. ,NewPha = 0.;

	for(int j = 0;j < m_height;j++)
	{
		for(int i = 0;i < m_width;i++)
		{
			NewMag = 256 * ( (GaborMag[j*m_width+i]-minM)/(maxM-minM) );
			NewPha = 359 * ( (GaborPha[j*m_width+i]-minP)/(maxP-minP) );
			GaborMag[j*m_width+i] = NewMag;
			GaborPha[j*m_width+i] = NewPha;
		}
	}

	for(int j = 0;j < m_height ;j++)
	{
		for(int i = 0;i < m_width;i++)
		{
			cvSetReal2D(ImgMag, j, i, GaborMag[j*m_width+i]);
			cvSetReal2D(ImgPhase, j, i, GaborPha[j*m_width+i]);
		}
	}
	if(m_debug)
	{
		cvNamedWindow("GaborMag",0);
		cvNamedWindow("GaborPhase",0);
		cvShowImage("GaborMag",ImgMag);
		cvShowImage("GaborPhase",ImgPhase);
	}
}
//LBP()
//To process magnitude image with its texture
void Recognition::LBP()
{
	int MidPoint	= 0;
	int LBP_Value	= 0;//binary transfer to decimal value.
	cvZero(ImgLBP);
	Mat test(m_height,m_width,1);
	for(int j = 1;j < m_height-1;j++)
	{
		for(int i = 1;i < m_width-1;i++)
		{
			LBP_Value  = 0;
			MidPoint   = 1.1*GaborMag[j*m_width +i];
			LBP_Value += (GaborMag[(j-1)*m_width + (i-1)] > MidPoint) ? 1:0;
			LBP_Value += (GaborMag[(j-1)*m_width + (i)]	  > MidPoint) ? 2:0;
			LBP_Value += (GaborMag[(j-1)*m_width + (i+1)] > MidPoint) ? 4:0;
			LBP_Value += (GaborMag[(j)*m_width + (i-1)]	  > MidPoint) ? 8:0;
			LBP_Value += (GaborMag[(j)*m_width + (i+1)]	  > MidPoint) ? 16:0;
			LBP_Value += (GaborMag[(j+1)*m_width + (i-1)] > MidPoint) ? 32:0;
			LBP_Value += (GaborMag[(j+1)*m_width + (i)]	  > MidPoint) ? 64:0;
			LBP_Value += (GaborMag[(j+1)*m_width + (i+1)] > MidPoint) ? 128:0;

			cvSetReal2D(ImgLBP,j,i,LBP_Value);

		}
	}
	if(m_debug)
	{
		cvNamedWindow("LBP",0);
		cvShowImage("LBP",ImgLBP);
		/*namedWindow("Lbp",0);
		imshow("Lbp",test);*/
	}
}
//LGXP()
//Local Gabor XOR Patterns
//To decrease the sensitivity of Gabor phase
void Recognition::LGXP()
{
	int		Number_of_Quantization = 4;
	int		phase_block			   = 360/Number_of_Quantization;
	int		MidPoint			   = 0;
	int		LGXP_Value = 0;//binary transfer to decimal value.

	cvZero(ImgLGXP);
	for(int j = 1;j < m_height-1;j++)
	{
		for(int i = 1;i < m_width-1;i++)
		{
			LGXP_Value  = 0;
			MidPoint	= GaborPha[j*m_width+i]/phase_block;
			LGXP_Value += int((GaborPha[(j-1)*m_width+(i-1)]/phase_block) != MidPoint) ? 1:0;
			LGXP_Value += (int(GaborPha[(j-1)*m_width+(i)]  /phase_block) != MidPoint) ? 2:0;
			LGXP_Value += (int(GaborPha[(j-1)*m_width+(i+1)]/phase_block) != MidPoint) ? 4:0;
			LGXP_Value += (int(GaborPha[(j)*m_width+(i-1)]  /phase_block) != MidPoint) ? 8:0;
			LGXP_Value += (int(GaborPha[(j)*m_width+(i+1)]  /phase_block) != MidPoint) ? 16:0;
			LGXP_Value += (int(GaborPha[(j+1)*m_width+(i-1)]/phase_block) != MidPoint) ? 32:0;
			LGXP_Value += (int(GaborPha[(j+1)*m_width+(i)]  /phase_block) != MidPoint) ? 64:0;
			LGXP_Value += (int(GaborPha[(j+1)*m_width+(i+1)]/phase_block) != MidPoint) ? 128:0;

			cvSetReal2D(ImgLGXP,j,i,LGXP_Value);
		}
	}
	if(m_debug)
	{
		cvNamedWindow("LGXP",0);
		cvShowImage("LGXP",ImgLGXP);
		cvWaitKey(0);
	}
	
}	
//CreateHistogram()
//scale : number of input image
// m_Histogram[2][40][16][4][4]:Save LBP and LGXP Image's features
void Recognition::CreateHistogram(IplImage* Img,int TypeImg,int scale)
{
	double Value  = 0;
	int Range_Y = m_height/m_numblock_h,
		Range_X = m_width /m_numblock_w;
	//int M = BIG_BLOCK_SIZE  ,K = 0;
	int K = 0;
	/***						START CalculateHistogram				****/
	for(int NUM_Y = 0;NUM_Y < m_numblock_h;NUM_Y++)
	{
		for(int NUM_X = 0;NUM_X < m_numblock_w;NUM_X++)
		{
			K = 0;
			//分四小塊
			for(int num_y = NUM_Y*2;num_y < NUM_Y*2+2;num_y++)
			{
				int Start_Block_Y = num_y*(Range_Y/2),
					End_Block_Y   = (num_y+1)*(Range_Y/2);
				
				for(int num_x = NUM_X*2;num_x < NUM_X*2+2;num_x++)
				{
					int	Start_Block_X = num_x*(Range_X/2),
						End_Block_X   = (num_x+1)*(Range_X/2);
				
					for(int j = Start_Block_Y; j < End_Block_Y ; j++)
					{
						for(int i = Start_Block_X ; i < End_Block_X   ; i++)
						{
							Value = cvGetReal2D(Img,j,i);
							int bar = Value/His_div;
							/*if(bar > 4) bar = 4;
							else if (bar < 0 )bar = 0;*/
							m_Histogram[TypeImg][scale][NUM_Y*m_numblock_w + NUM_X][K][bar]++;//
						}
					}
					K++;
				}
			}
		}
	}
	/***					END CalculateHistogram						***/
}
//Append to Excel in order to save all of register in database
//Create Features For Every One and Save All Of Them "In Order" in the Excel to Make Processing Easyier
void Recognition::ConcatenatingHistogram(int DoType)
{
	int NUM_BIG_BLOCK   = m_numOfM       ,
		NUM_SMALL_BLOCK = SMALL_BLOCK_SIZE * SMALL_BLOCK_SIZE   ,
		NUM_BIN         = 256 / His_div							; 

	NameinFile_1(DoType);//讀取寫入之位置
	FILE *fptr = NULL;
	fptr = fopen(Name_Concatenating,"a");//Append
	if(DoType == Test_MAG_TYPE)			DoType = MAG_TYPE;
	else if(DoType == Test_PHASE_TYPE)	DoType = PHASE_TYPE;

	if(fptr != NULL)
	{
		for(int M = 0;M < NUM_BIG_BLOCK;M++)//Big Block = 16
		{
			for(int Scale = 0;Scale < 40;Scale++)
			{
				for(int K = 0;K < NUM_SMALL_BLOCK;K++)//2*2//Small Block
				{
					for(int Bin = 0;Bin < NUM_BIN;Bin++)//4 Bins
					{
						fprintf(fptr,"%d\t",m_Histogram[DoType][Scale][M][K][Bin]);
					}
				}
			}

			fprintf(fptr,"\n");
		}
		fclose(fptr);
	}
}
//Write and Append(Build_in_PCA) to Excel
//PCA()
//Number_Of_Register = total of register
void Recognition::PCA(int TypeImg,int Number_Of_Register)
{
	int **Feature_Date = new int *[Number_Of_Register];
	for(int i = 0;i < Number_Of_Register;i++)
	{
		Feature_Date[i] = new int [m_numOfFeature*m_numOfM*m_numSample];
	}

	/***					Read Concatenating Histogram form Excel					***/
	NameinFile_1(TypeImg);

	FILE* Features = fopen(Name_Concatenating,"r");

	for(int j = 0; j < Number_Of_Register; j++)
	{
		for(int i = 0; i < m_numOfM*m_numOfFeature*m_numSample; i++) 
		{
			fscanf(Features, "%d", &Feature_Date[j][i]);
		}
	}

	fclose(Features);

	/*		為了PCA重新排列，排列方式依照夫哥程式規定排列			*/
	for(int k = 0; k < m_numOfM;k++)
	{
		printf("M_%d\n",k);
		NameinFile_1(PCA_TYPE);
		FILE* PCA = fopen(Name_Concatenating,"w");   //讀入訓練樣本,其排列方式 according to the PCA.h's manual

		for(int j = 0; j < Number_Of_Register; j++)
		{
			for(int s = 0;s < m_numSample;s++)
			{
				for(int i = 0; i < m_numOfFeature; i++) 
				{
					fprintf(PCA, "%d\t",Feature_Date[j][s*m_numOfFeature*m_numOfM+k*m_numOfFeature+i]);//(k*m_numSample+s)*m_numOfFeature
				}
				fprintf(PCA,"\n");
			}
		}
        fclose(PCA);

		CPCA	M1;
		M1.LoadMatrix(Name_Concatenating,m_numOfFeature,Number_Of_Register*m_numSample);
		M1.CalcMean();
		M1.CalcCoVarianceMatrix();
		M1.CalcEigenValueAndVector();
		M1.RearrangeEigenvalueAndVector();		
		M1.CalcProjectionSpace(m_projection);
		M1.ShowMatrixInf();
	}
}
//Register()
//To register man into the system
void Recognition::DatabaseConstrcutFeature()
{
	int scale = 0;
	/***	 讀取Register Image至double			***/
	IplImage *out_image = cvCreateImage(cvSize(m_width,m_height),8,1);
    cvCvtColor(Image_in,out_image,CV_BGR2GRAY);

	for(int j = 0;j < m_height ;j++)
	{
		for(int i = 0;i < m_width;i++)
		{
			PIE[j*m_width+i] = cvGetReal2D(out_image,j,i); 
		}
	}	
	iniHist();
	for(int PhaScale = 0;PhaScale < 360;PhaScale+=45)// 0 ,45 ,90 , 135 , 180, 225 , 270 , 315 
	{	
		//printf("PhaScale = %d\n",PhaScale);
		for(double FreqScale = freq_start;FreqScale <= freq_end;FreqScale += freq_add,scale++)
		{
			//printf("FreqScale = %f\n",FreqScale);
			CreateGaborImg(PhaScale,FreqScale);
			LBP();
			LGXP();
			CreateHistogram(ImgLBP , MAG_TYPE , scale);					//SAVE 40 SHEETS OF LBP  BLOCK_BASED FEATURES
			CreateHistogram(ImgLGXP,PHASE_TYPE,scale);					//SAVE 40 SHEETS OF LGXP BLOCK_BASED FEATURES
		}
	}			
	ConcatenatingHistogram(MAG_TYPE);
	ConcatenatingHistogram(PHASE_TYPE);
	cvReleaseImage(&out_image);
}

void Recognition::FeatureDimReduction(int Total_Of_Register)
{
		printf("PCA......\nMAG_TYPE \n");
		PCA(MAG_TYPE,Total_Of_Register);

		printf("PCA......\nPHASE_TYPE \n");
		PCA(PHASE_TYPE,Total_Of_Register);
}

void Recognition::Construct_Test_Date()
{
	/****** 讀取Test Image ******/
	IplImage *out_image = cvCreateImage(cvSize(m_width,m_height),8,1);
    cvCvtColor(Image_in,out_image,CV_BGR2GRAY);

	for(int j = 0;j < m_height ;j++)
	{
		for(int i = 0;i < m_width;i++)
		{
			PIE[j*m_width+i] = cvGetReal2D(out_image,j,i); 
		}
	}
	/***						 產生 Gabor Magnitude & Phase				***/

	int scale = 0;	//同等於計算輸入張數
	iniHist();
	//printf("Generating Feature.....\n");

	for(int PhaScale = 0;PhaScale < 360;PhaScale+=45)// 0 ,45 ,90 , 135 , 180, 225 , 270 , 315 , 360
	{	
		//printf("PhaScale = %d\n",PhaScale);
		for(double FreqScale = freq_start;FreqScale <= freq_end;FreqScale+=freq_add,scale++)
		{
			//printf("FreqScale = %f\n",FreqScale);
			CreateGaborImg(PhaScale,FreqScale);
			LBP();
			LGXP();
			
			/***	 降維256->4處理並且產生Block Base Histogram		***/
			CreateHistogram(ImgLBP,MAG_TYPE,scale);							//SAVE 40 SHEETS OF LBP BLOCK_BASED FEATURES
			CreateHistogram(ImgLGXP,PHASE_TYPE,scale);						//SAVE 40 SHEETS OF LGXP BLOCK_BASED FEATURES
		}
	}

	/***					 一次處理40張LBP(Mag) histogram and  LGXP(Phase) histogram				***/

	/*		 以Block Base 加上Concatenate		*/
	ConcatenatingHistogram(Test_MAG_TYPE);
	ConcatenatingHistogram(Test_PHASE_TYPE);

	/*		投影		*/
	//printf("Projection.....\n");
	TestingProjecction();
	cvReleaseImage(&out_image);
}

void Recognition::TestingProjecction()
{
	int NumOfProjectionPlanes = m_projection;

	/***		讀取Testing Picture's LGXP&LBP特徵		***/

	vector<vector<int>> Testdata = vector<vector<int>>(2*m_numOfM,m_numOfFeature);

	NameinFile_1(Test_MAG_TYPE);
	FILE * MAG;
	MAG = fopen(Name_Concatenating,"r");

	NameinFile_1(Test_PHASE_TYPE);
	FILE * PHASE;
	PHASE = fopen(Name_Concatenating,"r");

	for(int j = 0;j < m_numOfM; j++)
	{
		for(int i = 0;i < m_numOfFeature;i++)
		{
			fscanf(MAG,"%d",&Testdata[j][i]);
			fscanf(PHASE,"%d",&Testdata[j+m_numOfM][i]);
		}
	}

	fclose(MAG);
	fclose(PHASE);
	/****								讀取資料庫											****/

	/*			讀取資料庫的Mean		*/

	vector<vector<double>> m_Mean = vector<vector<double>>(2*m_numOfM,m_numOfFeature);

	FILE *fm_Mean = fopen("..\\Database\\m_Mean.xls","r");
	for(int i = 0; i < (m_numOfM*2); i++)
	{
		for(int j = 0; j < m_numOfFeature; j++)
		{
			fscanf(fm_Mean, "%lf", &m_Mean[i][j]);
		}
	}
	fclose(fm_Mean);

	/*							 讀取m_EigenSpace						*/

	vector<vector<double>>m_EigenSpace = vector<vector<double>> (m_numOfFeature*m_numOfM*2,NumOfProjectionPlanes);

	FILE *fm_EigenSpace = fopen("..\\Database\\m_EigenSpace.xls","r");
	for(int i = 0; i < (m_numOfFeature*m_numOfM*2); i++)
	{
		for(int j = 0; j < NumOfProjectionPlanes; j++) 
		{
			fscanf(fm_EigenSpace, "%lf", &m_EigenSpace[i][j]);
		}
		fscanf(fm_EigenSpace, "\t");
	} 
	fclose(fm_EigenSpace);

	/***								宣告投影陣列							****/

	vector<vector<double>> Intermediate = vector<vector<double>>(m_numOfM*2,m_numOfFeature);

	/***								投影m_projection									***/
	for(int i = 0; i < m_numOfM*2; i++)
	{
		for(int k = 0; k < m_numOfFeature; k++)
		{
			Intermediate[i][k] = 0;
			Intermediate[i][k] = Testdata[i][k] - m_Mean[i][k];
		}
		
		for(int k = 0; k < (m_numOfFeature<NumOfProjectionPlanes?m_numOfFeature:NumOfProjectionPlanes); k++)
		{
			m_Test_ProjectionSpace[i][k] = 0;

			for(int l = 0; l < m_numOfFeature; l++)
			{                   
				m_Test_ProjectionSpace[i][k]+=Intermediate[i][l]*m_EigenSpace[l+(m_numOfFeature*i)][k];
			}
		}
	}

	/***								寫出投影結果									***/
	FILE *fProjectionSpace;
	fProjectionSpace = fopen("..\\Database\\TestProjectionSpace.xls","w");
	for(int i = 0; i < (m_numOfM*2); i++){
		for(int j = 0; j < NumOfProjectionPlanes; j++) fprintf(fProjectionSpace, "%lf\t", m_Test_ProjectionSpace[i][j]);
		fprintf(fProjectionSpace, "\n");
	}
	fclose(fProjectionSpace);

}

void Recognition::LoadDatabase()
{
	/*******							讀取資料庫資料						*******/
	//printf("Database Loading.....\n");
	m_Register_ProjectionSpace = vector<vector<double>>(2*m_numOfM*m_numSample*m_numRegister,m_projection);
	FILE *Register_Projection = fopen("..\\Database\\m_ProjectionSpace.xls","r");
											
	for(int j = 0 ;j < (m_numSample*m_numOfM*2*m_numRegister);j++)
	{
		for(int i = 0;i < m_projection;i++)
		{
			fscanf(Register_Projection, "%lf",&m_Register_ProjectionSpace[j][i]);
		}
	}
	fclose(Register_Projection);
}

void Recognition::ShowBlockImg(int numSelectBlock,ResultReg Result,vector<vector<int>> &highestM)
{
	char chPanorammic[50];
	char BlockTest[30],BlockRegister[50];

	int rangeY  = m_height/m_numblock_h,//每個BIG BLOCK的寬度
		rangeX  = m_width /m_numblock_w;//每個BIG BLOCK的長度
	
	for(int s = 0;s < m_numSample;s++)
	{	
		sprintf(chPanorammic,"..\\PanormaicFace\\SmallStitch\\panoramic0_%d.bmp",Result.person);
		IplImage *srcCopy = cvCreateImage(cvSize(m_width,m_height),8,1);
		cvCvtColor(Image_in,srcCopy,CV_RGB2GRAY );
		IplImage *panoramic = cvLoadImage(chPanorammic,0);
		int count_select = 0;
		for(int m = 0;m < m_endM;m++)
		{
			if(m_no_black_block[highestM[s][m]] = 't')
			{
				int ingM   = highestM[s][m];	//現在處於得block
				int ingY = ingM/m_numblock_w;
				int ingX = ingM%m_numblock_w;
				int startX	= ingX*rangeX;
				int	endX	= rangeX + startX;
				int	startY	= ingY*rangeY;
				int	endY	= rangeY + startY;
				//取圖像的範圍，並且在範圍的最外框，用白色畫上
				for(int j = startY;j < endY && j < m_height;j++)
				{
					for(int i = startX;i < endX && i < m_width;i++)
					{
						if(j == startY || j == (endY-1))
						{
							cvSetReal2D(srcCopy,j,i,255 - m*(256/numSelectBlock)) ;
							cvSetReal2D(panoramic,j,i,255 - m*(256/numSelectBlock)) ;
						}
						else
						{
							if(i == startX || i== (endX-1))
							{
								cvSetReal2D(srcCopy,j,i,255 - m*(256/numSelectBlock)) ;
								cvSetReal2D(panoramic,j,i,255 - m*(256/numSelectBlock)) ;
							}
						}
					}
				}
				
				if(count_select++ == numSelectBlock-1)break;
			}
		}
		sprintf(BlockTest,"..\\Recognition\\block\\%d_%d.bmp",m_nameNum,s);
		sprintf(BlockRegister,"..\\Recognition\\block\\%d_%dp.bmp",m_nameNum,s);
						/*cvNamedWindow("TestImage",0);
		cvShowImage("TestImage",srcCopy);
		cvNamedWindow("Panoramic Image",0);
		cvShowImage("Panoramic Image",panoramic);
		cvWaitKey(0);*/
		cvSaveImage(BlockTest,srcCopy);
		cvSaveImage(BlockRegister,panoramic);
	
		cvReleaseImage(&srcCopy);
		cvReleaseImage(&panoramic);
	}
	
}

vector<double> Recognition::ComputeSimilarity()
{

	FILE *fdle = fopen("..\\Database\\Similarity_result.txt","a");
	FILE *TOTAL = fopen("..\\Database\\Similarity_Total_result.txt","a");
	FILE *TESTFILE = fopen("..\\Database\\Similarity_Block.txt","w");
	int startM = m_StartM,endM = m_endM, totalM = 0,
		W = m_SetM;
	if(m_debug)
	{
		startM = 0,endM = m_endM,W = 0,m_AddM = 1,m_SetM = 1;
	}
	double Avg = 0.;
	double *SumRate = new double [m_numRegister];
	double Weight = 0.5;

	for(int i = 0;i < m_numRegister;i++)
	{
		SumRate[i] = 0.;
		if(m_debug)
		{
			fprintf(TESTFILE,"%d\n",i);
		}
		for(int s = 0;s < m_numSample;s++)//
		{
			if(m_debug)
			{
				fprintf(TESTFILE,"S:%d\n",s);
			}
			double D_X = 0,D_B = 0, S_X = 0, S_B = 0;
				   Avg   = 0;
				   totalM = 0;
			for(int j = startM; j < endM; j+=pow(m_AddM*1.0,W),totalM++)
			{
				double dot_X = 0, dot_B = 0, D_X1 = 0, D_X2 = 0,  D_B1 = 0, D_B2 = 0;
				if(m_SetM == 0)
				{
					W++;
					if(W == 2)
					{
						W = 0;
					}
				}
				//num++;
				for(int k = 0; k < m_projection; k++)
				{
					//PHASE:LGXP
					int A =  s+m_numRegister*m_numSample*j+(m_numOfM*m_numSample*m_numRegister) + i*m_numSample,
						B =  j+m_numOfM;
					//Magnitude:LBP
					int C =  s+m_numRegister*m_numSample*j+ i*m_numSample , 
						D =  j;
					
					dot_X += m_Register_ProjectionSpace[A][k] * m_Test_ProjectionSpace[B][k];
					dot_B += m_Register_ProjectionSpace[C][k] * m_Test_ProjectionSpace[D][k];
					D_X1  += m_Register_ProjectionSpace[A][k] * m_Register_ProjectionSpace[A][k];
					D_B1  += m_Register_ProjectionSpace[C][k] * m_Register_ProjectionSpace[C][k];
					D_X2  +=     m_Test_ProjectionSpace[B][k] * m_Test_ProjectionSpace[B][k];
					D_B2  +=     m_Test_ProjectionSpace[D][k] * m_Test_ProjectionSpace[D][k];
				}
					D_X = sqrt(D_X1) * sqrt(D_X2);
					D_B = sqrt(D_B1) * sqrt(D_B2);
					S_X = (dot_X / D_X);
					S_B = (dot_B / D_B);
					Avg += (Weight * S_B + (1.0-Weight) * S_X);
					if(m_debug)
					{
						fprintf(TESTFILE,"%d: %f , %f\n",j,S_X,S_B);
					}
					//printf("SB = %f SX = %f Avg = %f\n",S_B,S_X,Avg);
			}

			Avg /= (totalM*1.0);//減少運算
			SumRate[i] += Avg;//取所有SAMPLE的平均值去做辨識
			m_sumRate[m_numSample*i+s] = Avg;//<-取每一個SAMPLE的辨識值儲存起來，用於K-TREE
			
			fprintf(fdle,"%f\n",m_sumRate[m_numSample*i+s]);
			fprintf(TOTAL,"%f\n",m_sumRate[m_numSample*i+s]);
		}
		SumRate[i]/=(m_numSample*1.0);
		//fprintf(fdle,"%d: %f\n",i,SumRate[i]);
	}
	fclose(fdle);
	fclose(TOTAL);
	fclose(TESTFILE);
	SelectBig(m_projection);
	if(m_debug)
	system("pause");
	return m_sumRate;
}

void Recognition::AutoSimilarityBetween(int numSelectBlock,double threshold,
										ResultReg &result,vector<vector<int>> &highestM)
{
	if(numSelectBlock <= m_endM)
	{
		FILE *Blockprint = fopen("..\\Database\\Debug_Similarity_Block.txt","w");
	
		int startM = 0,endM = m_endM, totalM = 0;
		double Weight = 0.5;
		ResultReg Result;
		vector<vector<double>> personRegRate(m_numSample,endM);
		vector<double> sumRegRate(m_numRegister);
		vector<double> regResult(m_numRegister);
		for(int i = 0;i < m_numRegister;i++)
		{
			for(int s = 0;s < m_numSample;s++)//
			{
				double D_X = 0,D_B = 0, S_X = 0, S_B = 0;
				//fprintf(Blockprint,"Person,Sample = %d,%d\n",i,s);
				for(int m = startM; m < endM; m++)
				{
					personRegRate[s][m] = 0;
				}
				for(int j = startM; j < endM; j++,totalM++)
				{
					if(m_no_black_block[j] == 't')
					{
						double dot_X = 0, dot_B = 0, D_X1 = 0, D_X2 = 0,  D_B1 = 0, D_B2 = 0;

						for(int k = 0; k < m_projection; k++)
						{
							//PHASE:LGXP
							int person_phs =  s+m_numRegister*m_numSample*j+(m_numOfM*m_numSample*m_numRegister) + i*m_numSample,
								block_phs  =  j+m_numOfM;
							//Magnitude:LBP
							int person_mag =  s+m_numRegister*m_numSample*j+ i*m_numSample , 
								block_mag  =  j;

							dot_X += m_Register_ProjectionSpace[person_phs][k] * m_Test_ProjectionSpace[block_phs][k];
							dot_B += m_Register_ProjectionSpace[person_mag][k] * m_Test_ProjectionSpace[block_mag][k];
							D_X1  += m_Register_ProjectionSpace[person_phs][k] * m_Register_ProjectionSpace[person_phs][k];
							D_B1  += m_Register_ProjectionSpace[person_mag][k] * m_Register_ProjectionSpace[person_mag][k];
							D_X2  +=     m_Test_ProjectionSpace[block_phs][k]  * m_Test_ProjectionSpace[block_phs][k];
							D_B2  +=     m_Test_ProjectionSpace[block_mag][k]  * m_Test_ProjectionSpace[block_mag][k];
						}

						D_X = sqrt(D_X1) * sqrt(D_X2);
						D_B = sqrt(D_B1) * sqrt(D_B2);
						S_X = (dot_X / D_X);
						S_B = (dot_B / D_B);
						if(m_debug && (i == 0 || i == 1))
						{
							cout << "S_X = "<< S_X <<"\tS_B = "<< S_B << endl;
						}
						personRegRate[s][j] = Weight * S_B + (1.0-Weight) * S_X;;
						if(m_debug )
						{
							fprintf(Blockprint,"%d  %f\n",j,personRegRate[s][j]);
						}
					}
				}	
			}
			//Sort
			vector<vector<int>>temp_M(m_numSample,m_numOfM);
			for(int s = 0;s < m_numSample; s++)
			{
				for(int j = startM;j < endM;j++)
				{		
					temp_M[s][j] = j;
				}
				for(int j = startM;j < endM;j++)
				{		
					for(int m = j+1;m < m_endM;m++)
					{
						if(personRegRate[s][j] < personRegRate[s][m])
						{
							double buf = personRegRate[s][m];
							personRegRate[s][m] = personRegRate[s][j];
							personRegRate[s][j]   = buf;
							buf	= temp_M[s][j];
							temp_M[s][j] = temp_M[s][m];
							temp_M[s][m] = buf;
						}
					}
				}
			}
			//Compute reg rate
			//highestM: hightestM[s][0]:紀錄分數最高的block代碼
			//			hightestM[s][1]:紀錄分數第二高的block代碼
			bool save = false;
			for(int s = 0;s < m_numSample;s++)
			{
				int numGetBlock = 0;
				for(int block = 0;block < m_numOfM;block++)
				{
					if(personRegRate[s][block] != 0)
					{
						numGetBlock++;
						regResult[i] += (personRegRate[s][block]);
						if(numGetBlock == numSelectBlock) break;
					}
				}
			}
			regResult[i] /= (numSelectBlock * m_numSample);

			if(regResult[i] > Result.regRate)
			{		
				Result.regRate = regResult[i];
				Result.person  = i;
				for(int s = 0; s < m_numSample;s++)
				{
					for(int m = 0; m < endM;m++)
					{
						highestM[s][m] = temp_M[s][m];
					}
				}
			}
			if(m_debug)
			{
				cout << "person = "<< i <<"  "<< regResult[i] << endl;
			}
		}
	
		if(Result.regRate > threshold)
		{
			cout << "result = " << Result.person <<" rate = "<< Result.regRate << "\n";
		}
		result.person  = Result.person;
		result.regRate = Result.regRate;
		fclose(Blockprint);
	}
	else
	{
		cout << "ERROR,U SELECT NUMBER OF BLOCK SO MANY\n";
	}
}

void Recognition::DeleteBlackBlock(const IplImage* no_pohe_warp,double percentblock)
{
	int block_width = m_width / m_numblock_w;
	int block_height = m_height / m_numblock_h;
	int block_total_point = block_width *block_height;
	int startM = 0,endM = m_endM;

	for(int m = startM;m < endM;m++)
	{
		int block_start_x = (m%m_numblock_w)*block_width;
		int block_start_y = (m/m_numblock_w)*block_height;
		int block_end_x   = ((m%m_numblock_w)+1)*block_width;
		int block_end_y	  = ((m/m_numblock_w)+1)*block_height;
		int num_black_point = 0;

		int total = 0;
		
		for(int j = block_start_y;j < block_end_y;j++)
		{
			for(int i = block_start_x;i < block_end_x;i++)
			{
				total += cvGetReal2D(no_pohe_warp,j,i);
				if(cvGetReal2D(no_pohe_warp,j,i) == 0)
				{
					num_black_point++;
				}
			}
		}

		double percent_black_block = static_cast<double>( num_black_point) / block_total_point ;
		double avg			       = static_cast<double> (total)/block_total_point;
		int threshold			   = 65;
		if(percent_black_block > percentblock || avg < threshold)
		{
			m_no_black_block[m] = 'f';
		}
		else
		{
			m_no_black_block[m] = 't';
		}
	}
}

void Recognition::RecordFaseTure(ResultReg Result, vector<double>&pre, vector<double>&recall,
								 int type,double str_th,double end_th,double th_gap,bool debug)
{
	string filename =			"..\\Database\\t_record_result.txt";
	string resultfilename =		"..\\Database\\m_record_result.xls";
	if(type == 1)
	{
		filename =			"..\\Database\\t_pro_record_result.txt";
		resultfilename =	"..\\Database\\m_pro_record_result.xls";
	}
	string s[3];
	fstream fr;

	fr.open(filename,ios::in);
	fr.seekg(0,ios::end);
	int filesize = fr.tellg();
	fr.seekg(0,ios::beg);

	int total_num_var_th = (end_th - str_th)/th_gap + 0.5 + 1;
	vector<int>get_result(4*total_num_var_th);

	if(filesize)
	{
		for(int rows = 0 ;rows < 4*total_num_var_th;rows++)
		{
			if(rows % 4 == 0)fr >> s[0];
			fr >> s[0] >> s[1] >> s[2];
			stringstream(s[2]) >> get_result[rows];
		}
		fr.close();
		fr.open(filename,ios::out);
	}
	fr.close();

	for(double threshold = str_th,index = 0;threshold < end_th + th_gap;threshold += th_gap,index +=4)
	{
		if(Result.regRate > threshold)
		{
			//if test data belongs to database
			if(m_nameNum  < m_numRegister)
			{
				if(Result.person == m_nameNum)
				{
					get_result[2 + index]++;//correctly identified
				}
				else
				{
					get_result[0 + index]++;//incorrectly identified
				}
			}
			//else not belong to
			else
			{
				get_result[0 + index]++;
			}
		}

		else
		{
			if(m_nameNum  < m_numRegister)
			{
				get_result[1 + index]++;//incorrectly rejected
			}
			else
			{
				get_result[3 + index]++;//correctly rejected
			}
		}

		//double pre = 0.;
		if((get_result[2 + index] + get_result[0 + index]) != 0)
		{
			pre[index/4] = (get_result[2 + index]*1.0/((get_result[2 + index] + get_result[0 + index])*1.0))*100;
		}
		//double recall = 0.;
		if((get_result[2 + index] + get_result[1 + index]) != 0)
		{
			recall[index/4] = (get_result[2 + index]*1.0/((get_result[2 + index] + get_result[1 + index])*1.0))*100;
		}		

		if(debug)
		{
			cout << "fp = "		<< get_result[0]   << "\n";
			cout << "fn = "		<< get_result[1]   << "\n";
			cout << "tp = "		<< get_result[2]   << "\n";
			cout << "tn = "		<< get_result[3]   << "\n";
			cout << "pre = "	<< pre[index/4]	   << "\n";
			cout << "recall = "	<< recall[index/4] << "\n";
		}

		ofstream fp;
		fp.open(filename,ios::app);
		fp <<threshold<< "\n";
		fp << "fp = " << get_result[0 + index]  << "\t\t\n";
		fp << "fn = " << get_result[1 + index]  << "\t\t\n";
		fp << "tp = " << get_result[2 + index]  << "\t\t\n";
		fp << "tn = " << get_result[3 + index]  << "\t\t\n";
		fp.close();

		if((threshold <=0.35 && threshold > 0) || (threshold >= 0.75 && threshold < 1)) continue;
		cout <<"thrd = " << threshold << "\tpre = " << pre[index/4] <<"\t recall = " << recall[index/4] << endl;
	}
	cout << "==============================================================================\n";
}

IplImage* Recognition::GetFeatureImg(int choose)
{
	if(choose)
	return ImgLBP;
	else
	return ImgLGXP;
}

void Recognition::iniHist()
{
		/*Initialize zero for all Histogram value*/
	for(int Type = 0;Type < 2 ;Type++)
	{
		for(int Scale = 0;Scale < 40; Scale++ )
		{
			for(int M = 0; M < m_numOfM; M++)
			{
				for(int K = 0; K < 4; K++)
				{
					for(int Bin = 0; Bin < (256 / His_div); Bin++)
					{
						m_Histogram[Type][Scale][M][K][Bin] = 0;
					}
				}
			}
		}
	}
}