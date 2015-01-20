#include "linear_search.cpp"
#include "kd_tree.cpp"
#include <vector>
#include <random>
#include <ctime>

using namespace std;
#define POINT_NUMBER 10000000
#define SEARCH_NUMBER 100000
#define SEARCH_RESULT 20
int main()
{
	Point* hehe;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(1, 100000);
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
	Rect* query=new Rect[SEARCH_NUMBER];
	Point result[SEARCH_RESULT];
	clock_t begin_t = clock();
	SearchContext haha(hehe, hehe + POINT_NUMBER);
	clock_t end_t = clock();
	//query[0].lx = 23.5;
	//query[0].hx = 93.2;
	//query[0].ly = 11.9;
	//query[0].hy = 59.3;
	//haha.stack_query(query[0], 2);
	//return 1;
	std::cout << "time elapsed during kd creation is " << end_t - begin_t << endl;
	//LinearSearchContext linehaha(hehe, hehe + POINT_NUMBER);
	begin_t = clock();
	std::cout << "time elapsed during linear creation is " << begin_t - end_t << endl;
	int c = 0;
	int fault = 0;
	int fault_size = 0;
	int total_size = 0;
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
		query[i] = current_rect;
		/*int linear_result_size = linehaha.search(current_rect, SEARCH_RESULT, linear_result);
		c = result.size();
		if (c != linear_result_size)
		{
			cout << "current range is : x " << current_rect.lx << "-" << current_rect.hx << " y" << current_rect.ly << "- " << current_rect.hy<<endl;
			cout << "tree result is ";
			for (int i = 0; i < c; i++)
			{
				cout << result[i].rank << " ";
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
				if (result[k].rank != linear_result[k].rank)
				{
					fault=1;
					break;
				}
			}
			if (fault == 1)
			{
				cout << "current range is : x " << current_rect.lx << "-" << current_rect.hx << " y" << current_rect.ly << "- " << current_rect.hy << endl;
				cout << "tree result is "<<endl;
				for (int i = 0; i < c; i++)
				{
					cout << result[i].rank << " ";
				}
				cout << endl;
				for (int i = 0; i < c; i++)
				{
					cout << result[i].x << " ";
				}
				cout << endl;
				cout << "linear result is ";
				for (int i = 0; i < linear_result_size; i++)
				{
					cout << linear_result[i].rank << " ";
				}
				cout << endl;
				for (int i = 0; i < linear_result_size; i++)
				{
					cout << linear_result[i].x << " ";
				}
				cout << endl;
				break;
			}
			
		}*/
		
	}
	//for (int i = 0; i < SEARCH_NUMBER; i++)
	//{
	//	auto kd_result = haha.stack_query(query[i], 20);
	//	auto linear_result = linehaha.search(query[i], 20);
	//	if (linear_result.size() != kd_result.size())
	//	{
	//		cout << "false size in the "<<i<<" round"<<endl;
	//		cout << linear_result.size() << "   " << kd_result.size()<<endl;
	//		cout << "x " << query[i].lx << "-" << query[i].hx << "  " << "y: " << query[i].ly << "-" << query[i].hy << endl;
	//		for (auto i : linear_result)
	//		{
	//			cout << i.rank << " ";
	//		}
	//		cout << endl;
	//		return 1;
	//	}
	//	else
	//	{
	//		for (int i = 0; i < kd_result.size(); i++)
	//		{
	//			if (kd_result[i].rank != linear_result[i].rank)
	//			{
	//				cout << " false content" << endl;
	//				return 1;
	//			}
	//		}
	//	}
	//}
	begin_t = clock();
	for (int i = 0; i < SEARCH_NUMBER; i++)
	{
		auto result = haha.stack_query(query[i], 20);
		total_size += result.size();
	}
	end_t = clock();
	std::cout << "total time elapsed during kd query is " << end_t-begin_t << endl;
	std::cout << "average query time is " << (1.0*(end_t-begin_t)) / SEARCH_NUMBER << endl;
	//begin_t = clock();
	//for (int i = 0; i < SEARCH_NUMBER; i++)
	//{
	//	auto result = linehaha.search(query[i], 20);
	//	total_size += result.size();
	//}
	//end_t = clock();
	std::cout << "total time elapsed during linear query is " << end_t - begin_t << endl;
	std::cout << "average query time is " << (1.0*(end_t - begin_t)) / SEARCH_NUMBER << endl;
}