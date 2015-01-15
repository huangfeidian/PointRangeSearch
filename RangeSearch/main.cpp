
#include "point_search.cpp"
#include <vector>
#include <random>
#include <ctime>
#include <boost\timer.hpp>
#include <boost\progress.hpp>
using namespace std;
#define POINT_NUMBER 10000000
#define SEARCH_NUMBER 1000
int main()
{
	Point* hehe;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(1, 10000);
	hehe = new Point[POINT_NUMBER];
	for (int i = 0; i < POINT_NUMBER; i++)
	{
		hehe[i].rank = i;
		hehe[i].x = dis(gen);
		hehe[i].y = dis(gen);
		hehe[i].id = 0;
	}
	clock_t begin_t = clock();
	SearchContext haha(hehe, hehe + POINT_NUMBER);
	clock_t end_t = clock();
	std::cout << "time elapsed during creation is " << end_t - begin_t << endl;
	int c = 0;
	boost::progress_display currentProgress(SEARCH_NUMBER);
	for (int i = 0; i < SEARCH_NUMBER; i++)
	{
		
		Rect current_rect;
		float a, b;
		a = dis(gen);
		b = dis(gen);
		if (a < b)
		{
			current_rect.lx = a;
			current_rect.hx = b;
		}
		else
		{
			current_rect.hx = a;
			current_rect.lx = b;
		}
		a = dis(gen);
		b = dis(gen);
		if (a < b)
		{
			current_rect.ly = a;
			current_rect.hy = b;
		}
		else
		{
			current_rect.hy = a;
			current_rect.ly = b;
		}
		auto result = haha.stack_query(current_rect,20);
		c = result.size();
		++currentProgress;
	}
	begin_t = clock();
	std::cout << "total time elapsed during query is " << begin_t - end_t << endl;
	std::cout << "average query time is " << (1.0*(begin_t - end_t)) / SEARCH_NUMBER << endl;
}