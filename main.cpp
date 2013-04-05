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
			else if(continued == 'r' || continued == 'r')
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

void main()
{
	const int total_testperson  = 30;
	const int numSample		= 5;
	const int numRegistor	= 20;
	const int numBlock		= 5*2;//recognition.h也要改
	int ini_size			= 60;    	   
	int ini_pcaDimension	= 5;  
	int ini_numSelectBlock	= 3;
	double ini_threshold	= 0.55; 
	bool brak_to_reg		= false;
	string filename				= "..\\Database\\reg_inf.txt";
	string clock_txt			= "..\\Database\\time.txt";
	string buf_resultfilename	= "..\\DataBase\\record_result.txt";
	string resultfilename		= "..\\DataBase\\record_result.xls";
	fstream fp;
	if(!read_setting_file(filename.c_str(),resultfilename.c_str(),
						  ini_size,ini_pcaDimension,ini_threshold,ini_numSelectBlock,brak_to_reg))
	{
		fp.open(resultfilename,ios::trunc|ios::out);
		fp.close();
	}

	FaceRecognition face_reg;
	for(int size = ini_size; size <= 300; size += 30)
	{
		for(int pca = ini_pcaDimension; pca <= 12;pca++)
		{
			cout << "size = " << size << endl;
			cout << "pca = "  << pca  << endl;
			fp.open(filename,ios::out);
			fp << numRegistor << " "	<< numSample << " " << numBlock << " "
			   << size        << " "    << size	     << " " << pca	<< " " << ini_threshold <<" "<< ini_pcaDimension;
			fp.close();

			face_reg.InitSetting(filename.c_str());

			fp.open(resultfilename,ios::app|ios::out);
			clock_t registration = clock();

			if(!brak_to_reg)
			face_reg.StartRegistration(false);///////////

			registration = clock() - registration;
			fp << "registration" <<"\t" << registration << "\n";
			fp.close();

			for(int numSelectBlock = ini_numSelectBlock; numSelectBlock <= 6;numSelectBlock++)//10
			{
				cout << "numSelectBlock" << numSelectBlock << endl;
				fp.open(resultfilename,ios::app|ios::out);
				fp <<"numSelectBlock"<< "\t" << "PCA" <<"\t"  << "Size" << endl;
				fp << numSelectBlock << "\t" << pca   <<"\t"  << size   << "\n\n";
				fp.close();

				for(double threshold = ini_threshold;threshold <= 0.86;threshold +=0.025)
				{
					//resetting the setting file(reg_inf.txt)
					//以及清空buf reg 裡面的東西
					fp.open(filename,ios::out);
					fp  << numRegistor << " "	<< numSample << " " << numBlock << " "
						<< size        << " "    << size	     << " " << pca	<< " "
						<< threshold <<" "<< numSelectBlock ;
					fp.close();
					//initialize file
					fp.open(buf_resultfilename,ios::out|ios::trunc);
					fp.close();
					face_reg.InitSetting(filename.c_str(),false);

					clock_t reg = clock();
					//正臉
					for(int i = 1;i <= total_testperson*3;i += 3)
					{
						face_reg.StartRecognition(i,false);
					}
					reg = clock() - reg;
					reg /= total_testperson;
					//read pre recall from buf
					double pre,recall;
					fp.open(buf_resultfilename);
					fp >> pre >> recall;
					fp.close();
					//write result
					fp.open(resultfilename,ios::app|ios::out);
					fp << threshold<<"\t"<< pre <<"\t"<< recall << "\t" << reg << "\n";
					fp.close();
					//側臉
					/*for(int i = 2;i <= total_testperson*3;i += 1)
					{
					face_reg.StartRecognition(i);
					}*/
				}
			}
			ini_numSelectBlock	= 2;
			ini_threshold		= 0.55;
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