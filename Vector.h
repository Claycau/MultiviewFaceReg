#include <float.h>
#include <iostream>
#ifndef PI
#define PI 3.14159265
#endif
class  Vector2D {
 public:
         // Vector2D Public Methods
        Vector2D(float _x=0, float _y=0)
                 : x(_x), y(_y) {
         }

        Vector2D operator+(const Vector2D &v) const {
                 return Vector2D(x + v.x, y + v.y);
        }
        
        Vector2D& operator+=(const Vector2D &v) {
                x += v.x; y += v.y;
               return *this;
         }
        Vector2D operator-(const Vector2D &v) const {
                 return Vector2D(x - v.x, y - v.y);
        }
        
        Vector2D& operator-=(const Vector2D &v) {
                x -= v.x; y -= v.y;
                return *this;
        }
        bool operator==(const Vector2D &v) const {
                 return x == v.x && y == v.y ;
         }
        Vector2D operator*(float f) const {
                return Vector2D(f*x, f*y);
        }
        
        Vector2D &operator*=(float f) {
                x *= f; y *= f; 
                return *this;
        }
         
		
        float LengthSquared() const { return x*x + y*y;}
        float Length() const { return sqrtf(LengthSquared()); }
		double Gettheta() const
		{ 
			/*return (atan2(y,x)*180/PI);*/
			if(x > 0 && y > 0)
			return (atan(y/x)*180/PI);

			else if(x > 0 && y < 0)
			return (atan(y/x)*180/PI + 360.);

			else if( x < 0)
			return (atan(y/x)*180/PI + 180.);

			else if(x == 0)
			{
				if(y > 0)return 90.;
				else	 return 270.;
			}
			else if(y == 0)
			{
				if( x > 0)return 0.;
				else      return 180;
			}
			else
			return atan(y/x)*180/PI ;
		}
         // Vector2D Public Data
         float x, y,theta;
 };