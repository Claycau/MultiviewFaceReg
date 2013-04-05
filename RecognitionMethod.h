#include <iostream>

using namespace std;
typedef int i_person;

class RecognitionMethod
{
	public:
		RecognitionMethod(char* filename,int numRegister,int numSample,double threshold);
		~RecognitionMethod();
		i_person SelectBig(int pcaDim);
		i_person KTree(int seletK);
		
		struct Similarity
		{
			int person;
		    int tp,tn, fp,fn;
			double similarityRate;
		};
	private:
		void BubbleSort();
	public:
		double m_threshold;
	private:
		int m_numSample;
		int m_numRegister;
		char *m_filename;
		float *m_recognitionRate;
		float *m_sort;
		float m_Precision;
		float m_Recall;
		Similarity max;
};