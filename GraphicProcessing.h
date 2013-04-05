#ifndef H_Graphic
#define H_Graphic
#include "Coordinate.h"
#include <iostream>
#include <vector>
using namespace std;
class Graphic
{
	public:
		  void Line(vector<Coordinate> coord1,int index1,vector<Coordinate> coord2,int index2,double &m,double &b)
		  {
				double dx = coord1[index1].x-coord2[index2].x;
				double dy = coord1[index1].y-coord2[index2].y;
				m =  (double)(dy)/(dx)*1.0;
				b =  (double)(coord1[index1].y - m*coord1[index1].x);
		  }

		  inline void CuttingRegion(vector<Coordinate> coord,int num_slope,double *m,double *b)
		 {
			for(int i = 0;i < num_slope;i++)
			{
				int next = i+1;
				if(next == num_slope)next = 0;

				//printf("Cutting points <%d,%d> and <%d,%d>\t",coord[i].x,coord[i].y,coord[next].x,coord[next].y);
				Line(coord,next,coord,i,m[i],b[i]);
				//printf("m = %f\tb = %f\n",m[i],b[i]);
			}
		 }
	private:
};
#endif 