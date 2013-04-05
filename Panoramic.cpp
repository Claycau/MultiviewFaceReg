#include "Panormaic.h"

Panoramic::Panoramic(IplImage *img,bool debug)
{
	m_height     = img->height; 
	m_width      = img->width;
	m_leftImg    = NULL;
	m_centerImg  = NULL;
	m_rightImg   = NULL;
	m_debug		 = debug;
}

Panoramic::Panoramic(const char *leftName, const char *centerName,const char *rightName,bool debug)
{
	m_do_sttich = false;
	m_lineT     = 30;//150
	m_leftImg   = cvLoadImage(leftName,0);
	m_centerImg = cvLoadImage(centerName,0);
	m_rightImg  = cvLoadImage(rightName,0); 
	m_height    = m_leftImg->height; 
	m_width     = m_leftImg->width;
	m_debug		= debug;
	m_PanoramicFace	= cvCreateImage(cvSize(m_width,m_height),8,1);
	cvZero(m_PanoramicFace);
}

Panoramic::~Panoramic()
{
	if(m_leftImg != NULL )
	{
		cvReleaseImage(&m_leftImg);
	}
	if(m_centerImg != NULL)
	{
		cvReleaseImage(&m_centerImg);
	}
	if(m_rightImg != NULL)
	{
		cvReleaseImage(&m_rightImg);
	}
	//cvReleaseImage(&m_PanoramicFace);
}

IplImage* Panoramic::StitchFace(const char *leftName, const char *centerName,const char *rightName)
{
	IplImage   *leftHsvImg,*centerHsvImg,*rightHsvImg;
	vector<Coordinate> leftCoord;
	vector<Coordinate> rightCoord;
	vector<Coordinate> centerCoord;
	vector<Coordinate> profileCoord(3);
	vector<Coordinate> centerAffineCoord(3);
	IplImage   *leftAffineImg     = cvCreateImage(cvSize(m_width,m_height),8,1);
	IplImage   *rightAffineImg    = cvCreateImage(cvSize(m_width,m_height),8,1);
	IplImage   *leftFeatureImg    = cvLoadImage(leftName,1);
	IplImage   *centerFeatureImg  = cvLoadImage(centerName,1);
	IplImage   *rightFeatureImg   = cvLoadImage(rightName,1); 
	cvZero(rightAffineImg);
	cvZero(leftAffineImg);

	//Using red color threshold to find the features from input image
	leftHsvImg  = GetHsvFeature(leftFeatureImg   ,0,255,255,51,51,51);
	centerHsvImg= GetHsvFeature(centerFeatureImg ,0,255,255,51,51,51);
	rightHsvImg = GetHsvFeature(rightFeatureImg  ,0,255,255,51,51,51);
	//FindFeatureCoord will decide whether it continues or not.
    leftCoord   =  FindFeatureCoord(leftHsvImg);
	rightCoord  =  FindFeatureCoord(rightHsvImg);
	centerCoord =  FindFeatureCoord(centerHsvImg);
	
	if(m_do_sttich)//when all number of feature coord = 12,it will be true,it decide in function:"FindFeatureCoord"
	{
		RearrangeCoord(leftCoord);
		RearrangeCoord(rightCoord);
		RearrangeCoord(centerCoord);

		for(int i = 0; i < m_numFeature; i++) 
		{
			m_centerCood[i] = centerCoord[i];
		}
		if(m_debug)
		{
			ShowFeature(leftCoord);
			ShowFeature(centerCoord);
			ShowFeature(rightCoord);
		}
	
		Graphic FindLine;

		for(int numStitch = 0; numStitch < 2;numStitch++)
		{
			for(int num = 0;num < 3;num++)
			{
				if(numStitch == 1)
				{
					if(num==0)
					{
						profileCoord[0]         = leftCoord[1];
						centerAffineCoord[0]    = centerCoord[1];
					}
					else
					{
						profileCoord[num]       = leftCoord[num+2];
						centerAffineCoord[num]  = centerCoord[num+2];
					}
				}

				else
				{
					if(num==0)
					{
						profileCoord[0]			= rightCoord [7];
						centerAffineCoord[0]	= centerCoord[7];
					}
					else
					{
						profileCoord[num]       = rightCoord [num+8];
						centerAffineCoord[num]  = centerCoord[num+8];
					}
				}
			}
			//縫合左側臉
			if(numStitch == 1)
			{
				FindLine.Line(centerAffineCoord,0,centerAffineCoord,2,m_slope,m_intercept);
				DoAffineTrsform(m_leftImg,leftAffineImg,profileCoord,centerAffineCoord);
				if(m_debug)
				{
					cvNamedWindow("leftAffineImg",0);
					cvShowImage("leftAffineImg",leftAffineImg);
				}
				ShowStitch(leftAffineImg,m_centerImg); //側臉縫合、線性模糊化
			}
			//縫合右側臉
			else
			{
				FindLine.Line(centerAffineCoord,0,centerAffineCoord,2,m_slope,m_intercept);
				DoAffineTrsform(m_rightImg,rightAffineImg,profileCoord,centerAffineCoord);
				if(m_debug)
				{
					cvNamedWindow("rightAffineImg",0);
					cvShowImage("rightAffineImg",rightAffineImg);
				}
				ShowStitch(rightAffineImg,m_centerImg);
			}
				m_saveSlope[numStitch]		=	m_slope;
				m_saveIntercept[numStitch]	=	m_intercept;
		
		}

		//縫合正臉
		for(int j = 0;j < m_height;j++)
		{
			for(int i = 0;i < m_width;i++)
			{
				double linePostionright = m_saveSlope[0]*i + m_saveIntercept[0]-j;
				double linePostionleft  = m_saveSlope[1]*i + m_saveIntercept[1]-j;

				if(linePostionright > m_lineT && linePostionleft > m_lineT)
				{
					double pixel = cvGetReal2D(m_centerImg,j,i);
					cvSetReal2D(m_PanoramicFace,j,i,pixel) ;
				}
			}
		}
		if(m_debug)
		{
			cvNamedWindow("PanoramicFace",0);
			cvShowImage("PanoramicFace",m_PanoramicFace);
			cvWaitKey(0);
		}
		cvReleaseImage(&leftHsvImg);	cvReleaseImage(&centerHsvImg);		cvReleaseImage(&rightHsvImg);
		cvReleaseImage(&leftAffineImg);	cvReleaseImage(&rightAffineImg);
		cvReleaseImage(&leftFeatureImg);cvReleaseImage(&centerFeatureImg);	cvReleaseImage(&rightFeatureImg);
		return m_PanoramicFace;
	}
	else
	{
		printf("Error when stich image....");
		return NULL;
	}
	
}

IplImage* Panoramic::GetHsvFeature(IplImage* src,int H,int S,int V,int Scale ,int Scale_1 ,int Scale_2)
{
	IplImage *colorImg	  = cvCreateImage(cvGetSize(src),8,3);
    IplImage *hsvImg	  = cvCreateImage(cvGetSize(src),8,3);
	cvCopy(src,colorImg);
	IplImage *Plane_1	  = cvCreateImage( cvGetSize(colorImg), 8, 1);//H plane
    IplImage *Plane_2	  = cvCreateImage( cvGetSize(colorImg), 8, 1);//S plane
	IplImage *Plane_3	  = cvCreateImage( cvGetSize(colorImg), 8, 1);//V plane
	IplImage *dst	      = cvCreateImage( cvGetSize(src),8,1);
	cvCvtColor(colorImg,hsvImg,CV_BGR2HSV);
	cvCvtPixToPlane( hsvImg, Plane_1, Plane_2, Plane_3, 0 );

	cvEqualizeHist(Plane_2,Plane_2);//s_plane
	cvEqualizeHist(Plane_3,Plane_3);//v_plane
	cvMerge(Plane_1,Plane_2,Plane_3,0,hsvImg);
	cvInRangeS(hsvImg, cvScalar(H,S, V), cvScalar(5*Scale+H,5*Scale_1+S,5*Scale_2+V), dst);//cvScalar(0,40, 40), cvScalar(60, 170, 255)
	cvErode(dst,dst,0,2);

	/*cvNamedWindow("HSV_ROI",0);
	cvShowImage ("HSV_ROI",dst);*/

	cvReleaseImage(&colorImg);
	cvReleaseImage(&hsvImg);
	cvReleaseImage(&Plane_1);
	cvReleaseImage(&Plane_2);
	cvReleaseImage(&Plane_3);

	return dst;
}

vector<Coordinate> Panoramic::FindFeatureCoord(IplImage *img)
{
	m_numFeature = 0;

	for(int j = 0;j < m_height;j++)
	{
		for(int i = 0;i < m_width;i++)
		{
			if(cvGetReal2D(img,j,i) > 0 )
			{	
				m_numFeature++;
			}
		}
	}

	vector<Coordinate> Coord(m_numFeature);

	if(m_debug)
	{
		printf("Total number of features = %d\n",m_numFeature);
	}

	int num = 0;

	for(int j = 0;j < m_height;j++)
	{
		for(int i = 0;i < m_width;i++)
		{
			if(cvGetReal2D(img,j,i) > 0 )
			{	
				Coord[num].x = i; 
				Coord[num].y = j;
				num++;
			}
		}
	}
	m_do_sttich =!(m_do_sttich);
	return Coord;
}

void Panoramic::RearrangeCoord(vector<Coordinate>& srcCoord)
{
	Coordinate new_origin;
	//int num_left = 0,num_right = 0;//右邊特徵點個數 = m_numFeature - num_left
	Coordinate leftCoord[6];
	Coordinate rightCoord[6];

	for(int i = 0; i < 5;i++)
	{
		if(i == 0)
		{
			if(srcCoord[2*i].x > srcCoord[2*i+1].x)
			{
				leftCoord[i]  = srcCoord[2*i+1];
				rightCoord[i] = srcCoord[2*i];
			}
			else
			{
				leftCoord[i]  = srcCoord[2*i];
				rightCoord[i] = srcCoord[2*i+1];
			}
		}

		else if(i == 1)
		{
			for(int j = 0;j < 4;j++)
			{
				new_origin.x += srcCoord[2*i+j].x/4;
			}

			int next_r = 0,next_l = 0;

			for(int j = 0;j < 4;j++)
			{
				if(srcCoord[2*i+j].x > new_origin.x)
				{
					rightCoord[i+next_r] = srcCoord[2*i+j];
					next_r++;
				}
				else
				{
					leftCoord[i+next_l] = srcCoord[2*i+j];
					next_l++;
				}
			}
			
			if(rightCoord[1].x < rightCoord[2].x)
			{
				Coordinate temp;
				temp = rightCoord[2];
				rightCoord[2] = rightCoord[1];
				rightCoord[1] = temp;
			}
			if(leftCoord[1].x > leftCoord[2].x)
			{
				Coordinate temp;
				temp = leftCoord[2];
				leftCoord[2] = leftCoord[1];
				leftCoord[1] = temp;
			}

		}
		//when i >= 2
		else
		{
			if(srcCoord[2*i+2].x > srcCoord[2*i+3].x)
			{
				leftCoord[i+1]  = srcCoord[2*i+3];
				rightCoord[i+1] = srcCoord[2*i+2];
			}
			else
			{
				leftCoord[i+1]  = srcCoord[2*i+2];
				rightCoord[i+1] = srcCoord[2*i+3];
			}
		}
	}

	for(int i = 0 ; i < m_numFeature/2;i++)
	{

			srcCoord[i]   = leftCoord[i];
			srcCoord[i+6] = rightCoord[i];
	}
}

void Panoramic::DoAffineTrsform(IplImage *src,IplImage *dst,vector<Coordinate> adjustedCoord,vector<Coordinate> adjustCoord)//m_lineT:Threshold of SSD's error;
{
	CvMat* Matrix = cvCreateMat(3,3,CV_32FC1);
	Matrix = GetAffineMatrix(adjustCoord,adjustedCoord);
	AffineTransform(src,dst,Matrix);
	cvReleaseMat(&Matrix);	   
}
/**	@Specifically designed for this program 
	*so it is include m_lineT to reduce unnecessary operation*/
void Panoramic::AffineTransform(IplImage *src,IplImage *dst,CvMat *Matrix)
{
	for(int j = 0; j < m_height;j++)
	{
		for(int i = 0 ; i < m_width;i++)
		{
			double linePosition = m_slope*i + m_intercept-j;

			if(linePosition <= m_lineT )  //
			{
				int x  = i*cvGetReal2D(Matrix,0,0) + j*cvGetReal2D(Matrix,0,1) + cvGetReal2D(Matrix,0,2)*1.0;
				int y  = i*cvGetReal2D(Matrix,1,0) + j*cvGetReal2D(Matrix,1,1) + cvGetReal2D(Matrix,1,2)*1.0;
					
				if(x > 0 && y > 0 && x < m_width && y < m_height)
				{
					double pixel = cvGetReal2D(src,y,x);
					cvSetReal2D(dst,j,i,pixel) ;
				}
				else
				{
					cvSetReal2D(dst,j,i,0) ;
				}
			}
		}
	}
}

void Panoramic::PrintMatrix(CvMat *Matrix,int Rows,int Cols,int Channels)
{
    for(int i = 0 ;i < Rows ;i++)
    {
        for(int j = 0 ; j < Cols ;j++)
        {
            for(int k = 0; k < Channels ;k++)
            {
				double a = cvGet2D(Matrix,i,j).val[k];
                printf("%.2f\t",a);
            }
        }
		printf("\n");
   }
}

CvMat* Panoramic::GetAffineMatrix(vector<Coordinate> adjustCoord,vector<Coordinate> adjustedCoord)
{
	//affineMatrix:affine transformation,b_temp_inv:inverse of affine transform
	// x1' = w11 * x1 + w12 * y1 + w13
    // y1' = w21 * x1 + w22 * y1 + w13
    // x2' = w11 * x2 + w12 * y2 + w13
    // y2' = w21 * x2 + w22 * y2 + w13
	// x3' = w11 * x3 + w12 * y3 + w13
    // y3' = w21 * x3 + w22 * y3 + w13
	// Ax = b  x:left or right b:front face 

			m_affineMatrix	= cvCreateMat(3, 3, CV_32FC1);
	CvMat *invAffineMatrix  = cvCreateMat(3, 3, CV_32FC1);
	CvMat *B				= cvCreateMat(3, 3, CV_32FC1);
	CvMat *X				= cvCreateMat(3, 3, CV_32FC1);
	CvMat *X_Inv			= cvCreateMat(3, 3, CV_32FC1);

	for(int i = 0;i < 3;i++)
	{
		cvSetReal2D(B, 0, i, adjustCoord[i].x);
		cvSetReal2D(B, 1, i, adjustCoord[i].y);
		cvSetReal2D(B, 2, i, 1);

		cvSetReal2D(X, 0 , i, adjustedCoord[i].x);
		cvSetReal2D(X, 1 , i, adjustedCoord[i].y);
		cvSetReal2D(X, 2 , i, 1);
	}

	cvInv(X, X_Inv);
	cvmMul(B,X_Inv,m_affineMatrix);
	cvInv(m_affineMatrix,invAffineMatrix);

	if(m_debug)
	{
		cout << "inverse affine matrix\n";
		PrintMatrix(invAffineMatrix,3,3);
	}

	cvReleaseMat(&B);
	cvReleaseMat(&X);	
	cvReleaseMat(&X_Inv);	
	cvReleaseMat(&m_affineMatrix);
	return invAffineMatrix;
}

bool Panoramic::SSD(Coordinate* coord1,Coordinate* coord2,CvMat *Matrix,double m_Threshold)
{
	double err = 0;

	Coordinate *coord_Trans = new Coordinate [m_numFeature];

	for(int i = 0 ; i < m_numFeature;i++)
	{

		double xprime = coord1[i].x*cvGetReal2D(Matrix,0,0) + coord1[i].y*cvGetReal2D(Matrix,0,1) + cvGetReal2D(Matrix,0,2)*1.0;
		double yprime = coord1[i].x*cvGetReal2D(Matrix,1,0) + coord1[i].y*cvGetReal2D(Matrix,1,1) + cvGetReal2D(Matrix,1,2)*1.0;
		double wprime = coord1[i].x*cvGetReal2D(Matrix,2,0) + coord1[i].y*cvGetReal2D(Matrix,2,1) + cvGetReal2D(Matrix,2,2)*1.0;

		coord_Trans[i].x = (int)ceil (xprime/wprime);
		coord_Trans[i].y = (int)ceil (yprime/wprime);
		//printf("SSD:coord_Trans:<x,y> = %d,%d\n",coord_Trans[i].x,coord_Trans[i].y);
	}

	for(int i = 0;i < m_numFeature;i++)
	{

		double err_x = coord2[i].x - coord_Trans[i].x;
		double err_y = coord2[i].y - coord_Trans[i].y;

		err += err_x*err_x + err_y*err_y;
	}

	printf("SSD\nError = %f\n",err);

	delete [] coord_Trans;

	if(err < m_Threshold)
	return true;

	else
	return false;
}

double Panoramic::LinearBlending(double m_lineT,double Position,double Alpha)
{
	//m_lineT:range of processing,in this programa is slope
	return (((1-Alpha)/(m_lineT))*(Position)+Alpha);
}

void Panoramic::ShowFeature(vector<Coordinate> featureCoord)
{
	IplImage   *dst = cvCreateImage(cvSize(m_width,m_height),8,1);
	cvZero(dst);
	
	for(int num = 0;num < m_numFeature;num++)
	{
		for(int y = 0;y < 10;y++)
		{
			for(int x = 0;x < 10;x++)
			{
				if(y+featureCoord[num].y < dst->height && x+featureCoord[num].x < dst->width)
				cvSetReal2D(dst,featureCoord[num].y+y,featureCoord[num].x+x,255);
			}
		}

		cvNamedWindow("Feature's coordinate",0);
		cvShowImage("Feature's coordinate",dst);
		cvWaitKey(0);

	}
	cvReleaseImage(&dst);
}

void Panoramic::ShowStitch(IplImage *profile,IplImage *center)
{
	double profileFaceDate = 0.;
	double Alpha = 0.5;//allocate middle of stich image a half of center and lateral image
	double Blendling = 0.;

	for(int j = 0;j < m_height;j++)
	{
		for(int i = 0;i < m_width;i++)
		{
			double linePosition = m_slope*i+m_intercept-j;

			profileFaceDate = cvGetReal2D(profile,j,i);

			if(linePosition <= -m_lineT)
			{
				cvSetReal2D(m_PanoramicFace,j,i,profileFaceDate);
			}

			else if(linePosition > -m_lineT && linePosition < m_lineT)
			{
				double getAlpha = LinearBlending(m_lineT,m_slope*i+m_intercept-j,Alpha);
				double getBeta  = 1-getAlpha;

				Blendling = getAlpha*cvGetReal2D(center,j,i) + getBeta*profileFaceDate;

				if(Blendling > 255)	  Blendling = 255;
				else if(Blendling < 0)Blendling = 0;
				cvSetReal2D(m_PanoramicFace,j,i,Blendling) ;
			}
		}
	}
}

void Panoramic::GetCenterFaceCoord(Coordinate* getCoord)
{
	for(int i = 0;i < 12;i++)
	{
		getCoord[i] = m_centerCood[i];
	}
}