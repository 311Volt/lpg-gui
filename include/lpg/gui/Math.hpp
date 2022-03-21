#ifndef LPG_GUI_MATH_H
#define LPG_GUI_MATH_H

namespace gui {
	class Point {
	public:
		Point():x(0.0f),y(0.0f){}
		Point(float x, float y):x(x),y(y){}
		float x, y;
		Point& operator+=(const Point& rhs){x+=rhs.x; y+=rhs.y; return *this;};
		Point& operator-=(const Point& rhs){x-=rhs.x; y-=rhs.y; return *this;};
		Point& operator*=(float rhs){x*=rhs; y*=rhs; return *this;}
		Point& operator/=(float rhs){x/=rhs; y/=rhs; return *this;}

		Point operator+() const {return *this;}
		Point operator-() const {Point ret; ret.x=-x; ret.y=-y; return ret;}

		Point operator+(const Point& rhs) const {Point ret=*this; ret+=rhs; return ret;}
		Point operator-(const Point& rhs) const {Point ret=*this; ret-=rhs; return ret;}
		Point operator*(float rhs) const {Point ret=*this; ret*=rhs; return ret;}
		Point operator/(float rhs) const {Point ret=*this; ret/=rhs; return ret;}

		bool operator==(const Point& rhs) const {return x==rhs.x && y==rhs.y;}
		bool operator!=(const Point& rhs) const {return !(*this==rhs);}
	};

	using Vec2 = Point;

	class Rect {
	public:
		Rect(){}
		Rect(const Point& a, const Point& b):a(a),b(b){}
		Rect(float x1, float y1, float x2, float y2):Rect(Point(x1,y1), Point(x2,y2)){}

		float getWidth() const {return b.x-a.x;}
		float getHeight() const {return b.y-a.y;}

		Point getSize() const {return Point(getWidth(),getHeight());}

		Point a, b;
		bool operator==(const Rect& rhs) const {return a==rhs.a && b==rhs.b;}
		bool operator!=(const Rect& rhs) const {return !(*this==rhs);}
	};
}

#endif // LPG_GUI_MATH_H
