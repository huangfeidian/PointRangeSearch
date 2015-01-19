#include "point_search.h"
#include <iterator>
#include <vector>
#include <algorithm>
inline bool is_inside(const Rect &rect, const Point &point)
{
	if (rect.lx <= point.x && rect.hx >= point.x &&
		rect.ly <= point.y && rect.hy >= point.y)
		return true;
	return false;
}
struct LinearSearchContext
{
	LinearSearchContext(const Point* points_begin, const Point* points_end)
		:points(points_begin, points_end)
	{
		using namespace std;
		sort(begin(points), end(points), [&](Point a, Point b){return a.rank < b.rank; });
	}

	int32_t search(const Rect rect, const int32_t count, Point* out_points)
	{
		using namespace std;
		int32_t result_count = 0;
		for (auto i = begin(points); result_count < count && i != end(points); i = next(i))
		{
			if (is_inside(rect, *i))
			{
				*out_points = *i;
				result_count++;
				out_points++;
			}
		}
		return result_count;
	}
	std::vector<Point> search(Rect rect, int count)
	{
		int result_count = 0;
		std::vector<Point> result;
		result.reserve(count);
		int size = points.size();
		for (int i = 0; i < size&&result_count < count; i++)
		{
			Point hehe = points[i];
			if (hehe.y >= rect.ly&&hehe.y <= rect.hy&&hehe.x >= rect.lx&&hehe.x <= rect.hx)
			{
				result.push_back(hehe);
				result_count++;
			}
		}
		return result;
	}
	std::vector<Point> points;

};




