#ifndef H_RESULTFEATURE
#define H_RESULTFEATURE

struct ResultReg
{
	ResultReg()
	{
		person = 0;
		regRate = 0;
	};

	int person;
	double regRate;

	void operator=(ResultReg &c)
	{ 
		this->person = c.person; 
		this->regRate = c.regRate;
	}
};

#endif