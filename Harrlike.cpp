#include "Harrlike.h"
//Cascade_Name:load Harrlike feature of face,ears,eyes,etc
//Do_pyramids:smallize origin image to accerlate excuting harrlike
Harrlike::Harrlike(IplImage *img,Coordinate *adjustedCoord,
				   String Cascade_Name,int sizeX,int sizeY,int Do_pyramids,int color)
{
	m_srcImage			=	cvCloneImage(img);
	m_OriginalImage		=	cvCreateImage(cvGetSize(m_srcImage),8,1);
	m_normalizeImage	=	cvCreateImage(cvSize(sizeX,sizeY),8,1);
	cvCopy(m_srcImage,m_OriginalImage);
	
	cascade   = (CvHaarClassifierCascade*)cvLoad( Cascade_Name.c_str() );
	m_PyrDown = Do_pyramids;
	m_Color   = color;
	m_justSet = true;
	scale     = true;
	m_do      = false;
	m_width   = sizeX;
	m_height  = sizeY;

	for(int i = 0;i < 12;i++)
	{
		m_TraningCoord[i] = adjustedCoord[i];
	}
	if(!(CvHaarClassifierCascade*) cascade)
	{
		cout<<"ERROR: Could not load classifier cascade."<<endl;
		m_justSet = 0;
	}
}

Harrlike::Harrlike(const char *filename,
				   String Cascade_Name,int Do_pyramids,int color)

{
	m_srcImage = cvLoadImage(filename);
	m_OriginalImage = cvCreateImage(cvGetSize(m_srcImage),8,3);
	cvCopy(m_srcImage,m_OriginalImage);
	cascade = (CvHaarClassifierCascade*)cvLoad( Cascade_Name.c_str() );

	m_PyrDown = Do_pyramids;
	m_Color   = color;
	m_justSet = 1;
	scale   = 1;
}

Harrlike::~Harrlike()
{
	cvReleaseHaarClassifierCascade( &cascade );
	cvReleaseImage( &m_OriginalImage );
}

IplImage* Harrlike::DetectSaveROI()
{
	if(m_justSet)
	{
		IplImage* small_image;
		CvMemStorage* storage = cvCreateMemStorage(0);//default:64k byte
		CvSeq* faces = NULL;

		/*Smallize to boost the speed of detection*/
		if( m_PyrDown )
		{
			if(m_srcImage->nChannels == 1)
			small_image = cvCreateImage( cvSize(m_srcImage->width/2,m_srcImage->height/2), IPL_DEPTH_8U, 1 );
			else
			small_image = cvCreateImage( cvSize(m_srcImage->width/2,m_srcImage->height/2), IPL_DEPTH_8U, 3 );

			cvPyrDown( m_srcImage, small_image, CV_GAUSSIAN_5x5 );
			scale = 2;
		}

		else small_image = m_srcImage;

		faces = cvHaarDetectObjects( small_image, cascade, storage, 1.2, 2 , CV_HAAR_DO_CANNY_PRUNING );

		/* Draw all faces with rectangle */
		if(faces!=NULL)
		{
			Draw_Rectangle(faces,scale);

			SaveROI(m_OriginalImage,faces);

			if( small_image != m_srcImage )cvReleaseImage( &small_image );
			
			cvClearSeq(faces);
			cvReleaseImage( &small_image );
			cvReleaseMemStorage( &storage );
			return m_normalizeImage;
		}
		else
		{
			printf("m_Number %d can't be foundd!\n");
			return NULL;
		}
		/*cvNamedWindow( "test", 0 );
			cvShowImage( "test", m_srcImage );
			cvSaveImage("Face_Detect1.jpg",m_srcImage); 
			cvWaitKey(0);*/
	}
	return NULL;
}

void Harrlike::Draw_Rectangle(CvSeq *faces,double scale)
{
	for( int i= 0; i < faces->total; i++ )
    {
        CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i );

		if(m_Color == 0)//b
        cvRectangle( m_srcImage, cvPoint(face_rect.x*scale*0.75,   //start point
											 face_rect.y*scale),
							     cvPoint((face_rect.x+face_rect.width)*scale*1.2,
										 (face_rect.y+face_rect.height)*scale),//end point
							     cvScalar(255,0,0), 3 );
		else if(m_Color == 1)//g
        cvRectangle( m_srcImage, cvPoint(face_rect.x*scale*0.8,
											face_rect.y*scale),
							     cvPoint((face_rect.x+face_rect.width)*scale*1.2,
									     (face_rect.y+face_rect.height)*scale),
								 cvScalar(0,255,0), 3 );
		else if(m_Color == 2)//r
        cvRectangle( m_srcImage, cvPoint(face_rect.x*scale*0.8,
										 face_rect.y*scale),
								 cvPoint((face_rect.x+face_rect.width)*scale*1.2,
									     (face_rect.y+face_rect.height)*scale),
								 cvScalar(0,0,255), 3 );
    }

}
//Show ROI Image,and Save ROI Image and it's coordinates
void Harrlike::SaveROI(IplImage *Img,CvSeq *faces)
{
	CvSize new_size = cvSize(m_width,m_height);
	CvRect ROI;
	double  Ratio = Img->width/1000.;
	double  m,angle;
	int ratioX = 2,ratioY = 2;
	
	//Rotate
	if((m_TraningCoord[7].x - m_TraningCoord[1].x)!=0)
	{
	   m = static_cast<double>(m_TraningCoord[7].y - m_TraningCoord[1].y)/(m_TraningCoord[7].x - m_TraningCoord[1].x);
	   angle = atan(m)*57.25972;
	}
	else
	{
		angle = 0;
	}
	CvPoint2D32f  src_center  = cvPoint2D32f(m_TraningCoord[1].x*1.0,  m_TraningCoord[1].y*1.0);    
	
	float map[6];
	CvMat rot_mat = cvMat( 2, 3, CV_32F,map);
	cv2DRotationMatrix( src_center, angle,1.0, &rot_mat);  
	IplImage* dst = cvCreateImage(cvSize(Img->width,Img->height),8,1);
    cvWarpAffine(Img, dst, &rot_mat);
	//cvShowImage("2",dst);
	//cvWaitKey(0);
	double Coord3X = m_TraningCoord[3].x*cvGetReal2D(&rot_mat,0,0) + m_TraningCoord[3].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord3Y = m_TraningCoord[3].x*cvGetReal2D(&rot_mat,1,0) + m_TraningCoord[3].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord9X = m_TraningCoord[9].x*cvGetReal2D(&rot_mat,0,0) + m_TraningCoord[9].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord9Y = m_TraningCoord[9].x*cvGetReal2D(&rot_mat,1,0) + m_TraningCoord[9].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;
	double Coord1X = m_TraningCoord[1].x*cvGetReal2D(&rot_mat,0,0) + m_TraningCoord[1].y*cvGetReal2D(&rot_mat,0,1) + cvGetReal2D(&rot_mat,0,2)*1.0;
	double Coord0Y = m_TraningCoord[0].x*cvGetReal2D(&rot_mat,1,0) + m_TraningCoord[0].y*cvGetReal2D(&rot_mat,1,1) + cvGetReal2D(&rot_mat,1,2)*1.0;

	//鼻子中間

	double middleX = (Coord3X + Coord9X)*0.5;
	double middleY = (Coord3Y + Coord9Y)*0.5;
	//取人臉出來
	ROI.x = Coord1X;
	ROI.y = Coord0Y;
	ROI.width  = (middleX-ROI.x)*ratioX;//600
	ROI.height = (middleY - Coord0Y)*ratioY;//m_TraningCoord[11].y-m_TraningCoord[0].y
	
	cvSetImageROI(dst, ROI);
	ResizeImage(dst,new_size);
	
	/*double multipleX = (m_width*1.0/ROI.width*1.0);
	double multipleY = (m_height*1.0/ROI.height*1.0);

	FILE *NormalizeCoord = fopen("..\\DataBase\\NormalizeCoord.xls","a");
	for(int i = 0;i < 12;i++)
	{
	int normalizeX = (m_TraningCoord[i].x - ROI.x)*multipleX;
	int normalizeY = (m_TraningCoord[i].y - ROI.y)*multipleY;
	if(normalizeX > m_width) normalizeX = m_width;
	if(normalizeY > m_height) normalizeY = m_height;
	if(normalizeX < 0)   normalizeX = 0;
	if(normalizeY < 0)   normalizeY = 0;
	fprintf(NormalizeCoord,"%d\t%d\t",normalizeX,normalizeY);
	}
	fprintf(NormalizeCoord,"\n");
	fclose(NormalizeCoord);*/

	FILE *Coord = fopen("..\\DataBase\\Coordinate.xls","a");
	for(int i = 0;i < 12;i++)
	{
		fprintf(Coord,"%d\t%d\t",m_TraningCoord[i].x,m_TraningCoord[i].y);
	}
	fprintf(Coord,"\n");
	fclose(Coord);
	//cvReleaseImage(&Img);
	//cvSaveImage(filename,Img);
	//cvShowImage("roi",Img);
	//cvWaitKey(0);
}

void Harrlike::ResizeImage(IplImage* img,CvSize new_size)
{
	IplImage *GrayImage = cvCreateImage(cvGetSize(img),8,1);
	char *Newfilename = new char [150];

	if(img->nChannels == 3)
	cvCvtColor(img,GrayImage,CV_BGR2GRAY);
	else
	cvCopy(img,GrayImage);
	/*縮小和儲存圖片*/
	cvResize(GrayImage,m_normalizeImage,3);

	cvReleaseImage(&img);
	cvReleaseImage(&GrayImage);
	delete [] Newfilename;
}

IplImage* Harrlike::Translation(IplImage *Img,int x , int y)
{
	int dx,dy;
	int width = Img->width,height = Img->height;
	IplImage* Image = cvCreateImage(cvSize(width,height),8,1);
	cvZero(Image);

	for(int j = 0;j < height ; j++)
	{
		for(int i = 0;i < width ; i++)
		{
			dy = j-y;
			dx = i-x;
			if( dy > -1 && dx > -1 && dy < height && dx < width )	
			{
				Image->imageData[ dy*width + dx] = Img->imageData[j*width+i];
			}
		}
	}
	/*cvNamedWindow("center",0);
	cvShowImage("center",Image);
	cvWaitKey(0);*/
	return Image;
}

