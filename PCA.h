#include <windows.h>
#include <vector>
/* 
	1. ��J��ƽШ̷ӤU�C�榡
			s1f1	s1f2	s1f3	s1f4	s1f5	s1f6
			s2f1	s2f2	s2f3	s2f4	s2f5	s2f6	// s1f5	���sample 1��5th feature, s2f1 ���sample 2��1th feature

	�ϥάy�{:
		Step1:	�p��mean
		Step2:	�p��Covariance matrix
		Step3:	�p��EigenValue and EigenVector
		Step4:	�p���v�Ŷ�

	�ϥνd��:		
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
								
								// ��ܥثematrix��T
				bool			ShowMatrixInf();
				bool			CalcMean(void);
				bool			CalcCoVarianceMatrix(void);
				bool			CalcEigenValueAndVector(void);
								// ��v��NumOfProjectionPlanes���פ��Ŷ�(default=3)
				bool			CalcProjectionSpace(int NumOfProjectionPlanes=3);	
								// ��Eigenvalue�j�p���s�ƦCEigenvalue��Eigenvector
				bool			RearrangeEigenvalueAndVector();

								// ���ɮ�LoadMatrix
				bool			LoadMatrix(char FileName[], int NumOfFeature, int NumOfSample);
								// �ѯx�}LoadMatrix
				bool			LoadMatrix(double *MatrixData,int NumOfFeature,int NumOfSample);

								// �x�sEigenvalue�eN�j�ҹ�����Eigenvector
				bool			SaveNPrinEigenvector(char name_File[],int NumberOfPrinEigenvector);

								// ���oEigenspace�ܥ~��
				bool			GetEigenspace(std::vector<double> EigenspaceData);

								CPCA(double *MatrixData=NULL,int NumOfFeature=0,int NumOfSample=0);	// �غc��
	virtual						~CPCA(void);	// �Ѻc��											

private:

				int				m_NumOfFeature;	// �C1�˥��㦳��Feature�ƶq
				int				m_NumOfSample;	// ��J���˥��ƶq
				int				m_NumOfProjectionPlanes;	// ��v�ܦh�ִX���Ŷ��W
				//double		**	m_Matrix;	
				//double		*	m_MatrixData;
				//double		*	m_Mean;	// ��J�˥����C��feature��mean
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