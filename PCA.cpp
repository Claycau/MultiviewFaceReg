#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
//#include "stdafx.h"
#include "PCA.h"

#include "highgui.h"
#include "cv.h"

// 建構式
CPCA::CPCA(double *MatrixData,int NumOfFeature,int NumOfSample){

	// 維度
	m_NumOfFeature	=	NumOfFeature;
	m_NumOfSample	=	NumOfSample;

	// 建立matrix空間並賦值
	m_MatrixData = std::vector<double> (m_NumOfFeature*m_NumOfSample);
	m_Matrix	 = std::vector<std::vector<double>> (m_NumOfSample,m_NumOfFeature);

	for(int i=0;i<m_NumOfSample;i++){
		for(int j=0;j<m_NumOfFeature;j++){
			m_Matrix[i][j]=*(MatrixData++);
		}
	}
}
// 解構式
CPCA::~CPCA(){

}
// 由檔案LoadMatrix
bool CPCA::LoadMatrix(char FileName[],int NumOfFeature,int NumOfSample){

	m_NumOfFeature  =	NumOfFeature;
	m_NumOfSample	=	NumOfSample;

	// load file
	if(m_NumOfFeature!=0 && m_NumOfSample!=0){
		
		m_MatrixData	=	std::vector<double>	(m_NumOfFeature*m_NumOfSample);
		m_Matrix		=	std::vector<std::vector<double>> (m_NumOfSample,m_NumOfFeature);

		FILE	*fn=fopen(FileName,"r");
		for(int i=0;i<m_NumOfSample;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				fscanf(fn,"%lf",&m_Matrix[i][j]);
			}
		}
		fclose(fn);

		return true;
	}else{
		return false;
	}
}

// 由矩陣LoadMatrix
bool CPCA::LoadMatrix(double *MatrixData,int NumOfFeature,int NumOfSample){

	m_NumOfFeature	=	NumOfFeature;
	m_NumOfSample	=	NumOfSample;

	// load matrix
	if(m_NumOfFeature!=0	&&	m_NumOfSample!=0){
		
		m_MatrixData	=	std::vector<double>	(m_NumOfFeature*m_NumOfSample);
		m_Matrix		=	std::vector<std::vector<double>> (m_NumOfSample,m_NumOfFeature);
		
		for(int i = 0;i < m_NumOfSample;i++){
			for(int j = 0;j < m_NumOfFeature;j++){ 
				m_Matrix[i][j] = MatrixData[i*m_NumOfFeature+j];
			}
		}

		return true;
	}else{
		return false;
	}
}
// 儲存Eigenvalue前N大所對應的Eigenvector
bool CPCA::SaveNPrinEigenvector(char name_File[],int NumberOfPrinEigenvector){

	if(m_EigenSpace.size() == 0){
		printf("Error!!\tm_EigenSpace==NULL\n");
		return false;
	}
	else{
		// 設定上限
		if(NumberOfPrinEigenvector > m_NumOfFeature){
			NumberOfPrinEigenvector = m_NumOfFeature;
		}

		FILE *fn = fopen(name_File,"w");
		for(int i = 0;i < NumberOfPrinEigenvector;i++){
			for(int j = 0;j < m_NumOfFeature;j++){
				fprintf(fn,"%lf\t",m_EigenSpace[j][i]);
			}
			fprintf(fn,"\n");
		}
		fclose(fn);
	
		return true;
	}
}
// 取得Eigenspace至外部
bool CPCA::GetEigenspace(std::vector<double> EigenspaceData){

	if(m_EigenSpace.size() == 0){
		printf("Error!!\tm_EigenSpace==NULL\n");
		return false;
	}else{
		for(int i=0;i < m_NumOfFeature;i++){
			for(int j = 0;j < m_NumOfFeature;j++){
				EigenspaceData[i*m_NumOfFeature + j] = m_EigenSpace[j][i];
			}
		}
	}

	return true;
}
// 顯示目前matrix資訊
bool CPCA::ShowMatrixInf(){
/*
	printf("顯示m_Matrix資訊:\n");
	for(int i=0;i<m_NumOfSample;i++){
		printf("Sample %d:\t",i);
		for(int j=0;j<m_NumOfFeature;j++){
			printf("%lf\t",m_Matrix[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	if(m_Mean!=NULL){
		printf("顯示m_Mean資訊:\n");
		for(int i=0;i<m_NumOfFeature;i++){
			printf("%lf\t",m_Mean[i]);
		}
		printf("\n");
	}

	printf("\n");

	if(m_CoVarianceMatrix!=NULL){
		printf("顯示m_CoVarianceMatrix資訊:\n");
		for(int i=0;i<m_NumOfFeature;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				printf("%lf\t",m_CoVarianceMatrix[i][j]);
			}
			printf("\n");
		}
	}

	printf("\n");

	if(m_EigenValue!=NULL){
		printf("顯示m_EigenValue資訊:\n");
		for(int i=0;i<m_NumOfFeature;i++){
			printf("第%d組:\t%lf\n",i,m_EigenValue[i]);
		}
		printf("\n");
	}

	printf("\n");

	if(m_EigenSpace!=NULL){
		printf("顯示m_EigenVectors資訊:\n");
		for(int i=0;i<m_NumOfFeature;i++){
			printf("第%d組:\t",i);
			for(int j=0;j<m_NumOfFeature;j++){
				printf("%lf\t",m_EigenSpace[j][i]);
			}
			printf("\n");
		}
	}

	printf("\n");

	if(m_ProjectionSpace!=NULL){
		printf("顯示m_ProjectionSpace資訊(Projection on first %d prin. comps):\n",(m_NumOfFeature<m_NumOfProjectionPlanes?m_NumOfFeature:m_NumOfProjectionPlanes));
		for(int i=0;i<m_NumOfSample;i++){
			printf("Sample %d:\t",i);
			for(int j=0;j<(m_NumOfFeature<m_NumOfProjectionPlanes?m_NumOfFeature:m_NumOfProjectionPlanes);j++){
				printf("%lf\t",m_ProjectionSpace[i][j]);
			}
			printf("\n");
		}
	}*/
	
	//if(m_Matrix!=NULL){
	//	FILE *fm_Matrix=fopen("information\\m_Matrix.xls","w");
	//	for(int i=0;i<m_NumOfSample;i++){
	//		for(int j=0;j<m_NumOfFeature;j++){
	//			fprintf(fm_Matrix,"%d\t",(int)m_Matrix[i][j]);
	//		}
	//		fprintf(fm_Matrix,"\n");
	//	}
	//	fclose(fm_Matrix);
	//}

	if(m_Mean.size() > 0){
		FILE *fm_Mean = fopen("..\\DataBase\\m_Mean.xls","a");
		for(int i=0;i < m_NumOfFeature;i++){
			fprintf(fm_Mean,"%lf\t",m_Mean[i]);
		}
		fprintf(fm_Mean,"\n");
		fclose(fm_Mean);
	}

	/*if(m_CoVarianceMatrix!=NULL){
		FILE *fm_CoVariance=fopen("information\\m_CoVarianceMatrix.xls","w");
		for(int i=0;i<m_NumOfFeature;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				fprintf(fm_CoVariance,"%lf\t",m_CoVarianceMatrix[i][j]);
			}
			fprintf(fm_CoVariance,"\n");
		}
		fclose(fm_CoVariance);
	}*/

	/*if(m_EigenValue!=NULL){
		FILE *fm_EigenValue=fopen("information\\m_Eigenvalue.xls","w");
		for(int x=0;x<m_NumOfFeature;x++){
			fprintf(fm_EigenValue,"%lf\t",m_EigenValue[x]);
		} 
		fprintf(fm_EigenValue,"\t");
		fclose(fm_EigenValue);
	}*/

	if(m_EigenSpace.size() > 0){
		FILE *fm_EigenSpace=fopen("..\\DataBase\\m_EigenSpace.xls","a");
		for(int x=0;x<m_NumOfFeature;x++){
			for(int y=0;y<m_NumOfProjectionPlanes;y++){
				fprintf(fm_EigenSpace,"%lf\t",m_EigenSpace[x][y]);
			}
			fprintf(fm_EigenSpace,"\n");
		}
		//fprintf(fm_EigenSpace,"\n");
		fclose(fm_EigenSpace);
	}

	//每個影像投影的值
	if(m_ProjectionSpace.size() > 0){
		FILE *fm_ProjectionSpace=fopen("..\\DataBase\\m_ProjectionSpace.xls","a");
		for(int i=0;i<m_NumOfSample;i++){
			for(int j=0;j<(m_NumOfFeature<m_NumOfProjectionPlanes?m_NumOfFeature:m_NumOfProjectionPlanes);j++){
				fprintf(fm_ProjectionSpace,"%lf\t",m_ProjectionSpace[i][j]);
			}
			fprintf(fm_ProjectionSpace,"\n");
		}
		fclose(fm_ProjectionSpace);
	}

	return true;
}
// 計算mean
bool CPCA::CalcMean(void){

	if(m_Matrix.size() == 0){
		printf("Error!!\tm_Matrix==NULL\n");
		return false;
	}else{
		// 建立Mean空間
		m_Mean	=	std::vector<double>(m_NumOfFeature);

		// 計算mean
		for(int i=0;i<m_NumOfFeature;i++){
			for(int j=0;j<m_NumOfSample;j++){
				m_Mean[i]+=m_Matrix[j][i];
			}
			m_Mean[i]/=(double)m_NumOfSample;
		}
		return true;
	}
}
// 計算CoVariance
bool CPCA::CalcCoVarianceMatrix(void){

	if(m_Mean.size() == 0){
		printf("Error!!\tm_Mean==NULL\n");
		return false;
	}else{

		m_CoVarianceMatrixData	=	std::vector<double>(m_NumOfFeature*m_NumOfFeature);
		m_CoVarianceMatrix		=	std::vector<std::vector<double>>(m_NumOfFeature,m_NumOfFeature);

		// 求Variance矩陣
		double	*	VarianceMatrixData	=	new	double		[m_NumOfSample*m_NumOfFeature];
		double	**	VarianceMatrix		=	new	double	*	[m_NumOfSample];
		for(int i=0;i<m_NumOfSample;i++){
			VarianceMatrix[i]=&VarianceMatrixData[i*m_NumOfFeature];
		}
		for(int i=0;i<m_NumOfSample;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				VarianceMatrix[i][j]  = m_Matrix[i][j];
				VarianceMatrix[i][j] -= m_Mean[j];
			}
		}

		// 計算CoVariance矩陣
		for(int x=0;x<m_NumOfFeature;x++){
			for(int y=x;y<m_NumOfFeature;y++){
				m_CoVarianceMatrix[x][y]=0.;
				for(int i=0;i<m_NumOfSample;i++){
					m_CoVarianceMatrix[x][y]+=VarianceMatrix[i][x]*VarianceMatrix[i][y];
				}
				m_CoVarianceMatrix[y][x]=m_CoVarianceMatrix[x][y];
			}
		}

		// 取平均
		for(int i=0;i<m_NumOfFeature;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				m_CoVarianceMatrix[i][j]/=m_NumOfSample;
			}
		}
		return true;
	}
}
// 計算EigenValue
bool CPCA::CalcEigenValueAndVector(void){

	if(m_CoVarianceMatrix.size() == 0){
		printf("Error!!\tm_CoVarianceMatrix==NULL\n");
		return false;
	}else if(m_NumOfFeature==0){
		printf("Error!!\tm_NumOfFeature==0\n");
		return false;
	}else{
		
		// 建立EigenValue的空間
		m_EigenValue	=	std::vector<double> (m_NumOfFeature);

		// 建立EigenSpace的空間並賦值

		m_EigenSpaceData	=	std::vector<double>		(m_NumOfFeature*m_NumOfFeature);
		m_EigenSpace		=	std::vector<std::vector<double>> (m_NumOfFeature,m_NumOfFeature);

		for(int i=0;i<m_NumOfFeature;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				m_EigenSpace[i][j]=m_CoVarianceMatrix[i][j];
			}
		}
		// 建立Intermediate空間
		std::vector<double>Intermediate(m_NumOfFeature);

		// = = = = = Triangular decomposition = = = = = //
		// Householder reduction of matrix a to tridiagonal form.
		// Algorithm: Martin et al., Num. Math. 11, 181-195, 1968.
		// Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
		// Springer-Verlag, 1976, pp. 489-494.
		// W H Press et al., Numerical Recipes in C, Cambridge U P,
		// 1988, pp. 373-374.
		for(int i=m_NumOfFeature-1;i>0;i--){
			int		l		=i-1;
			double	h		=0.;
			if(l>0){
				double	scale	=0.;
				for(int k=0;k<=l;k++){
					scale+=fabs(m_EigenSpace[i][k]);
				}
				if(scale==0.){
					Intermediate[i]=m_EigenSpace[i][l];
				}else{
					for(int k=0;k<=l;k++){
						m_EigenSpace[i][k]/=scale;
						h+=m_EigenSpace[i][k]*m_EigenSpace[i][k];
					}
					double	f=m_EigenSpace[i][l];
					double	g=f>0?-sqrt(h):sqrt(h);
					Intermediate[i]=scale*g;
					h-=f*g;
					m_EigenSpace[i][l]=f-g;
					f=0.;
					for(int j=0;j<=l;j++){
						m_EigenSpace[j][i]=m_EigenSpace[i][j]/h;
						g=0.;
						for(int k=0;k<=j;k++){
							g+=m_EigenSpace[j][k]*m_EigenSpace[i][k];
						}
						for(int k=j+1;k<=l;k++){
							g+=m_EigenSpace[k][j]*m_EigenSpace[i][k];
						}
						Intermediate[j]=g/h;
						f+=Intermediate[j]*m_EigenSpace[i][j];
					}
					double	hh=f/(h+h);
					for(int j=0;j<=l;j++){
						f=m_EigenSpace[i][j];
						Intermediate[j]=Intermediate[j]-hh*f;
						g=Intermediate[j];
						for(int k=0;k<=j;k++){
							m_EigenSpace[j][k]-=(f*Intermediate[k]+g*m_EigenSpace[i][k]);
						}
					}
				}
			}else{
				Intermediate[i]=m_EigenSpace[i][l];
			}
			m_EigenValue[i]=h;
		}
		m_EigenValue[0]=0.;
		Intermediate[0]=0.;
		for(int i=0;i<m_NumOfFeature;i++){
			int l=i-1;
			if(m_EigenValue[i]){
				for(int j=0;j<=l;j++){
					double	g=0.;
					for(int k=0;k<=l;k++){
						g+=m_EigenSpace[i][k]*m_EigenSpace[k][j];
					}
					for(int k=0;k<=l;k++){
						m_EigenSpace[k][j]-=g*m_EigenSpace[k][i];
					}
				}
			}
			m_EigenValue[i]=m_EigenSpace[i][i];
			m_EigenSpace[i][i]=1.;
			for(int j=0;j<=l;j++){
				m_EigenSpace[j][i]=0.;
				m_EigenSpace[i][j]=0.;
			}
		}


/*		printf("interm:\n");
		for(int i=0;i<m_NumOfFeature;i++){
			printf("%.8lf\t",Intermediate[i]);
		}
		printf("\n");

		printf("eigenvalue:\n");
		for(int i=0;i<m_NumOfFeature;i++){
			printf("%.2lf\t",m_EigenValue[i]);
		}
		printf("\n");

		printf("EigenSpace:\n");
		for(int i=0;i<m_NumOfFeature;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				printf("%.2lf\t",m_EigenSpace[i][j]);
			}
			printf("\n");
		}
		printf("\n");*/


		// = = = = = Tridiagonal QL algorithm -- Implicit = = = = = //
		int loopA=0;
		int loopB=0;
		int loopC=0;
		int loopD=0;
		int loopE=0;
		// loopA
		for(int i=1;i<m_NumOfFeature;i++){
			loopA++;
			Intermediate[i-1]=Intermediate[i];
		}
		Intermediate[m_NumOfFeature-1]=0.;
		// loopB
		for(int l=0;l<m_NumOfFeature;l++){
			loopB++;
			int		iter=0;
			double	dd;
			int		m;
			do{				
				// loopC
				for(m=l;m<m_NumOfFeature-1;m++){	// add =
					loopC++;
					dd=fabs(m_EigenValue[m])+fabs(m_EigenValue[m+1]);

					if((float)(fabs(Intermediate[m])+dd)==((float)dd)){ 
						break;
					}
				}
				if(m!=l){
					if(iter++==30){
						printf("Error!!\tNo convergence in TLQI.\n");
						system("pause");
						exit(0);
					}
					double	g	=(m_EigenValue[l+1]-m_EigenValue[l])/(2.*Intermediate[l]);
					double	r	=sqrt((g*g)+1.);
					g	=	m_EigenValue[m]-m_EigenValue[l]+Intermediate[l]/(g+(g<0?-fabs(r):fabs(r)));
					double	s=1.;
					double	c=1.;
					double	p=0.;
					// loopD
					for(int i=m-1;i>=l;i--){
						loopD++;
						double	f=s*Intermediate[i];
						double	b=c*Intermediate[i];
						if(fabs(f)>=fabs(g)){
							c=g/f;
							r=sqrt((c*c)+1.);
							Intermediate[i+1]=f*r;
							c*=(s=1./r);
						}else{
							s=f/g;
							r=sqrt((s*s)+1.);
							Intermediate[i+1]=g*r;
							c=1./r;
							s*=c;
						}
						g=m_EigenValue[i+1]-p;
						r=(m_EigenValue[i]-g)*s+2.*c*b;
						p=s*r;
						m_EigenValue[i+1]=g+p;
						g=c*r-b;
						// loopE
						for(int k=0;k<m_NumOfFeature;k++){
							loopE++;
							f=m_EigenSpace[k][i+1];
							m_EigenSpace[k][i+1]=s*m_EigenSpace[k][i]+c*f;
							m_EigenSpace[k][i]=c*m_EigenSpace[k][i]-s*f;
						}
					}
					m_EigenValue[l]=m_EigenValue[l]-p;
					Intermediate[l]=g;
					Intermediate[m]=0.;
				}
			}while(m!=l);
		}

		return true;
	}
}
// 計算ProjectionSpace
bool CPCA::CalcProjectionSpace(int NumOfProjectionPlanes){

	if(m_Matrix.size() == 0){
		printf("Error!!\tm_Matrix==NULL\n");
		return false;
	}else if(m_Mean.size() == 0){
		printf("Error!!\tm_Mean==NULL\n");
		return false;
	}else if(m_EigenSpace.size() == 0){
		printf("Error!!\tm_EigenSpace==NULL\n");
		return false;
	}else{

		m_NumOfProjectionPlanes = NumOfProjectionPlanes;

		// 建立m_ProjectionSpace
		int sudoDim = m_NumOfFeature<NumOfProjectionPlanes?m_NumOfFeature:NumOfProjectionPlanes;
		m_ProjectionSpaceData	=	std::vector<double>	(m_NumOfSample*sudoDim);
		m_ProjectionSpace		=	std::vector<std::vector<double>>(m_NumOfSample,sudoDim);

		// 建立intermediate空間
		std::vector<double>Intermediate(m_NumOfFeature);

		// 投影
		for(int i=0;i<m_NumOfSample;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				Intermediate[j]=m_Matrix[i][j]-m_Mean[j];
			}
			for(int j=0;j<(m_NumOfFeature<NumOfProjectionPlanes?m_NumOfFeature:NumOfProjectionPlanes);j++){
				for(int k=0;k<m_NumOfFeature;k++){
					m_ProjectionSpace[i][j]+=Intermediate[k]*m_EigenSpace[k][j];
				}
			}
		}

		return true;
	}
}
// 照Eigenvalue大小重新排列Eigenvalue及Eigenvector
bool CPCA::RearrangeEigenvalueAndVector(){

	if(m_EigenValue.size() == 0){
		printf("Error!!\tm_EigenValue==NULL\n");
		return false;
	}else if(m_EigenSpace.size() == 0){
		printf("Error!!\tm_EigenSpace==NULL\n");
		return false;
	}else{
		
		// 用來放sample編號的
		std::vector<int>tempEigenvalueNumber(m_NumOfFeature);
		for(int i=0;i<m_NumOfFeature;i++){
			tempEigenvalueNumber[i]=i;
		}
		// 變更Eigenvalue的順序
		for(int count=0;count<m_NumOfFeature;count++){
			for(int i=0;i<m_NumOfFeature-1;i++){
				if(m_EigenValue[i]<m_EigenValue[i+1]){
					double	temp=m_EigenValue[i];
					m_EigenValue[i]=m_EigenValue[i+1];
					m_EigenValue[i+1]=temp;
					int		temp1=tempEigenvalueNumber[i];
					tempEigenvalueNumber[i]=tempEigenvalueNumber[i+1];
					tempEigenvalueNumber[i+1]=temp1;
				}
			}
		}
	
		// 建立空間
		std::vector<double>PseudoEigenSpaceData(m_NumOfFeature*m_NumOfFeature);
		std::vector<std::vector<double>>PseudoEigenSpace(m_NumOfFeature,m_NumOfFeature);

		// 藉tempEigenvalueNumber取得新的Eigenvector
		for(int i=0;i<m_NumOfFeature;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				PseudoEigenSpace[j][i]=m_EigenSpace[j][tempEigenvalueNumber[i]];	
			}
		}
		// 複製
		for(int i=0;i<m_NumOfFeature;i++){
			for(int j=0;j<m_NumOfFeature;j++){
				m_EigenSpace[i][j]=PseudoEigenSpace[i][j];	
			}
		}
			
		return true;
	}
}