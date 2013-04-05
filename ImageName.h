#ifndef H_ImageName
#define H_ImageName
#include <string>
#include <iostream>
using namespace std;


 /****DoType****/
#define STITCH_TYPE				1
#define HARR_LIKE_RESIZE		2
#define REGISTER				3
 /****DoType****/
class FileNamed
{
	public:

	void NameinFile(int sample,int number_of_image,int DoType)
	{
		int_to_string_1<<sample;
		int_to_string_2<<number_of_image;
		 BMP = "_"+int_to_string_2.str() +".bmp";

		type = DoType;
		if(DoType == STITCH_TYPE)
		{
			StitchName_Center	= "..\\Feature Photo\\" + int_to_string_1.str() + BMP;
			FeatureName_Center	= "..\\feature\\"		+ int_to_string_1.str() + BMP;

			int_to_string_2.str("");
			int_to_string_2.clear();
			int_to_string_2<<number_of_image + 1;
			BMP = "_"+int_to_string_2.str()  + ".bmp";
			StitchName_Left		= "..\\Feature Photo\\" + int_to_string_1.str() + BMP;
			FeatureName_Left	= "..\\feature\\"		+ int_to_string_1.str() + BMP;

			int_to_string_2.str("");
			int_to_string_2.clear();
			int_to_string_2<<number_of_image + 2;
			BMP = "_" + int_to_string_2.str()  + ".bmp";
			StitchName_Right	= "..\\Feature Photo\\" + int_to_string_1.str() + BMP;
			FeatureName_Right	= "..\\feature\\"		+ int_to_string_1.str() + BMP;

			int_to_string_2.str("");
			int_to_string_2.clear();
			int_to_string_2<<number_of_image/3;
			BMP = "_" + int_to_string_2.str()  + ".bmp";
			FileName_Out		= "..\\PanormaicFace\\OrignialStitch\\Stitch" + int_to_string_1.str() + BMP;
			ResizeName			= "..\\PanormaicFace\\SmallStitch\\panoramic" + int_to_string_1.str() + BMP;
		}
		else if(DoType == REGISTER)
		{
			int_to_string_2.str("");
			int_to_string_2.clear();
			int_to_string_2<<number_of_image/3;
			BMP = "_"+int_to_string_2.str() +".bmp";
			ResizeName = "..\\PanormaicFace\\SmallStitch\\panoramic" + int_to_string_1.str() + BMP;
			//sprintf(ResizeName,		   "..\\PanormaicFace\\SmallStitch\\panoramic%d_%d.bmp",sample,number_of_image/3);
			//sprintf(ResizeName,		   "..\\face\\0_%d.bmp",number_of_image);
		}

	}
	private:
		int type;
	public:
		string StitchName_Center,StitchName_Left,StitchName_Right;
		string FeatureName_Center, FeatureName_Left,FeatureName_Right;
		string FileName_Out;
		string PanormicName;
		string ResizeName;
		string BMP;
		string test;
		ostringstream int_to_string_1;
		ostringstream int_to_string_2;
};

#endif