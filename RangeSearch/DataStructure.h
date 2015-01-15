
#include <functional>
#include "point_search.h"
#pragma once
struct normal_rect
{
	int lx;
	int ly;
	int hx;
	int hy;
};
std::function<bool(const Point&, const Point&)>point_rank_predicate = [](const Point& a, const Point& b)->bool
{
	if (a.rank < b.rank)
	{
		return true;
	}
	else
	{
		return false;
	}
};
std::function<bool(const Point&, const Point&)>point_y_predicate = [](const Point& a, const Point& b)->bool
{
	if (a.y < b.y)
	{
		return true;
	}
	else
	{
		return false;
	}
};
std::function<bool(const Point&, const Point&)>point_x_predicate = [](const Point& a, const Point& b)->bool
{
	if (a.x< b.x)
	{
		return true;
	}
	else
	{
		return false;
	}
};
