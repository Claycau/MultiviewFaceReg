#include "RecognitionMethod.h"

RecognitionMethod::RecognitionMethod(char* filename,int numRegister,int numSample,double threshold)
{
	int length = strlen(filename);
	m_filename = new char[length];
	m_threshold		=   threshold;
	m_numSample		=	numSample;
	m_numRegister	=	numRegister;
	m_recognitionRate = new float [m_numSample*m_numRegister];
	strcpy(m_filename,filename);

	m_sort = NULL;
}

RecognitionMethod::~RecognitionMethod()
{
	//delete [] m_recognitionRate;
	//delete [] m_filename;
	/*if(m_sort!=NULL)
		delete m_sort;*/
}

i_person RecognitionMethod::SelectBig(int pcaDim)
{
	char buf[128];
	int member = -1;
	double threshold = m_threshold;
	static int pca  =  pcaDim;
	
	FILE *fdle = fopen(m_filename,"r");
	static int tp = 0,tn = 0,fp = 0, fn = 0;
	fscanf(fdle,"%s",buf);

	if(tp+tn+fp+fn == 20)
	{
		tp = tn = fp = fn = 0;
		pca++;
	}
	if(buf[0] == '#')
	{
		member = atoi(&buf[1]);
		fgets(buf,sizeof(buf),fdle); 
	}
	for(int i = 0;i < m_numRegister;i++)
	{
		for(int j = 0;j < m_numSample;j++)
		{
			fscanf(fdle,"%f",&m_recognitionRate[i*m_numSample+j]);
		}
	}
	fclose(fdle);

	fdle = fopen("..\\Database\\method_reulst.txt","a");
	max.similarityRate = 0;

	for(int i = 0;i < m_numRegister;i++)
	{
		for(int j = 0;j < m_numSample;j++)
		{
			if(max.similarityRate < m_recognitionRate[i*m_numSample+j])
			{
				max.similarityRate = m_recognitionRate[i*m_numSample+j];
				max.person  = i;
			}	
		}
	}

	if(m_threshold < max.similarityRate)
	{
		if(member < m_numRegister)
		{
			if(max.person == member)
			{
				tp++;
			}
			else
			{
				fp++;
			}
		}
		else
		{
			fp++;
		}
		fprintf(fdle,"%d %f\n",max.person,max.similarityRate);
	}
	else
	{
		if(member < m_numRegister)
		{
			fn++;
		}
		else
		{
			tn++;
		}
		fprintf(fdle,"NO ONE DETECTED\n");
	}

	fclose(fdle);
	
	m_Precision = (tp*1.0/((tp+fp)*1.0))*100.;
	m_Recall = (tp*1.0/(tp+fn)*1.0)*100.;

	sprintf(buf,"..\\Database\\method_recall_pca%d.txt",pca);
	//sprintf(buf,"..\\Database\\method_recall_th%f.txt",threshold);
	fdle = fopen(buf,"w");
	fprintf(fdle,"fp:%d fn:%d tp:%d tn:%d\nPRE : %f\nRecall : %f",fp,fn,tp,tn,m_Precision,m_Recall);
	fclose(fdle);
	return max.person;
}

i_person RecognitionMethod::KTree(int seletK)
{
	FILE *fdle = fopen(m_filename,"r");
	for(int i = 0;i < m_numRegister;i++)
	{
		for(int j = 0;j < m_numSample;j++)
		{
			fscanf(fdle,"%f",m_recognitionRate[i*m_numSample+j]);
		}
	}
	fclose(fdle);

	for(int i = 0;i < m_numRegister;i++)
	{
		for(int j = 0;j < m_numSample;j++)
		{

		}
	}
	return max.person;
}

void RecognitionMethod::BubbleSort()
{
	m_sort = new float [m_numRegister*m_numSample];
	float temp;
	int location;
	for(int k = 0;k < m_numRegister*m_numSample;k++)
	{
		for(int i = 0;i < m_numRegister;i++)
		{
			for(int j = k%5;j < m_numSample;j++)
			{
				location = i*m_numSample+j;
				if(m_recognitionRate[location] > m_recognitionRate[location+1])
				{
					temp = m_recognitionRate[location];
					m_recognitionRate[location]	  = m_recognitionRate[location+1];
					m_recognitionRate[location+1] = temp;
				}
			}
		}
	}
}