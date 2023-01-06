#pragma once

struct IntPoint2D
{
public:
	int X;
	int Y;

public:
	inline IntPoint2D();

	inline IntPoint2D(int scalar);

	inline IntPoint2D(int x, int y);

public:
	inline IntPoint2D operator-(const IntPoint2D& v) const;
};

inline IntPoint2D::IntPoint2D()
	: X(0), Y(0) { }

inline IntPoint2D::IntPoint2D(int scalar)
	: X(scalar), Y(scalar) { }

inline IntPoint2D::IntPoint2D(int x, int y)
	: X(x), Y(y) { }

inline IntPoint2D IntPoint2D::operator-(const IntPoint2D& v) const
{
	return IntPoint2D(X - v.X, Y - v.Y);
}
