#include <windows.h>
#include <vector>
/* 
	1. 輸入資料請依照下列格式
			s1f1	s1f2	s1f3	s1f4	s1f5	s1f6
			s2f1	s2f2	s2f3	s2f4	s2f5	s2f6	// s1f5	表示sample 1的5th feature, s2f1 表示sample 2的1th feature

	使用流程:
		Step1:	計算mean
		Step2:	計算Covariance matrix
		Step3:	計算EigenValue and EigenVector
		Step4:	計算投影空間

	使用範例:		
		CPCA	M1;
		M1.LoadMatrix("test2.txt",6,8);

		M1.CalcMean();
		M1.CalcCoVarianceMatrix();
		M1.CalcEigenValueAndVector();
		M1.CalcProjectionSpace();
		M1.RearrangeEigenvalueAndVector();

		M1.ShowMatrixInf();		
*/


class CPCA{
							
public:
								
								// 顯示目前matrix資訊
				bool			ShowMatrixInf();
				bool			CalcMean(void);
				bool			CalcCoVarianceMatrix(void);
				bool			CalcEigenValueAndVector(void);
								// 投影到NumOfProjectionPlanes維度之空間(default=3)
				bool			CalcProjectionSpace(int NumOfProjectionPlanes=3);	
								// 照Eigenvalue大小重新排列Eigenvalue及Eigenvector
				bool			RearrangeEigenvalueAndVector();

								// 由檔案LoadMatrix
				bool			LoadMatrix(char FileName[], int NumOfFeature, int NumOfSample);
								// 由矩陣LoadMatrix
				bool			LoadMatrix(double *MatrixData,int NumOfFeature,int NumOfSample);

								// 儲存Eigenvalue前N大所對應的Eigenvector
				bool			SaveNPrinEigenvector(char name_File[],int NumberOfPrinEigenvector);

								// 取得Eigenspace至外部
				bool			GetEigenspace(std::vector<double> EigenspaceData);

								CPCA(double *MatrixData=NULL,int NumOfFeature=0,int NumOfSample=0);	// 建構式
	virtual						~CPCA(void);	// 解構式											

private:

				int				m_NumOfFeature;	// 每1樣本具有的Feature數量
				int				m_NumOfSample;	// 輸入的樣本數量
				int				m_NumOfProjectionPlanes;	// 投影至多少幾維空間上
				//double		**	m_Matrix;	
				//double		*	m_MatrixData;
				//double		*	m_Mean;	// 輸入樣本中每個feature的mean
				//double		**	m_CoVarianceMatrix;
				//double		*	m_CoVarianceMatrixData;
				//double		*	m_EigenValue;
				//double		**	m_EigenSpace;
				//double		*	m_EigenSpaceData;
				//double		**	m_ProjectionSpace;
				//double		*	m_ProjectionSpaceData;
				std::vector<std::vector<double>>	m_Matrix;
				std::vector<double>					m_MatrixData;
				std::vector<double>					m_Mean;
				std::vector<std::vector<double>>	m_CoVarianceMatrix;
				std::vector<double>					m_CoVarianceMatrixData;
				std::vector<double>					m_EigenValue;
				std::vector<std::vector<double>>	m_EigenSpace;
				std::vector<double>					m_EigenSpaceData;
				std::vector<std::vector<double>>	m_ProjectionSpace;
				std::vector<double>					m_ProjectionSpaceData;

};