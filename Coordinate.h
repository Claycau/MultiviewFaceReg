#ifndef H_Coordinate
#define H_Coordinate

class Coordinate
{
	public:
		Coordinate();
		Coordinate(int X);
		Coordinate(int X, int Y);
		friend Coordinate &operator*(Coordinate ob1, Coordinate ob2);
		friend Coordinate &operator+(Coordinate ob1, Coordinate ob2);
		void        operator=(Coordinate &);
		Coordinate  operator-();
		Coordinate& operator++();
		Coordinate  operator++(int);
		void print();
		int x,y;
};
#endif