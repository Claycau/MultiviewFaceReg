#include "Morphining.h"
#include "GraphicProcessing.h"
#include "Vector.h"
// static c AffineTrsform
Morphining::Morphining(int num_Feature,IplImage *img_in,bool debug)
{
	m_numFeature  =  num_Feature;
	m_width   = img_in->width;
	m_height  = img_in->height;
	m_debug	  = debug;
	U		  = cvCreateMat( 2*m_numFeature, 1 , CV_32FC1);
	W		  = cvCreateMat( 2*m_numFeature, 8 , CV_32FC1);
    Inv		  = cvCreateMat( 8 , 8 , CV_32FC1);
	MulResult = cvCreateMat( 8 , 8 , CV_32FC1);
	T         = cvCreateMat( 8 , 2*m_numFeature,   CV_32FC1);
	m		  = new double [m_numFeature];
	b		  = new double [m_numFeature];
}

Morphining::~Morphining()
{
	cvReleaseMat(&U);
	cvReleaseMat(&W);
	cvReleaseMat(&Inv);
	cvReleaseMat(&MulResult);
	cvReleaseMat(&T);
	delete [] m;
	delete [] b;
}
//coord1:校正用之點 coord2:被校正之點
//可執行四個以上的轉換
CvMat* Morphining::GetProjectionMatrix(vector<Coordinate> adjust,vector<Coordinate> adjusted,bool debug)
{
	//spec://cvSetReal2D(cvMat,row,col);
	if(m_numFeature < 4)
	{
		printf("Not enough tiles points to execute perspective transform!\n");
		exit(0);
	}

	//Element of PerspectiveTransformation 
	CvMat *b			= cvCreateMat( 8   ,  1         , CV_32FC1);
	CvMat *MUL_8_2N		= cvCreateMat( 8   , 2*m_numFeature , CV_32FC1);
	CvMat *b_temp		= cvCreateMat( 3   , 3          , CV_32FC1);
	CvMat *b_temp_inv   = cvCreateMat( 3   , 3          , CV_32FC1);
	//Size:2n*1 
	for(int i = 0;i < m_numFeature;i++)
	{
		cvSetReal2D(U, i,                0, adjusted[i].x);
		cvSetReal2D(U, i + m_numFeature, 0, adjusted[i].y);
	}
	//Size:2n*8
	for(int i = 0; i < m_numFeature;i++)
	{

		double col_6	  = -1.0 * adjust[i].x * cvGetReal2D(U,i,0);
		double col_7      = -1.0 * adjust[i].y * cvGetReal2D(U,i,0);
		double col_6_pair = -1.0 * adjust[i].x * cvGetReal2D(U,i + m_numFeature,0);
		double col_7_pair = -1.0 * adjust[i].y * cvGetReal2D(U,i + m_numFeature,0);

		cvSetReal2D(W, i, 0, adjust[i].x);
		cvSetReal2D(W, i, 1, adjust[i].y);
		cvSetReal2D(W, i, 2, 1);
		cvSetReal2D(W, i, 3, 0);
		cvSetReal2D(W, i, 4, 0);
		cvSetReal2D(W, i, 5, 0);
		cvSetReal2D(W, i, 6, col_6);
		cvSetReal2D(W, i, 7, col_7);
		
		cvSetReal2D(W, i + m_numFeature, 0, 0);
		cvSetReal2D(W, i + m_numFeature, 1, 0);
		cvSetReal2D(W, i + m_numFeature, 2, 0);
		cvSetReal2D(W, i + m_numFeature, 3, adjust[i].x);
		cvSetReal2D(W, i + m_numFeature, 4, adjust[i].y);
		cvSetReal2D(W, i + m_numFeature, 5, 1);
		cvSetReal2D(W, i + m_numFeature, 6, col_6_pair);
		cvSetReal2D(W, i + m_numFeature, 7, col_7_pair);

	}
	if(debug)
	{
		std::cout << "U:\n";
		PrintMatrix(U, 2*m_numFeature,1,1);
		std::cout << "W:\n";
		PrintMatrix(W, 2*m_numFeature,8,1);
	}

	if(W->cols == W->rows )cvInv(W,MUL_8_2N);
	
	else {	//pseudo inverse
			cvTranspose(W,T);
			cvmMul(T,W,MulResult);
			cvInv(MulResult,Inv);
			cvmMul(Inv,T,MUL_8_2N);
			/*printf("T\n");
			PrintMatrix(T, 8,2*m_numFeature,1);*/
			/*printf("MulResult\n");
			PrintMatrix(MulResult, 8,2*m_numFeature,1);*/
			/*printf("Inv\n");
			PrintMatrix(Inv,2*m_numFeature, 8,1);*/
	}
	
	cvmMul(MUL_8_2N,U,b);

	cvSetReal2D(b_temp, 0, 0, cvGetReal2D(b,0,0)); cvSetReal2D(b_temp, 0, 1, cvGetReal2D(b,1,0)); cvSetReal2D(b_temp, 0, 2, cvGetReal2D(b,2,0));
	cvSetReal2D(b_temp, 1, 0, cvGetReal2D(b,3,0)); cvSetReal2D(b_temp, 1, 1, cvGetReal2D(b,4,0)); cvSetReal2D(b_temp, 1, 2, cvGetReal2D(b,5,0));
	cvSetReal2D(b_temp, 2, 0, cvGetReal2D(b,6,0)); cvSetReal2D(b_temp, 2, 1, cvGetReal2D(b,7,0)); cvSetReal2D(b_temp, 2, 2, 1);
	cvInv(b_temp,b_temp_inv);
	if(debug)
	{
		std::cout << "b_temp\n";
		PrintMatrix(b_temp,3, 3,1);
	
		std::cout << "b_temp_inv\n";
		PrintMatrix(b_temp_inv,3, 3,1);
		std::cout << "......................................................................\n";
		std::cout << "......................................................................\n";
	}
	cvReleaseMat(&b);
	cvReleaseMat(&MUL_8_2N);
	cvReleaseMat(&b_temp);
	//return b_temp;
	return b_temp_inv;
}
//Gray Image
void   Morphining::ProjectionTrsform(IplImage *src,IplImage *dst,CvMat *Matrix,bool debug)
{
	int width = src->width,height = src->height;
	if(m_numFeature < 4)
	{
		printf("Not enough tiles points to execute perspective transform!\n");
		exit(0);
	}
	IplImage * test = cvCreateImage(cvSize(60,60),8,1);
	cvZero(test);
	for(int j = 0; j < height;j++)
	{
		for(int i = 0 ; i < width;i++)
		{
			if(Point_in_Polygon(Vertex,i,j,m_numFeature))
			{
				double xprime = i*cvGetReal2D(Matrix,0,0) + j*cvGetReal2D(Matrix,0,1) + cvGetReal2D(Matrix,0,2)*1.0;
				double yprime = i*cvGetReal2D(Matrix,1,0) + j*cvGetReal2D(Matrix,1,1) + cvGetReal2D(Matrix,1,2)*1.0;
				double wprime = i*cvGetReal2D(Matrix,2,0) + j*cvGetReal2D(Matrix,2,1) + cvGetReal2D(Matrix,2,2)*1.0;

				int x = (int) (xprime/wprime + 0.5);
				int y = (int) (yprime/wprime + 0.5); 
			
				if(x >= 0 && y >= 0 && x < width && y < height)
				{
					double pixel = cvGetReal2D(src,y,x);
					cvSetReal2D(dst,j,i,pixel) ;
				}
			}
		}
	}

	cvReleaseImage(&test);
	if(m_debug)
	{
		cvNamedWindow("morphine",0);
		cvShowImage("morphine",dst);
		cvWaitKey(0);
	}
}

void   Morphining::PrintMatrix(CvMat *Matrix,int Rows,int Cols,int Channels=1)
{
    for(int i = 0 ;i < Rows ;i++)
    {
        for(int j = 0 ; j < Cols ;j++)
        {
            for(int k = 0; k < Channels ;k++)
            {
				double a = cvGet2D(Matrix,i,j).val[k];
				cout << a << "\t";
            }
        }
	cout << "\n";
   }
	cout << "\n";
}

bool   Morphining::SSD(Coordinate* coord1,Coordinate* coord2,CvMat *Matrix,double Threshold)
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
		printf("SSD:coord_Trans:<x,y> = %d,%d\n",coord_Trans[i].x,coord_Trans[i].y);
	}

	for(int i = 0;i < m_numFeature;i++)
	{

		double err_x = coord2[i].x - coord_Trans[i].x;
		double err_y = coord2[i].y - coord_Trans[i].y;

		err += err_x*err_x + err_y*err_y;
	}

	printf("SSD\nError = %f\n",err);

	delete [] coord_Trans;

	if(err < Threshold)
	return true;

	else
	return false;
}
//src:要被轉換的圖 dst:輸出圖//coord1:轉換後之點//coord2:轉換前之點//T:Acceptable range of threshold after transforming
void   Morphining::DoProjection(IplImage *src,IplImage *dst,vector<Coordinate> adjusted,vector<Coordinate> adjust,double T,bool debug)
{
	CvMat* Matrix = cvCreateMat(3,3,CV_32FC1);
	//rearrage coord to clockwise bcs we need to cut the polygon from image
	cvZero(Matrix);

	Coordinate temp;
	temp = adjusted[3];
	adjusted[3] = adjusted[2];
	adjusted[2] = temp;
	temp = adjust[3];
	adjust[3] = adjust[2];
	adjust[2] = temp;
	
	Matrix = GetProjectionMatrix(adjust,adjusted,debug);
	
	/*PrintMatrix(Matrix,3,3);*/

	//if(SSD(adjusted,adjust,Matrix,T))
	//{ 
		Vertex = new Coordinate [m_numFeature];
		for(int i = 0;i < m_numFeature;i++)Vertex[i] = adjusted[i];
		ProjectionTrsform(src,dst,Matrix,debug);
	/*}*/

	cvReleaseMat(&Matrix);
}
//Rearrange Coordinate if u don't know where the points are,and there function will rearrange ur point 從右下到右上以順時鐘轉動
void   Morphining::ReArrangeCoord(Coordinate *coord)
{
	//No change vector,the vectors just act as sort points

	Coordinate *Points = new Coordinate [m_numFeature];
	Vector2D   *vector = new Vector2D   [m_numFeature];
	int		   *index  = new int		[m_numFeature];//sort of degree of new coordinate system


	//Reconstruct the coordinate system
	//Make average of all coord point serving as origin 
    float new_origin_x = 0,new_origin_y = 0.;

	for(int i = 0;i < m_numFeature;i++)
	{
		  new_origin_x += coord[i].x/(m_numFeature)*1.0;
		  new_origin_y += coord[i].y/(m_numFeature)*1.0;
	}
	//construct vector from new origin
	for(int i = 0;i < m_numFeature;i++)
	{
		vector[i].x = coord[i].x - new_origin_x;
		vector[i].y = coord[i].y - new_origin_y;
		 index[i]   = i;
	}
	
	/*Sort of coordinate by using new coordinate system degree to locate corresponding position*/
	for(int j = 0;j < m_numFeature;j++)
	{
		for(int i = j + 1;i < m_numFeature;i++)
		{
			float theta_1 = vector[index[j]].Gettheta();
			float theta_2 = vector[index[i]].Gettheta();
			
			if(theta_1 > theta_2)
			{
				int temp_x = coord[i].x;
				int temp_y = coord[i].y;
				coord[i].x = coord[j].x;
				coord[i].y = coord[j].y;
				coord[j].x = temp_x;
				coord[j].y = temp_y;

				int temp = index[j];
				index[j] = index[i];
				index[i] = temp;
			}
		}
	}
	printf("Finish to coord arrangement\n");
	//index[i]:角度從小排到大 ,i = 0 角度最小,i = num_pair-1角度最大find cutting line
	for(int i = 0;i < m_numFeature;i++)
	{
		printf("i = %d <%d,%d> theta = %f\n",i,coord[i].x,coord[i].y,vector[index[i]].Gettheta());
	}
	
	delete [] Points;
	delete [] vector;
	delete [] index;
}

CvMat *Morphining::GetAffineMatrix(vector<Coordinate> coord1,vector<Coordinate>coord2,bool debug)
{
	//b_temp:affine transformation,b_temp_inv:inverse of affine transform
	CvMat *b_temp	  = cvCreateMat(3, 3, CV_32FC1);
	CvMat *b_temp_inv = cvCreateMat(3, 3, CV_32FC1);
	CvMat *B		  = cvCreateMat(3, 3, CV_32FC1);
	CvMat *X		  = cvCreateMat(3, 3, CV_32FC1);
	CvMat *X_Inv	  = cvCreateMat(3, 3, CV_32FC1);
     // x1' = w11 * x1 + w12 * y1 + w13
     // y1' = w21 * x1 + w22 * y1 + w13
     // x2' = w11 * x2 + w12 * y2 + w13
     // y2' = w21 * x2 + w22 * y2 + w13
	 // x3' = w11 * x3 + w12 * y3 + w13
     // y3' = w21 * x3 + w22 * y3 + w13
	 // Ax = b  x:left or right b:front face 

	for(int i = 0;i < 3;i++)
	{
		cvSetReal2D(B, 0, i, coord1[i].x);
		cvSetReal2D(B, 1, i, coord1[i].y);
		cvSetReal2D(B, 2, i, 1);
	}

	for(int i = 0;i < 3;i++)
	{
		cvSetReal2D(X, 0 , i, coord2[i].x);
		cvSetReal2D(X, 1 , i, coord2[i].y);
		cvSetReal2D(X, 2 , i, 1);
	}

	cvInv(X, X_Inv);
	cvmMul(B,X_Inv,b_temp);
	cvInv(b_temp,b_temp_inv);
	if(debug)
	{
		cout << "b_temp:\n";
		PrintMatrix(b_temp,3,3);
		cout << "b_temp_inv:\n";
		PrintMatrix(b_temp_inv,3,3);
	}
	cvReleaseMat(&B);
	cvReleaseMat(&X);
	cvReleaseMat(&X_Inv);
	cvReleaseMat(&b_temp);
	//return b_temp;
	return b_temp_inv;
}

void   Morphining::AffineTrsform(IplImage *src,IplImage *dst,CvMat *Matrix,int type,bool debug)
{
	int width = src->width,height = src->height;

	bool in_region = false;
		for(int j = 0; j < height;j++)
		{
			for(int i = 0 ; i < width;i++)
			{
				if(type == 0)
				{
					in_region = (j - m[1]*i -b[1]) <= 0  && (j - m[0]*i -b[0]) >= 0;
				}
				else
				{
					in_region = Point_in_Polygon(Vertex,i,j,m_numFeature);
				}
				if(in_region)
				{
					int x  = i*cvGetReal2D(Matrix,0,0) + j*cvGetReal2D(Matrix,0,1) + cvGetReal2D(Matrix,0,2)*1.0 + 0.5;
					int y  = i*cvGetReal2D(Matrix,1,0) + j*cvGetReal2D(Matrix,1,1) + cvGetReal2D(Matrix,1,2)*1.0 + 0.5;
					
					if(x >= 0 && y >= 0 && x < width && y < height)
					{
						double pixel = cvGetReal2D(src,y,x);
						cvSetReal2D(dst,j,i,pixel);
					}
				}
			}
		}

	if(m_debug)
	{
		cvNamedWindow("morphine",0);
		cvShowImage("morphine",dst);
		cvWaitKey(0);
	}
}

void   Morphining::DoAffineTrsform(IplImage *src,IplImage *dst,vector<Coordinate> coord1,vector<Coordinate> coord2,double T,int type,bool debug)//T:Threshold of SSD's error;
{
	CvMat* Matrix = cvCreateMat(3,3,CV_32FC1);
	Matrix = GetAffineMatrix(coord1,coord2,debug);

	//if(SSD(coord1,coord2,Matrix,100))
	//{ 
		Vertex = new Coordinate [m_numFeature];
		for(int i = 0;i < m_numFeature;i++)
		{
			Vertex[i] = coord1[i];
		}
		Graphic FindLine;
		FindLine.CuttingRegion(coord1,m_numFeature,m,b);	
        AffineTrsform(src,dst,Matrix,type,debug);
		if(debug)
		{
			cvNamedWindow("dst",0);
			cvShowImage("dst",dst);
			cvWaitKey(0);
		}
	//}
		
	cvReleaseMat(&Matrix);	   
}

bool   Morphining::Point_in_Polygon(Coordinate *v,int x,int y,int numVertex)
{
    bool c = false;

    for (int i = 0, j = numVertex-1; i < numVertex; j = i++)
	{
        if ((v[i].y > y) != (v[j].y > y) &&
            x < (v[j].x - v[i].x)*(y-v[i].y)/((v[j].y-v[i].y)*1.0)+v[i].x)
            c = !c;
	}
	/*int j = numVertex - 1;
	for(int i = 0; i < numVertex;i++)
	{
		if((v[i].y < y &&  v[j].y >= y
		||  v[j].y < y &&  v[i].y >= y)
		&& (v[i].x <= x || v[j].x <= x))
		{
			bool b = v[i].x + (y - v[i].y)/((v[j].y - v[i].y)*1.0)*(v[j].x - v[i].x) < x;
			c ^= b;
		}
		j = i;
	}*/
	
    return c;
}

void   Morphining::RearrangeCoord(Coordinate *srcCoord)//arrange
{
	//int numFeature = 12;
	Coordinate new_origin;
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
	
	for(int i = 0 ; i <  12/2;i++)
	{

		srcCoord[i]   = leftCoord[i];
		srcCoord[i+6] = rightCoord[i];

	}
}

void   Morphining::ShowFeature(Coordinate* featureCoord)
{
	IplImage   *dst = cvCreateImage(cvSize(m_width,m_height),8,1);
	cvZero(dst);

	for(int num = 0;num < m_numFeature;num++)
	{
		for(int y = 0;y < 5;y++)
		{
			for(int x = 0;x < 5;x++)
			{
				if(y+featureCoord[num].y < dst->height && x+featureCoord[num].x < dst->width)
				dst->imageData[(y+featureCoord[num].y)*m_width+(x+featureCoord[num].x)] = 255;
			}
		}

		cvNamedWindow("Feature's coordinate",0);
		cvShowImage("Feature's coordinate",dst);
		cvWaitKey(0);

	}
	cvZero(dst);
}

