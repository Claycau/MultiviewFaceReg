#include "FaceRecognition.h"
bool read_setting_file(const char *filename,const char *resultfilename,int &ini_size,
	int &ini_pcaDimension,double &ini_threshold,int &ini_numSelectBlock,bool &brak_to_reg)
{
	fstream fp;
	char continued	= 'f';
	fp.open(filename,ios::end|ios::in);
	fp.seekg(0,ios::end);
	int fsize = fp.tellg();
	if(fsize > 0)
	{
		while(true)
		{
			cout << "有過去記錄檔，是否繼續使用?\ntrue(t) or false(f):";
			cin	 >> continued;
			if(continued == 't' || continued == 'T')
			{
				int buf[4];
				fp.seekg(0,ios::beg);
				fp >> buf[0]		>> buf[1]	>> buf[2] 
				   >> buf[3]		>> ini_size	   >> ini_pcaDimension >> ini_threshold	  >> ini_numSelectBlock;
				   return true;
			}
			else if(continued == 'r' || continued == 'R')
			{
				int buf[4];
				fp.seekg(0,ios::beg);
				fp >> buf[0]		>> buf[1]	>> buf[2] 
				   >> buf[3]		>> ini_size	   >> ini_pcaDimension >> ini_threshold	  >> ini_numSelectBlock;
				   brak_to_reg = true;
				   return true;
			}
			else if(continued == 'f' || continued == 'F')
			{
				return false;
			}
			else
			{
				cout << "輸入錯誤，請輸入t or f\n";
			}
		}
	}
	else
	{
		return false;
	}
}

void record_ini(int numRegistor,int numSample,int numBlock ,int size,int pca ,double ini_threshold,int numSelectBlock)
{
	string filename				= "..\\Database\\reg_inf.txt";
	string clock_txt			= "..\\Database\\time.txt";
	string buf_resultfilename	= "..\\DataBase\\t_record_result.txt";
	string resultfilename		= "..\\DataBase\\m_record_result.xls";
	string buf_pro_resultfilename	= "..\\DataBase\\t_pro_record_result.txt";
	string pro_resultfilename		= "..\\DataBase\\m_pro_record_result.xls";

	fstream fp;
	fp.open(resultfilename,ios::app|ios::out);
	fp <<"numSelectBlock"<< "\t" << "PCA" <<"\t"  << "Size" << endl;
	fp << numSelectBlock << "\t" << pca   <<"\t"  << size   << "\n\n";
	fp.close();

	fp.open(pro_resultfilename,ios::app|ios::out);
	fp <<"numSelectBlock"<< "\t" << "PCA" <<"\t"  << "Size" << endl;
	fp << numSelectBlock << "\t" << pca   <<"\t"  << size   << "\n\n";
	fp.close();

	//resetting the setting file(reg_inf.txt)
	fp.open(filename,ios::out);
	fp  << numRegistor << " "	<< numSample << " " << numBlock << " "
		<< size        << " "    << size	     << " " << pca	<< " "
		<< ini_threshold <<" "<< numSelectBlock ;
	fp.close();
	//initialize file
	fp.open(buf_resultfilename,ios::out|ios::trunc);
	fp.close();
	fp.open(buf_pro_resultfilename,ios::out|ios::trunc);
	fp.close();
}

void main()
{
	const int total_testperson  = 20;
	const int numSample		= 5;
	const int numRegistor	= 20;
	const int numBlock		= 5*2;//recognition.h也要改
	int ini_size			= 80;    	   
	int ini_pcaDimension	= 5;  
	int ini_numSelectBlock	= 4;
	double ini_threshold	= 0.5;
	double end_threshold	= 0.85; 
	double gap_threshold	= 0.025; 
	bool brak_to_reg		= false;
	string filename				= "..\\Database\\reg_inf.txt";
	string clock_txt			= "..\\Database\\time.txt";
	string buf_resultfilename	= "..\\DataBase\\t_record_result.txt";
	string resultfilename		= "..\\DataBase\\m_record_result.xls";
	string buf_pro_resultfilename	= "..\\DataBase\\t_pro_record_result.txt";
	string pro_resultfilename		= "..\\DataBase\\m_pro_record_result.xls";
	fstream fp;

	if(!read_setting_file(filename.c_str()  , resultfilename.c_str(),
						  ini_size			, ini_pcaDimension,
						  ini_threshold		, ini_numSelectBlock , brak_to_reg))
	{
		// == f or F will reset all
		fp.open(resultfilename,ios::trunc|ios::out);
		fp.close();
		fp.open(pro_resultfilename,ios::out|ios::trunc);
		fp.close();
	}

	FaceRecognition face_reg;
	for(int size = ini_size; size <= 300; size += 20)
	{
		for(int pca = ini_pcaDimension; pca <= 5;pca++)
		{
			cout << "size = " << size << endl << "pca = "  << pca  << endl;

			fp.open(filename,ios::out);
			fp << numRegistor << " "	<< numSample << " " << numBlock << " "
			   << size        << " "    << size	     << " " << pca	<< " " << ini_threshold <<" "<< ini_numSelectBlock;
			fp.close();

			face_reg.InitSetting(filename.c_str());

			
			clock_t registration = clock();
			if(!brak_to_reg)
			{
				face_reg.StartRegistration(false);///////////
			}
			registration = clock() - registration;
			//record time of registration
			fp.open(resultfilename,ios::app|ios::out);
			fp << "registration" <<"\t" << registration << "\n";
			fp.close();

			for(int numSelectBlock = ini_numSelectBlock; numSelectBlock <= 4;numSelectBlock++)//10
			{
				cout << "numSelectBlock = " << numSelectBlock << endl;
				record_ini(numRegistor,numSample,numBlock ,size,pca ,ini_threshold,numSelectBlock);

				face_reg.InitSetting(filename.c_str(),false);

				//正臉
				int num_data = (end_threshold - ini_threshold)/gap_threshold + 1.5;
				vector<vector<double>>pre(2,num_data);
				vector<vector<double>>recall(2,num_data);
				for(int i = 1;i <= total_testperson*3;i++)
				{
					face_reg.StartRecognition(i,pre,recall,false);
				}

				for(int j = 0;j < 2;j++)
				{
					ofstream fp;
					if(j == 0)fp.open(resultfilename,ios::app);
					else fp.open(pro_resultfilename,ios::app);

					for(double threshold = ini_threshold,i = 0;threshold < end_threshold + gap_threshold;threshold += gap_threshold,i++)
					{
						fp << threshold << "\t" << pre[j][i] << "\t"<< recall[j][i] << "\n";
					}
					fp.close();
				}
			}
			ini_numSelectBlock	= 2;
			ini_threshold		= 0.5;
		}
	}
	system("pause");
}
	//while(true)
	//{
	//	int REGISTRATION = -1;
	//	FaceRecognition f(true);
	//	cout<<"登記 : 0\n辨識 : 1\n兩者都要:2\n";
	//	cin >> REGISTRATION;
	//	if(REGISTRATION == 0)
	//	{
	//		fstream fp;
	//		fp.open(clock_txt,ios::app|ios::out);

	//		clock_t registration_time = clock();
	//		f.StartRegistration();
	//		registration_time = clock() - registration_time;
	//		cout <<"registration time = "<< registration_time << endl;

	//		fp <<"registration time = " << registration_time <<"\n";
	//		fp.close();
	//	}
	//	else if(REGISTRATION == 1)
	//	{
	//		int chooseDirection = -1;
	//		cout <<"profile face:1" << endl;
	//		cout <<"left face    2" << endl;
	//		cout <<"right face   3 "<< endl;
	//		cin  >> chooseDirection;

	//		fstream fp;
	//		fp.open(clock_txt,ios::app|ios::out);
	//		clock_t reg_time = clock();

	//		for(int i = chooseDirection;i <= total_testperson*3;i+=3)
	//		{
	//			f.StartRecognition(i);
	//		}
	//		reg_time = clock() - reg_time;
	//		cout <<"Reg time = "  << (reg_time/total_testperson) << endl;
	//		fp   << "reg_time = " << (reg_time/total_testperson) << "\n" ;
	//		fp.close();
	//	}
	//	else if(REGISTRATION == 2)
	//	{
	//		f.StartRegistration();
	//		int chooseDirection = -1;
	//		cout <<"profile face:1" << endl;
	//		cout <<"left face    2" << endl;
	//		cout <<"right face   3 "<< endl;
	//		cin  >> chooseDirection;
	//		fstream fp;
	//		fp.open(clock_txt,ios::app|ios::out);
	//		clock_t reg_time = clock();
	//		for(int i = chooseDirection;i <= total_testperson*3;i+=3)
	//		{
	//			f.StartRecognition(i);
	//		}
	//		reg_time = clock() - reg_time;
	//		cout <<"Reg time = " << (reg_time/total_testperson) << endl;
	//		fp   << "reg_time = "<< (reg_time/total_testperson) << "\n" ;
	//		fp.close();
	//	}
	//	else
	//	{
	//		cout<<"ERROR INPUT"<<endl;
	//	}
	//	printf("Continue 1 else 0\n");
	//	cin >> REGISTRATION;
	//	if(REGISTRATION == 0) break;
	//}	