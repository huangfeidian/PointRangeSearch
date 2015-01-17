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
	std::vector<Point> points;

};




