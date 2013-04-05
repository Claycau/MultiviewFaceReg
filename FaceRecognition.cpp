#include "FaceRecognition.h"
#include "ImageName.h"

FaceRecognition::FaceRecognition(bool debug)
{
	if(debug)
	{
		m_debug = true;
		int makeDecision = -1;
		int *buf = new int [6];
		FILE *ReadFile = NULL;
		m_threshold = 0.4;

		ReadFile = fopen("..\\Database\\Setting.txt","r");
		//讀取設定檔，看是否存在
		if(ReadFile!=NULL)
		{
			while(true)
			{
				cout<<("有過去設定檔，是否使用過去設定Yes or No(1,0)?");
				cin >>makeDecision;
				if(makeDecision == false || makeDecision == true)
					break;
			}

			//讀資料庫資料
			if(makeDecision)
			{
				for(int i = 0;i < 6;i++)
				{
					fscanf(ReadFile,"%d",&buf[i]);
				}
				m_numRegister  = buf[0];
				m_numSample    = buf[1];
				m_pcaDimension = buf[2];
				m_width    = buf[3];
				m_height   = buf[4];
				m_numBlock = buf[5];
			}
			fclose(ReadFile);
		}
		//設定檔並無建立，必須建立 
		//或 要重新設定
		if(ReadFile == NULL || makeDecision == false)
		{
			//刪除或建立空資料檔
			FILE *WriteFile = fopen("../Database/Setting.txt","w");
			fclose(WriteFile);

			cout<<"請按照順序輸入:\n1.資料庫人數\n2.樣本數\n3.PCA維度\n4.縮小後寬度\n5.縮小後長度\n6.Block數目(2的倍數)\n";
			cin >>m_numRegister>>m_numSample>>m_pcaDimension>>m_width>>m_height>>m_numBlock;
			while(true)
			{
				printf("輸入分別為:人數 = %d\n樣本數 = %d\n維度 = %d\n縮小後寬度 = %d\n縮小後長度 = %d\nBlock數:%d\n是否正確(1(yes),0(no))",
					m_numRegister,
					m_numSample,
					m_pcaDimension,
					m_width,
					m_height,
					m_numBlock);

				cin>>makeDecision;

				if(makeDecision==1)//輸入正確，儲存檔案並離開
				{
					WriteFile = fopen("../Database/Setting.txt","w");
					fprintf(WriteFile,
						"%d\t%d\t%d\t%d\t%d\t%d",
						m_numRegister,
						m_numSample,
						m_pcaDimension,
						m_width,
						m_height,
						m_numBlock);
					fclose(WriteFile);
					break;
				}
				else
				{
					cout<<"請按照順序輸入:\n1.資料庫人數\n2.樣本數 = %d\n3.PCA維度\n4.縮小後寬度\n5.縮小後長度\n6.Block數目(2的倍數)\n";
					cin >>m_numRegister>>m_numSample>>m_pcaDimension>>m_width>>m_height>>m_numBlock;
				}
			}
		}

		m_PanoramicFace  = NULL;
		m_TestColorImage = NULL;
		m_normalizePanoramicFace = NULL;

		FILE *fdle = fopen("..\\DataBase\\record_result.txt","w");
		fclose(fdle);

		/*
		fdle = fopen("..\\DataBase\\method_reulst.txt","w");
		fclose(fdle);
		FILE *fdle = fopen("..\\DataBase\\Similarity_Total_result.txt","w");
		fclose(fdle);
		*/
		delete [] buf;
	}
	else
	{
		m_debug = false;
	}
}

void FaceRecognition::InitSetting(const char* filename,bool debug)
{
	fstream fr;
	fr.open(filename,ios::end|ios::in);
	fr.seekg(0,ios::end);
	int filesize = fr.tellg();

	if(filesize != 0)
	{
		fr.seekg(0,ios::beg);
		fr >> m_numRegister >> m_numSample >> m_numBlock 
		   >> m_width       >> m_height	   >> m_pcaDimension   >> m_threshold >> m_numSelectBlock;

		if(debug)
		{
			cout  <<"m_numRegister\t= "		<< m_numRegister	<< endl;
			cout  <<"m_numSample\t= "		<< m_numSample		<< endl;
			cout  <<"m_numBlock\t= "		<< m_numBlock		<< endl;
			cout  <<"m_width\t\t= "			<< m_width			<< endl;
			cout  <<"m_height\t= "			<< m_height			<< endl;
			cout  <<"m_pcaDimension\t= "	<< m_pcaDimension	<< endl;
			cout  <<"m_threshold\t= "		<< m_threshold		<< endl;
			cout  <<"m_numSelectBlock= "	<< m_numSelectBlock << endl;
		}
	}
	else
	{
		cout << "file doesn't have data\n";
	}

	fr.close();

	if(debug)
	{
		system("pause");
	}
}

bool FaceRecognition::InitRegister()
{
	FILE *fdle = NULL;
	fdle = fopen("..\\Database\\Coordinate.xls","w");
	fclose(fdle);
	fdle = NULL;

	fdle = fopen("..\\Database\\ConcatenatingHisMAG_total.xls","w");
	fclose(fdle);
	fdle = NULL;

	fdle = fopen("..\\Database\\ConcatenatingHisPHASE_total.xls","w");
	fclose(fdle);
	fdle = NULL;

	fdle = fopen("..\\Database\\m_ProjectionSpace.xls","w");
	fclose(fdle);
	fdle = NULL;

	fdle = fopen("..\\Database\\m_EigenSpace.xls","w");
	fclose(fdle);
	fdle = NULL;

	fdle = fopen("..\\Database\\m_Mean.xls","w");
	fclose(fdle);
	fdle = NULL;

	fdle = fopen("..\\DataBase\\NormalCoord.xls","w");
	fclose(fdle);
	fdle = NULL;
	printf("Finish resetting File\n");
	return true;
}

void FaceRecognition::StartRegistration(bool debug)
{
	int buf[1] = {};
	fstream fp("..\\Database\\clock.txt");
	if(InitRegister())
	{
		//Step1:縫合人臉
		if(m_debug)
		{
			printf("panoramic?y/n 1/0\n");
			scanf("%d",buf);
			if(buf[0] == 1)
			{
				clock_t time_used_cpu = clock();
				ConstructPanoramicFace();
				time_used_cpu = clock() - time_used_cpu;
				fp << "Total panoramic construction = " << time_used_cpu << endl;
			}
		}
		else
		{
			clock_t time_used_cpu = clock();
			ConstructPanoramicFace(debug);
			time_used_cpu = clock() - time_used_cpu;
			fp << "Total panoramic construction = " << time_used_cpu << endl;
		}
		//Step2:建造特徵點
		clock_t time_used_cpu = clock();
		ConstructFeature();
		time_used_cpu = clock() - time_used_cpu;
		fp << "Total ConstructFeature" << time_used_cpu << endl;
		fp.close();
	}
	cvReleaseImage(&m_normalizePanoramicFace);
	cvReleaseImage(&m_PanoramicFace);
}
/*
*ConstrcutPanoramicFace is to construct panoramic face which
* is composed of three different phase face picture
*/
void FaceRecognition::ConstructPanoramicFace(bool debug)
{	
	cout << "Panoramic construction.....\n";
	Coordinate centerCood[12];
	String    cascade_name[3];
	int totalIndex = m_numRegister*3;
	cascade_name[0] = "haarcascade_frontalface_default.xml";//C:\\OpenCV-2.4\\opencv\\data\\haarcascades\\haarcascade_frontalface_default.xml

	for(int person = 1;person < totalIndex;person+=3)
	{
		for(int sample = 0; sample < m_numSample;sample++)
		{
			printf("<Sample,Person> <%d,%d>\n",sample,person/3);

			FileNamed fname;
			fname.NameinFile(sample,person,STITCH_TYPE);
			Panoramic Do(fname.StitchName_Left.c_str(),
						 fname.StitchName_Center.c_str(),
						 fname.StitchName_Right.c_str(),debug);
			
			m_PanoramicFace = Do.StitchFace(fname.FeatureName_Left.c_str(),
											fname.FeatureName_Center.c_str(),
											fname.FeatureName_Right.c_str());			
			Do.GetCenterFaceCoord(centerCood);/*Getting panoramic face and center face's feature coordinates*/
			
			m_normalizePanoramicFace = Procrustes(m_PanoramicFace,centerCood);
			
			if(false)
			{
				IplImage *enhencePanoramiceFace;
				enhencePanoramiceFace = Enhence(m_normalizePanoramicFace);
				cvSaveImage(fname.ResizeName.c_str(),enhencePanoramiceFace);
				cvReleaseImage(&enhencePanoramiceFace);
			}
			////cvSaveImage(FileName_Out,m_PanoramicFace);
			if(true)//not enhence
			{
				cvSaveImage(fname.ResizeName.c_str(),m_normalizePanoramicFace);
			}
			cvReleaseImage(&m_PanoramicFace);
			cvReleaseImage(&m_normalizePanoramicFace);
		}
	}
}

IplImage* FaceRecognition::Enhence(IplImage* src)
{
	Mat	m_dst,m_dst2;
	Mat	m_src  = Mat(src);
	Mat m_src2 = m_src.clone();
	
	POHE::enhance(m_src2,m_dst,49,49);	// two parameters the block sizes of height and width are allowed
	IplImage* dst = cvCloneImage(&(IplImage)m_dst);
	return dst;
}

void FaceRecognition::Enhence(string filename)
{
	Mat src	=	imread(filename,CV_LOAD_IMAGE_GRAYSCALE);
	Mat	m_dst;
	POHE::enhance(src,m_dst,49,49);	// two parameters the block sizes of height and width are allowed
	imwrite(filename,m_dst);
}
/*
*ConstructFeature() is to construct forty dimensions of gabor of test and training data
*/
void FaceRecognition::ConstructFeature()
{
	printf("Generating Feature.....\n");

	m_resultFile  = "..\\DataBase\\result2.txt";
	int totalIndex = m_numRegister*3;

	/*clock_t time_used_cpu = clock();*/
	for(int person = 1;person < totalIndex;person+=3)	
	{
		cout << "person = " << person/3 << endl;
		for(int sample = 0; sample < m_numSample;sample++)
		{
			FileNamed fname;
			fname.NameinFile(sample,person,REGISTER);//讀取位置REGISTER

			Recognition Reg(fname.ResizeName.c_str(),m_pcaDimension,m_numBlock,
							m_numSample,m_numRegister,m_resultFile,
							0.5,m_FaceInf,false);//NUMBLOCK:16
			Reg.DatabaseConstrcutFeature();
			
			/*	When features is constructed,then using pca to get lower-dimension features	*/
			if((sample == (m_numSample-1)) && (person == totalIndex-2))
			{
				fstream fp("..\\Database\\clock.txt");
				clock_t time_used_pca = clock();

				Reg.FeatureDimReduction(m_numRegister);

				time_used_pca = clock() - time_used_pca;
				fp << "PCA = " << time_used_pca << endl;
				fp.close();
			}
		}
	}
}

void FaceRecognition::StartRecognition(int index,bool debug)
{
	int  IndexPicture	= index;
	int  person         = (IndexPicture-1)/3;
	bool trueLoad		= false;
	Coordinate centerCoord[12];
	int testMode = 1;

	if(InitRecog(IndexPicture))
	{
		cout << "\nperson = " << person << endl;
		/*								正規畫						*/
		for(int i = 0;i < 12;i++)
		{
			centerCoord[i] = m_testCoord[i];
		}

		//SetWarp();
		IplImage* normal_test = Procrustes(m_TestImage,centerCoord,testMode);

		save_warp(normal_test,person);
		cvSaveImage("../Database/normal.jpg",normal_test);
		cvReleaseImage(&normal_test);
		NormalizeTestImg(person);

		ostringstream int_to_string;
		int_to_string << person;
		string chRegImg,BMP = ".bmp";;
		chRegImg = "../Recognition/" + int_to_string.str() + BMP;
		/*if(true)
		{
			Enhence(chRegImg);
		}*/
		Recognition recognition(chRegImg.c_str(),m_pcaDimension,m_numBlock,
								m_numSample,m_numRegister,m_resultFile,m_threshold,m_FaceInf,debug);	
		recognition.LoadDatabase();
		recognition.Construct_Test_Date();
		//if(SetWarp())
		//{
			recognition.DeleteBlackBlock(0.4);
		//}
		ResultReg resultget;
		vector<vector<int>>highestM(m_numSample,m_numBlock);
		recognition.AutoSimilarityBetween(m_numSelectBlock,m_threshold,resultget,highestM);
		recognition.ShowBlockImg(m_numSelectBlock,resultget,highestM);
		recognition.RecordFaseTure(resultget,m_threshold,m_debug);
		cvReleaseImage(&m_TestImage);
		cvReleaseImage(&m_TestColorImage);
	}
	else
	{
		printf("Error Loading....... ");
	}
}
/*
*SetWarp if face is profiled return true
*else return false
*/
bool FaceRecognition::SetWarp()
{
	Coordinate adjust_affLeft[3];	
	Coordinate adjust_affRight[3];
	//獲取左右左右側臉的點，並且算出他是屬於正臉、左側還是右側
	for(int i = 0;i < 3;i++)
	{
		if(i==0)
		{
			adjust_affLeft[i]   = m_testCoord[1];
			adjust_affRight[i]  = m_testCoord[7];
		}
		else
		{
			adjust_affLeft[i]   = m_testCoord[i+2];
			adjust_affRight[i]  = m_testCoord[i+8];
		}
	}
	m_FaceInf = GetFaceInf(&adjust_affLeft[0],&adjust_affRight[0]);
	if(m_FaceInf == -1)
	{
		cout << " Error when detect face degree\n";
	}
	cvReleaseImage(&m_TestGrayImage);
	
	if(m_FaceInf == 1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool FaceRecognition::InitRecog(int IndexPicture)
{
	string chTestFeature , chNameTestReg,
		   BMP = ".bmp";
	m_resultFile  = "..\\DataBase\\Similarity_result.txt";//<-傳進recognitionMethod

	ostringstream int_to_string;
	int_to_string << IndexPicture;

	chNameTestReg = "..\\TEST1\\" + int_to_string.str() + BMP;
	chTestFeature = "..\\feature\\5_" + int_to_string.str() + BMP;

	if( (m_TestImage	  = cvLoadImage(chNameTestReg.c_str(),0))  &&
		(m_TestColorImage = cvLoadImage(chTestFeature.c_str(),1))
		!=0 )
	{
			//***						覆蓋掉原來的資料						***/
		fstream fdle;
		fdle.open("..\\DataBase\\testConcatenatingHisMAG_total.xls",ios::out);
		fdle.close();
		fdle.open("..\\DataBase\\testConcatenatingHisPHASE_total.xls",ios::out);
		fdle.close();
		fdle.open("..\\DataBase\\TestProjectionSpace.xls",ios::out);
		fdle.close();

		/*fdle.open("..\\DataBase\\Similarity_result.txt",ios::out);
		fdle.close();
		fdle.open("..\\DataBase\\Similarity_Total_result.txt",ios::app);
		fdle.close();*/

		Panoramic test(m_TestColorImage);
		m_TestGrayImage = test.GetHsvFeature(m_TestColorImage);
		m_testCoord		= test.FindFeatureCoord(m_TestGrayImage);
		test.RearrangeCoord(m_testCoord);

		m_TraningCoord = vector<vector<Coordinate>>(m_numRegister*m_numSample,12);
		
		FILE *testfile = fopen("..\\DataBase\\NormalCoord.xls","r");//NormalizeCoord.xls
		for(int k =0;k < m_numRegister*m_numSample;k++)
		{
			for(int j = 0;j < 24;j++)
			{
				if(j%2 == 0)
				{
					fscanf(testfile,"%d",&m_TraningCoord[k][j/2].x);
				}
				else
				{
					fscanf(testfile,"%d",&m_TraningCoord[k][j/2].y);
				}
			}
		}
		fclose(testfile);

		testfile = fopen("..\\DataBase\\normalCoord_t.xls","w");
		fclose(testfile);

		return  true;
	}
	else return false;
}

int FaceRecognition::GetFaceInf(Coordinate *coord1,Coordinate *coord2)
{
	double Area1 = 0,Area2 = 0;
	double Ratio = 0;
	Area1 = 0.5*(coord1[0].x*coord1[1].y+coord1[0].y*coord1[2].x+coord1[1].x*coord1[2].y)
		  - 0.5*(coord1[1].y*coord1[2].x+coord1[0].y*coord1[1].x+coord1[0].x*coord1[2].y);		  
	Area2 = 0.5*(coord2[0].x*coord2[1].y+coord2[0].y*coord2[2].x+coord2[1].x*coord2[2].y)
		  - 0.5*(coord2[1].y*coord2[2].x+coord2[0].y*coord2[1].x+coord2[0].x*coord2[2].y);		  

	Ratio = fabs(Area1/Area2);
	if(Ratio < 1.32 && Ratio > 0.83)//-2
	{
		return FRONATAL;
	}
	else if(Ratio <= 0.83)//right
	{
		return PROFILERIGHT;
	}
	else if(Ratio >= 1.32)//left
	{
		return PROFILELEFT;
	}
	else
	{
		return -1;
	}
}

IplImage* FaceRecognition::Procrustes(IplImage *Img,Coordinate *centerCoord,int charMode)
{
	//Rotation
	double  m = 0,angle = 0;
	if((centerCoord[7].x - centerCoord[1].x)!=0)
	{
	   m = static_cast<double>(centerCoord[7].y - centerCoord[1].y)/static_cast<double>(centerCoord[7].x - centerCoord[1].x);
	   angle = atan(m)*57.25972;
	}
	CvPoint2D32f  src_center  = cvPoint2D32f(centerCoord[1].x*1.0,  centerCoord[1].y*1.0);    
	
	float map[6];
	CvMat rot_mat = cvMat( 2, 3, CV_32F,map);
	cv2DRotationMatrix( src_center, angle,1.0, &rot_mat);  

	IplImage* warpImg = cvCreateImage(cvSize(Img->width,Img->height),8,1);
    cvWarpAffine(Img, warpImg, &rot_mat);
	if(m_debug)
	{
		cvSaveImage("warpImg.bmp",warpImg);
	}
	Coordinate normal_rotate_coord[12];
	for(int i = 0;i < 12;i++)
	{
		normal_rotate_coord[i].x = centerCoord[i].x*cvGetReal2D(&rot_mat,0,0) + centerCoord[i].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
		normal_rotate_coord[i].y = centerCoord[i].x*cvGetReal2D(&rot_mat,1,0) + centerCoord[i].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	}

	//Scaling
	//鼻子中間
	CvSize new_size = cvSize(m_width,m_height);
	CvRect ROI;
	double middleX = (normal_rotate_coord[3].x + normal_rotate_coord[9].x)*0.5;
	double middleY = (normal_rotate_coord[3].y + normal_rotate_coord[9].y)*0.5;
	//取人臉出來
	int w = (normal_rotate_coord[7].x - normal_rotate_coord[1].x)/3;
	ROI.x = normal_rotate_coord[1].x - w;
	ROI.y = (normal_rotate_coord[0].y);// + normal_rotate_coord[1].y)/2;
	ROI.width  = w*5;//(middleX-ROI.x)*ratioX;//600
	ROI.height = normal_rotate_coord[5].y  - normal_rotate_coord[0].y;//(middleY - Coord0Y)*ratioY;//centerCoord[11].y-centerCoord[0].y

	if(m_FaceInf == PROFILELEFT)
	{
		ROI.x	  = normal_rotate_coord[1].x - 2*w;
		ROI.width = w*6;
	}
	if(m_FaceInf == PROFILERIGHT)
	{
		ROI.x	  = normal_rotate_coord[1].x + 1*w;
		ROI.width = w*6;
	}
	cvSetImageROI(warpImg, ROI);
	IplImage* noramlizeImg = cvCreateImage(new_size,8,1);
	cvResize(warpImg,noramlizeImg,3);
	cvReleaseImage(&warpImg);

	double ratio_w = static_cast<double>(m_width)/ROI.width;
	double ratio_h = static_cast<double>(m_height)/ROI.height;
	Coordinate normal_coord[12];
	for(int i = 0;i < 12;i++)
	{
		normal_coord[i].x = static_cast<int>((normal_rotate_coord[i].x - ROI.x )* ratio_w);
		normal_coord[i].y = static_cast<int>((normal_rotate_coord[i].y - ROI.y )* ratio_h);
	}

	string filename;
	if(charMode == 0)filename = "..\\DataBase\\NormalCoord.xls";
	else			 filename = "..\\DataBase\\normalCoord_t.xls";
	FILE* Coord = fopen(filename.c_str(),"a");
	for(int i = 0;i < 12;i++)
	{
		fprintf(Coord,"%d\t%d\t",normal_coord[i].x,normal_coord[i].y);
	}
	fprintf(Coord,"\n");
	fclose(Coord);

	return noramlizeImg;
}

void FaceRecognition::save_warp(IplImage* Img,int person)
{
	bool testMode = false;
	Coordinate normal_coord_t[12];
	fstream fptr;
	fptr.open("../Database/normalCoord_t.xls",ios::in);
	for(int i = 0;i < 12;i++)
	{
		fptr >> normal_coord_t[i].x >> normal_coord_t[i].y;
	}
	//======   Start to pre-warping   ==========//
	vector<Coordinate>adjust(4);
	vector<Coordinate>adjusted(4);
	IplImage *dst = cvCreateImage(cvSize(m_width,m_height),8,1);					//改
	cvZero(dst);
	Morphining M(4,dst);

	int sample_index = 0;//sample_index =>sample = 1
	int database_index = sample_index + person*m_numSample;
	int index; 
	for(int j = 0;j < 4;j++)
	{
		for(int i = 0;i < 4;i++)
		{	
			if(j == 0)		index = i;
			else if(j == 1)	index = i + 6;
			else if(j == 2)
			{
				if(i == 2 || i == 3) index = i + 7;
				else				 index = i + 3;
			}
			else
			{
				if(i == 2 || i == 3) index = i + 8;
				else				 index = i + 4;
			}
			adjust[i]   = normal_coord_t[index];
			adjusted[i] = m_TraningCoord[database_index][index];
		}

		M.DoProjection(Img,dst,adjusted,adjust,20000,testMode);
	}
	for(int j = 0; j < 2;j++)
	{
		for(int i = 0;i < 2;i++)
		{
			if( j == 0) index = i*2;
			else		index = i + 2;

			adjust[i]     = normal_coord_t[index];
			adjusted[i]	  = m_TraningCoord[database_index][index];
			adjust[i+2]   = normal_coord_t[index + 6];
			adjusted[i+2] = m_TraningCoord[database_index][index + 6];
			if(testMode)
			{
				printf("%d,%d  ,%d,%d \n",m_testCoord[i*2].x,m_testCoord[i*2].y,adjusted[i].x,adjusted[i].y);
				printf("%d,%d  ,%d,%d \n",m_testCoord[i*2].x,m_testCoord[i*2].y,adjusted[i].x,adjusted[i].y);
			}
		}
		M.DoProjection(Img,dst,adjusted,adjust,20000,testMode);
	}

	vector<Coordinate>adjust_aff(4);
	vector<Coordinate>adjusted_aff(4);
	vector<Coordinate>adjust_affLeft(4);
	vector<Coordinate>adjust_affRight(4);

	////-------------Do Affine------------------//
	////若圖為右側臉，則不做，反之。
	//
	Morphining S(3,dst);
	bool affine_debug = false;
	for(int i = 0;i < 2;i++)
	{
		adjusted_aff[i]  = normal_coord_t[i];
		adjust_aff[i]    = m_TraningCoord[database_index][i];

	}
	S.DoAffineTrsform(Img,dst,adjust_aff,adjusted_aff,30000000,1,affine_debug);

	for(int i = 0;i < 3;i++) 
	{
		if(i == 0)
		{
			adjusted_aff[i].x  = dst->width-1;
			adjusted_aff[i].y  = 0;
			adjust_aff[i].x    = dst->width-1;
			adjust_aff[i].y    = 0;
		}
		else
		{
			adjusted_aff[i]  = normal_coord_t[i+5];
			adjust_aff[i]    = m_TraningCoord[database_index][i+5];
		}
	}
	S.DoAffineTrsform(Img,dst,adjust_aff,adjusted_aff,30000000,1,affine_debug);

	if(m_FaceInf != PROFILERIGHT)
	{
		for(int i = 0;i < 3;i++)
		{
			if(i==0)
			{
				adjust_aff[i]		= m_TraningCoord[database_index][1];
				adjust_affLeft[i]   = normal_coord_t[1];
			}
			else
			{
				adjust_aff[i]		= m_TraningCoord[database_index][i+2];
				adjust_affLeft[i]   = normal_coord_t[i+2];
			}
		}
		S.DoAffineTrsform(Img,dst,adjust_aff,adjust_affLeft,30000000);
	}
	//若圖為左側臉，則不做，反之。
  	if(m_FaceInf != PROFILELEFT)
	{
		for(int i = 0;i < 3;i++)
		{
			if(i==0)
			{
				adjust_aff[i]	    = m_TraningCoord[database_index][7];
				adjust_affRight[i]  = normal_coord_t[7];
			}
			else
			{
				adjust_aff[i]	    = m_TraningCoord[database_index][i+8];
				adjust_affRight[i]  = normal_coord_t[i+8];
			}
		}
		S.DoAffineTrsform(Img,dst,adjust_aff,adjust_affRight,30000000);
	}
	if(m_debug)
	{
		cvNamedWindow("small",0);
		cvShowImage("small",dst);
		cvWaitKey(0);
	}
	char nameImg[30];
	sprintf(nameImg,"..\\Recognition\\%d.bmp",person);
	cvSaveImage(nameImg,dst);
}

void FaceRecognition::NormalizeTestImg(int IndexPicture)
{
	vector<Coordinate> testCoord ;
	Panoramic test(m_TestColorImage);
	IplImage   *dst;
	double  Ratio = m_TestColorImage->width/1000.;
	char nameImg[30];
	double  m,angle;
	//抓取紅色特徵點
	dst = test.GetHsvFeature(m_TestColorImage,0,255,255,51,51,51);
	testCoord = test.FindFeatureCoord(dst);
	test.RearrangeCoord(testCoord);

	//Rotate
	if((testCoord[7].x - testCoord[1].x)!=0)
	{
	   m = static_cast<double>(testCoord[7].y - testCoord[1].y)/static_cast<double>(testCoord[7].x - testCoord[1].x);
	   angle = atan(m)*57.25972;
	}
	else
	{
		angle = 0;
	}
	CvPoint2D32f  src_center  = cvPoint2D32f(testCoord[1].x*1.0,  testCoord[1].y*1.0);    
	
	float map[6];
	CvMat rot_mat = cvMat( 2, 3, CV_32F,map);
	cv2DRotationMatrix( src_center, angle,1.0, &rot_mat);  
	IplImage* rotate = cvCreateImage(cvSize(m_TestColorImage->width,m_TestColorImage->height),8,1);
    cvWarpAffine(m_TestImage, rotate, &rot_mat);

	//Coordinate normal_rotate_coord[12];
	//for(int i = 0;i < 12;i++)
	//{
	//	normal_rotate_coord[i].x = centerCoord[i].x*cvGetReal2D(&rot_mat,0,0) + centerCoord[i].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	//	normal_rotate_coord[i].y = centerCoord[i].x*cvGetReal2D(&rot_mat,1,0) + centerCoord[i].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	//}
	double Coord3X = testCoord[3].x*cvGetReal2D(&rot_mat,0,0) + testCoord[3].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord3Y = testCoord[3].x*cvGetReal2D(&rot_mat,1,0) + testCoord[3].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord9X = testCoord[9].x*cvGetReal2D(&rot_mat,0,0) + testCoord[9].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord9Y = testCoord[9].x*cvGetReal2D(&rot_mat,1,0) + testCoord[9].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord1X = testCoord[1].x*cvGetReal2D(&rot_mat,0,0) + testCoord[1].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord1Y = testCoord[1].x*cvGetReal2D(&rot_mat,1,0) + testCoord[1].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord0Y = testCoord[0].x*cvGetReal2D(&rot_mat,1,0) + testCoord[0].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord7X = testCoord[7].x*cvGetReal2D(&rot_mat,0,0) + testCoord[7].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord4Y = testCoord[4].x*cvGetReal2D(&rot_mat,1,0) + testCoord[4].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord5Y = testCoord[5].x*cvGetReal2D(&rot_mat,1,0) + testCoord[5].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;

	CvSize new_size = cvSize(m_width,m_height);
	CvRect ROI;
	//設定ROI之範圍為點點的各個max值
	double middleX = (Coord3X + Coord9X)*0.5;
	double middleY = (Coord3Y + Coord9Y)*0.5;
	//取人臉出來
	int w = (Coord7X - Coord1X)/3;
		ROI.x = Coord1X - w;
		ROI.y = (Coord0Y);//+Coord1Y)/2;
		ROI.width  = w*5;//(middleX-ROI.x)*ratioX;//600
		ROI.height = Coord5Y - Coord0Y;//Coord4Y + (Coord5Y - Coord4Y)/2 - Coord0Y;//(middleY - Coord0Y)*ratioY;//centerCoord[11].y-centerCoord[0].y
		if(m_FaceInf == PROFILELEFT)
		{
			ROI.x	  = Coord1X - 2*w;
			ROI.width = w*6;
		}
		if(m_FaceInf == PROFILERIGHT)
		{
			ROI.x	  = Coord1X + 1*w;
			ROI.width = w*6;
		}
		cvSetImageROI(rotate, ROI);
	//設定完畢
	IplImage *New_Image = cvCreateImage(new_size,8,1);
	cvResize(rotate,New_Image,3);

	sprintf(nameImg,"..\\Recognition\\%d_0.bmp",IndexPicture);
	cvSaveImage(nameImg,New_Image);

	cvReleaseImage(&rotate);
	cvReleaseImage(&New_Image);
	cvReleaseImage(&dst);
}
/*

*Generally speaking,Translation、uniform scaling、rotation is procustes analysis
*NormalizeTestImg() is to normalize face's angle and rotation

void FaceRecognition::NormalizeTestImg(int IndexPicture)
{
	vector<Coordinate> testCoord ;
	Panoramic test(m_TestColorImage);
	IplImage   *dst;
	double  Ratio = m_TestColorImage->width/1000.;
	char nameImg[30];
	double  m,angle;
	//抓取紅色特徵點
	dst = test.GetHsvFeature(m_TestColorImage,0,255,255,51,51,51);
	testCoord = test.FindFeatureCoord(dst);
	test.RearrangeCoord(testCoord);

	//Rotate
	if((testCoord[7].x - testCoord[1].x)!=0)
	{
	   m = static_cast<double>(testCoord[7].y - testCoord[1].y)/static_cast<double>(testCoord[7].x - testCoord[1].x);
	   angle = atan(m)*57.25972;
	}
	else
	{
		angle = 0;
	}
	CvPoint2D32f  src_center  = cvPoint2D32f(testCoord[1].x*1.0,  testCoord[1].y*1.0);    
	
	float map[6];
	CvMat rot_mat = cvMat( 2, 3, CV_32F,map);
	cv2DRotationMatrix( src_center, angle,1.0, &rot_mat);  
	IplImage* rotate = cvCreateImage(cvSize(m_TestColorImage->width,m_TestColorImage->height),8,1);
    cvWarpAffine(m_TestImage, rotate, &rot_mat);

	//Coordinate normal_rotate_coord[12];
	//for(int i = 0;i < 12;i++)
	//{
	//	normal_rotate_coord[i].x = centerCoord[i].x*cvGetReal2D(&rot_mat,0,0) + centerCoord[i].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	//	normal_rotate_coord[i].y = centerCoord[i].x*cvGetReal2D(&rot_mat,1,0) + centerCoord[i].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	//}
	double Coord3X = testCoord[3].x*cvGetReal2D(&rot_mat,0,0) + testCoord[3].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord3Y = testCoord[3].x*cvGetReal2D(&rot_mat,1,0) + testCoord[3].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord9X = testCoord[9].x*cvGetReal2D(&rot_mat,0,0) + testCoord[9].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord9Y = testCoord[9].x*cvGetReal2D(&rot_mat,1,0) + testCoord[9].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord1X = testCoord[1].x*cvGetReal2D(&rot_mat,0,0) + testCoord[1].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord1Y = testCoord[1].x*cvGetReal2D(&rot_mat,1,0) + testCoord[1].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord0Y = testCoord[0].x*cvGetReal2D(&rot_mat,1,0) + testCoord[0].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord7X = testCoord[7].x*cvGetReal2D(&rot_mat,0,0) + testCoord[7].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord4Y = testCoord[4].x*cvGetReal2D(&rot_mat,1,0) + testCoord[4].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord5Y = testCoord[5].x*cvGetReal2D(&rot_mat,1,0) + testCoord[5].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;

	CvSize new_size = cvSize(m_width,m_height);
	CvRect ROI;
	//設定ROI之範圍為點點的各個max值
	double middleX = (Coord3X + Coord9X)*0.5;
	double middleY = (Coord3Y + Coord9Y)*0.5;
	//取人臉出來
	int w = (Coord7X - Coord1X)/3;
		ROI.x = Coord1X - 0*w;
		ROI.y = (Coord0Y);//+Coord1Y)/2;
		ROI.width  = w*3;//(middleX-ROI.x)*ratioX;//600
		ROI.height = Coord4Y + (Coord5Y - Coord4Y)/2 - Coord0Y;//(middleY - Coord0Y)*ratioY;//centerCoord[11].y-centerCoord[0].y
		cvSetImageROI(rotate, ROI);
	//設定完畢
	IplImage *New_Image = cvCreateImage(new_size,8,1);
	cvResize(rotate,New_Image,3);

	sprintf(nameImg,"..\\Recognition\\%d.bmp",IndexPicture);
	cvSaveImage(nameImg,New_Image);

	cvReleaseImage(&rotate);
	cvReleaseImage(&New_Image);
	cvReleaseImage(&dst);
}
*/

/*
IplImage* FaceRecognition::NorDatabaseImg(IplImage *panoramic_face,Coordinate *centerCoord)
{
	CvSize new_size = cvSize(m_width,m_height);
	CvRect ROI;
	double  m = 0,angle = 0;
	int ratioX = 2,ratioY = 2;
	//Rotate
	if((centerCoord[7].x - centerCoord[1].x)!=0)
	{
	   m = static_cast<double>(centerCoord[7].y - centerCoord[1].y)/static_cast<double>(centerCoord[7].x - centerCoord[1].x);
	   angle = atan(m)*57.25972;
	}
	CvPoint2D32f  src_center  = cvPoint2D32f(centerCoord[1].x*1.0,  centerCoord[1].y*1.0);    
	
	float map[6];
	CvMat rot_mat = cvMat( 2, 3, CV_32F,map);
	cv2DRotationMatrix( src_center, angle,1.0, &rot_mat);  

	IplImage* warpImg = cvCreateImage(cvSize(panoramic_face->width,panoramic_face->height),8,1);
    cvWarpAffine(panoramic_face, warpImg, &rot_mat);

	Coordinate normal_rotate_coord[12];
	for(int i = 0;i < 12;i++)
	{
		normal_rotate_coord[i].x = centerCoord[i].x*cvGetReal2D(&rot_mat,0,0) + centerCoord[i].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
		normal_rotate_coord[i].y = centerCoord[i].x*cvGetReal2D(&rot_mat,1,0) + centerCoord[i].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	}

		//鼻子中間
	double middleX = (normal_rotate_coord[3].x + normal_rotate_coord[9].x)*0.5;
	double middleY = (normal_rotate_coord[3].y + normal_rotate_coord[9].y)*0.5;
	//取人臉出來
	int w = (normal_rotate_coord[7].x - normal_rotate_coord[1].x)/3;
	ROI.x = normal_rotate_coord[1].x - 1*w;
	ROI.y = (normal_rotate_coord[0].y);// + normal_rotate_coord[1].y)/2;
	ROI.width  = w*5;//(middleX-ROI.x)*ratioX;//600
	ROI.height = normal_rotate_coord[5].y  - normal_rotate_coord[0].y;//(middleY - Coord0Y)*ratioY;//centerCoord[11].y-centerCoord[0].y

	cvSetImageROI(warpImg, ROI);
	IplImage* noramlizeImg = cvCreateImage(new_size,8,1);
	cvResize(warpImg,noramlizeImg,3);
	cvReleaseImage(&warpImg);

	double ratio_w = static_cast<double>(m_width)/ROI.width;
	double ratio_h = static_cast<double>(m_height)/ROI.height;
	Coordinate normal_coord[12];
	for(int i = 0;i < 12;i++)
	{
		int normal_x = static_cast<int>((normal_rotate_coord[i].x - ROI.x )* ratio_w);
		int normal_y = static_cast<int>((normal_rotate_coord[i].y - ROI.y )* ratio_h);
		normal_coord[i].x = normal_x;
		normal_coord[i].y = normal_y;
		//if(normal_x > -1 && normal_x < m_width)
		//	normal_coord[i].x = normal_x;
		//else if(normal_x > m_width)
		//	normal_coord[i].x = m_width;
		//else
		//	normal_coord[i].x = 0;
		//if(normal_y > -1 && normal_y < m_height)
		//	normal_coord[i].y = normal_y;
		//else if(normal_y > m_height)
		//	normal_coord[i].y = m_height;
		//else
		//	normal_coord[i].y = 0;
	}

	FILE *Coord = fopen("..\\DataBase\\Coordinate.xls","a");
	for(int i = 0;i < 12;i++)
	{
		fprintf(Coord,"%d\t%d\t",centerCoord[i].x,centerCoord[i].y);
	}
	fprintf(Coord,"\n");
	fclose(Coord);

	Coord = fopen("..\\DataBase\\NormalCoord.xls","a");
	for(int i = 0;i < 12;i++)
	{
		fprintf(Coord,"%d\t%d\t",normal_coord[i].x,normal_coord[i].y);
	}
	fprintf(Coord,"\n");
	fclose(Coord);
	return noramlizeImg;
}

*/

/*
//coord正規畫的部分還沒處理
void FaceRecognition::PreWarping(int person)
{
	vector<Coordinate>adjust(4);
	vector<Coordinate>adjusted(4);
	vector<Coordinate>adjust_aff(4);
	vector<Coordinate>adjusted_aff(4);
	vector<Coordinate>adjust_affLeft(4);
	vector<Coordinate>adjust_affRight(4);
	Coordinate test[3];
	IplImage   *ResizeImg = cvCreateImage(cvSize(m_width,m_height),8,1);
	IplImage   *dst;
	char nameImg[30];
	double  Ratio = m_TestColorImage->width/1000.;

	dst = cvCreateImage(cvSize(m_TestImage->width,m_TestImage->height),8,1);					//改
	cvZero(dst);
	//======   Start to pre-warping   ==========//
	Morphining M(4,dst);

	for(int i = 0;i < 4;i++)
	{
		adjust[i]   = m_testCoord[i];
 		adjusted[i] = m_TraningCoord[person][i];
		//printf("adjust%d,%d   \n",adjust[i].x,adjust[i].y);
		//printf("adjusted:%d,%d  \n",adjusted[i].x,adjusted[i].y);
	}

	M.DoProjection(m_TestImage,dst,adjusted,adjust,20000);

	for(int i = 0;i < 2;i++)
	{
		adjust[i]     = m_testCoord[i*2];
		adjusted[i]	  = m_TraningCoord[person][i*2];
		adjust[i+2]   = m_testCoord[i*2+6];
		adjusted[i+2] = m_TraningCoord[person][i*2+6];
		//printf("%d,%d  ,%d,%d \n",m_testCoord[i*2].x,m_testCoord[i*2].y,adjusted[i].x,adjusted[i].y);
		//printf("%d,%d  ,%d,%d \n",m_testCoord[i*2].x,m_testCoord[i*2].y,adjusted[i].x,adjusted[i].y);
	}
	M.DoProjection(m_TestImage,dst,adjusted,adjust,20000);

	for(int i = 0;i < 4;i++)
	{
		adjust[i]    = m_testCoord[i+6];
		adjusted[i]  = m_TraningCoord[person][i+6];
		//printf("%d,%d  ,%d,%d \n",adjust[i].x,adjust[i].y,adjusted[i].x,adjusted[i].y);
	}
	M.DoProjection(m_TestImage,dst,adjusted,adjust,20000);

	for(int i = 0;i < 2;i++)
	{
		adjust[i]     = m_testCoord[i+2];
		adjusted[i]   = m_TraningCoord[person][i+2];
		adjust[i+2]   = m_testCoord[i+2+6];
		adjusted[i+2] = m_TraningCoord[person][i+2+6];
	}
	M.DoProjection(m_TestImage,dst,adjusted,adjust,20000);

	for(int i = 0;i < 4;i++)
	{
		adjust[i]    = m_testCoord[i+3];
		adjusted[i] = m_TraningCoord[person][i+3];

		if(i == 2||i==3)
		{
			adjust[i]   = m_testCoord[i+7];
			adjusted[i] = m_TraningCoord[person][i+7];
		}
	}
	M.DoProjection(m_TestImage,dst,adjusted,adjust,20000);

	for(int i = 0;i < 4;i++)
	{
		adjust[i]   = m_testCoord[i+4];
		adjusted[i] = m_TraningCoord[person][i+4];
		if(i == 2||i==3)
		{
			adjust[i]   = m_testCoord[i+8];
			adjusted[i] = m_TraningCoord[person][i+8];
		}
	}
	M.DoProjection(m_TestImage,dst,adjusted,adjust,20000);

	//-------------Do Affine------------------//
	//若圖為右側臉，則不做，反之。
	
	Morphining S(3,dst);

	for(int i = 1;i < 3;i++)
	{
		adjusted_aff[i]  = m_testCoord[i];
		adjust_aff[i]    = m_TraningCoord[person][i];
		
	}
	S.DoAffineTrsform(m_TestImage,dst,adjust_aff,adjusted_aff,30000000);

	for(int i = 0;i < 3;i++)
	{
		if(i == 0)
		{
			adjusted_aff[i].x  = dst->width-1;
			adjusted_aff[i].y  = 0;
			adjust_aff[i].x    = dst->width-1;
			adjust_aff[i].y    = 0;
		}
		else
		{
			adjusted_aff[i]  = m_testCoord[i+5];
			adjust_aff[i]    = m_TraningCoord[person][i+5];
		}
	}
	S.DoAffineTrsform(m_TestImage,dst,adjust_aff,adjusted_aff,30000000);

	if(m_FaceInf != PROFILERIGHT)
	{
		for(int i = 0;i < 3;i++)
		{
			if(i==0)
			{
				adjust_aff[i]		= m_TraningCoord[person][1];
				adjust_affLeft[i]   = m_testCoord[1];
			}
			else
			{
				adjust_aff[i]		= m_TraningCoord[person][i+2];
				adjust_affLeft[i]   = m_testCoord[i+2];
			}
		}
		S.DoAffineTrsform(m_TestImage,dst,adjust_aff,adjust_affLeft,30000000);
	}
	//若圖為左側臉，則不做，反之。
	if(m_FaceInf != PROFILELEFT)
	{
		for(int i = 0;i < 3;i++)
		{
			if(i==0)
			{
				adjust_aff[i]	    = m_TraningCoord[person][7];
				adjust_affRight[i]  = m_testCoord[7];
			}
			else
			{
				adjust_aff[i]	    = m_TraningCoord[person][i+8];
				adjust_affRight[i]  = m_testCoord[i+8];
			}
		}
		S.DoAffineTrsform(m_TestImage,dst,adjust_aff,adjust_affRight,30000000);
	}
	////做正規化
	CvRect ROI;
	//鼻子中間
	double middleX = (m_TraningCoord[person][3].x + m_TraningCoord[person][9].x)*0.5;
	double middleY = (m_TraningCoord[person][3].y + m_TraningCoord[person][9].y)*0.5;
	int w = (m_TraningCoord[person][7].x - m_TraningCoord[person][1].x)/3;
	//設定ROI之範圍為點點的各個max值
		ROI.x = m_TraningCoord[person][1].x - 1*w;
		ROI.y = m_TraningCoord[person][0].y;
		ROI.width  = w*5;//(middleX-ROI.x)*WIDTHR;
		ROI.height = m_TraningCoord[person][5].y - m_TraningCoord[person][0].y;//(middleY - m_TraningCoord[person][0].y)*HEIGHTR;
		cvSetImageROI(dst, ROI);
	cvResize(dst,ResizeImg,CV_INTER_CUBIC);
	sprintf(nameImg,"..\\Recognition\\%d.bmp",person/m_numSample);
	cvSaveImage(nameImg,dst);
	cvReleaseImage(&dst);
	//cvReleaseImage(&ResizeImg);
}

*/