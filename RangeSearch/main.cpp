#include "linear_search.cpp"
#include "quad_tree.cpp"
#include <vector>
#include <random>
#include <ctime>
#include <boost\timer.hpp>
#include <boost\progress.hpp>
using namespace std;
#define POINT_NUMBER 10000000
#define SEARCH_NUMBER 10000
#define SEARCH_RESULT 20
int main()
{
	Point* hehe;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(1, 100);
	vector<pair<int, int>> test_data{ { 2, 19 }, { 7, 10 }, { 12, 3 }, { 17, 62 }, { 21, 49 }, { 41, 95 }, { 58, 59 }, { 93, 70 },
									{ 5, 80 }, { 8, 37 }, { 15, 99 }, { 33, 30 }, { 52, 23 }, { 67, 89 } };
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
	LinearSearchContext linehaha(hehe, hehe + POINT_NUMBER);
	Point linear_result[SEARCH_RESULT];
	clock_t end_t = clock();
	std::cout << "time elapsed during creation is " << end_t - begin_t << endl;
	int c = 0;
	int fault = 0;
	int fault_size = 0;
	//Rect now;
	//now.hx = 33.6;
	//now.lx = 20.5;
	//now.hy = 64.7;
	//now.ly = 24.5;
	//auto tree_result = haha.stack_query(now, SEARCH_RESULT);
	//for (auto i : tree_result)
	//{
	//	cout << i << " ";
	//}
	//cout << endl;
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
		auto result = haha.stack_query(current_rect, SEARCH_RESULT);
		int linear_result_size = linehaha.search(current_rect, SEARCH_RESULT, linear_result);
		c = result.size();
		if (c != linear_result_size)
		{
			cout << "current range is : x " << current_rect.lx << "-" << current_rect.hx << " y" << current_rect.ly << "- " << current_rect.hy<<endl;
			cout << "tree result is ";
			for (int i = 0; i < c; i++)
			{
				cout << result[i] << " ";
			}
			cout << endl;
			cout << "linear result is ";
			for (int i = 0; i < linear_result_size; i++)
			{
				cout << linear_result[i].rank << " ";
			}
			cout << endl;
			break;
		}
		else
		{
			for (int k = 0; k < c; k++)
			{
				if (result[k] != linear_result[k].rank)
				{
					fault=1;
					break;
				}
			}
			if (fault == 1)
			{
				cout << "current range is : x " << current_rect.lx << "-" << current_rect.hx << " y" << current_rect.ly << "- " << current_rect.hy << endl;
				cout << "tree result is ";
				for (int i = 0; i < c; i++)
				{
					cout << result[i] << " ";
				}
				cout << endl;
				cout << "linear result is ";
				for (int i = 0; i < linear_result_size; i++)
				{
					cout << linear_result[i].rank << " ";
				}
				cout << endl;
				break;
			}
			
		}
		
	}
	begin_t = clock();
	std::cout << "total time elapsed during query is " << begin_t - end_t << endl;
	std::cout << "average query time is " << (1.0*(begin_t - end_t)) / SEARCH_NUMBER << endl;
	std::cout << "false is " << fault << std::endl;
	std::cout << "false size is " << fault_size << std::endl;
	haha.clear_memory();
}