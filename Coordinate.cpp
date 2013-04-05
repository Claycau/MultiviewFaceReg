#include "Coordinate.h"
#include <iostream>

using namespace std;

Coordinate::Coordinate()
{
    x = 0;
    y = 0;
}

Coordinate::Coordinate(int X)
{
	x = X;
	y = X;
}

Coordinate::Coordinate(int X,int Y)
{
	x = X;
	y = Y;
}

Coordinate &operator+(Coordinate ob1, Coordinate ob2)
{
      Coordinate temp;

      temp.x = ob1.x + ob2.x;
      temp.y = ob2.y + ob2.y;

      return temp;  
}

Coordinate &operator*(Coordinate ob1, Coordinate ob2)
{
      Coordinate temp;

      temp.x = ob1.x * ob2.x;
      temp.y = ob2.y * ob2.y;

      return temp;  
}

void Coordinate::operator=(Coordinate &c)
{ 
		this->x = c.x; 
		this->y = c.y;
}

Coordinate Coordinate::operator-() { // prefix -
      x--;y--;
      return *this;
}

Coordinate& Coordinate::operator++() 
{ // prefix ++
	x++;y++;
    return *this;	
}

Coordinate Coordinate::operator++(int) 
{//postfix ++

      Coordinate temp = *this;
      x++;y++;
      return *this;
}

void Coordinate::print()
{
	cout<<"("<<x<<","<<y<<")"<<endl;
}