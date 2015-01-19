#include "point_search.h"
#include <iterator>
#include <vector>
#include <algorithm>
#include <array>
#include <future>
struct LinearSearchContext
{
	struct mini_k_heap
	{
	public:
		std::vector<int> result;
		int mini_k;
		mini_k_heap(int INmini_k)
		{
			mini_k = INmini_k;
			result.reserve(INmini_k);
		}
		inline void push_back(int new_rank)
		{
			if (result.size() < mini_k)
			{
				result.push_back(new_rank);
				if (result.size() == mini_k)
				{
					std::make_heap(result.begin(), result.end());
				}
			}
			else
			{
				if (new_rank < result[0])
				{
					result[0] = new_rank;
					int i = 0;
					while (i < mini_k / 2)
					{
						int less_index = 2 * i + 1;
						if (2 * i <= mini_k - 3)
						{
							if (result[2 * i + 1] < result[2 * i + 2])
							{
								less_index = 2 * i + 2;
							}
						}
						if (result[less_index]>result[i])
						{
							std::swap(result[less_index], result[i]);
							i = less_index;
						}
						else
						{
							break;
						}
					}
				}

			}
		}

	};
public:
	std::vector<Point> points;
	std::vector<int> points_x;
	std::array<std::vector<int>, 64> partition;
	std::array<int, 64> partition_size;
	std::array<float, 63> partition_x;
	int point_number;
	LinearSearchContext(const Point* points_begin, const Point* points_end)
	{
		using namespace std;
		point_number = points_end - points_begin;
		if (point_number == 0)
		{
			return;
		}
		points.reserve(point_number);
		points_x.reserve(point_number);
		for (int i = 0; i < point_number; i++)
		{
			points.push_back(*(points_begin + i));
		}
		sort(begin(points), end(points), [&](Point a, Point b)
		{
			return a.rank < b.rank;
		});
		for (int i = 0; i < point_number; i++)
		{
			points_x.push_back(i);
		}
		sort(points_x.begin(), points_x.end(), [&](int a, int b)->bool
		{
			return points[a].x < points[b].x;
		});
		int clip = (point_number+63) / 64;
		for (int i = 0; i < 63; i++)
		{
			partition_x[i] = points[points_x[i*clip]].x;
		}
		for (int i = 0; i < 64; i++)
		{
			partition_size[i] = 0;
		}
		for (int i = 0; i < point_number; i++)
		{
			float temp_x = points[i].x;
			auto temp_lower_bound = std::upper_bound(partition_x.cbegin(), partition_x.cend(),temp_x);
			int step = std::distance(partition_x.cbegin(), temp_lower_bound);
			partition_size[step]++;
		}
		for (int i = 0; i < 64; i++)
		{
			partition[i].reserve(partition_size[i]);
		}
		for (int i = 0; i < point_number; i++)
		{
			float temp_x = points[i].x;
			auto temp_lower_bound = std::upper_bound(partition_x.cbegin(), partition_x.cend(), temp_x);
			int step = std::distance(partition_x.cbegin(), temp_lower_bound);
			partition[step].push_back(i);
		}

	}
	std::vector<Point> search(const Rect& rect, int count)
	{
		using namespace std;
		std::vector<int> query_stack;
		std::vector<Point> final_result;
		final_result.reserve(count);
		mini_k_heap result_heap(count);
		query_stack.reserve(64);
		future<vector<int>> total_result[64];
		int total_result_size = 0;
		if (partition_x[0]>rect.lx)
		{
			total_result[total_result_size++] = move(async([&]()
			{
				return xy_search(partition[0], rect, count);
			}));
		}
		for (int i = 0; i < 62; i++)
		{
			if (rect.lx <= partition_x[i] && rect.hx >= partition_x[i + 1])
			{
				total_result[total_result_size++] = async([&]()
				{
					return y_search(partition[i + 1], rect, count);
				});
			}
			else
			{
				if ((rect.lx>partition_x[i + 1] || rect.hx < partition_x[i]))
				{
					continue;
				}
				else
				{
					total_result[total_result_size++] = async([&]()
					{
						return xy_search(partition[i + 1], rect, count);
					});
				}
			}
		}
		if (partition_x[62] <= rect.hx)
		{
			total_result[total_result_size++] = async([&]()
			{
				return xy_search(partition[63], rect, count);
			});
		}
		for (int i = 0; i < total_result_size;i++)
		{
			auto temp_result = total_result[i].get();
			for (auto j : temp_result)
			{
				result_heap.push_back(j);
			}
		}
		make_heap(result_heap.result.begin(), result_heap.result.end());
		sort_heap(result_heap.result.begin(), result_heap.result.end());
		for (auto i : result_heap.result)
		{
			final_result.push_back(points[i]);
		}
		return final_result;
		
	}
	std::vector<int> y_search(const std::vector<int>& input_vector, const Rect& rect, int count)
	{
		int result_count = 0;
		int vector_size = input_vector.size();
		std::vector<int> result;
		result.reserve(count);
		for (int i = 0; i < vector_size&&result_count < count; i++)
		{
			int temp = input_vector[i];
			if (points[temp].y >= rect.ly&&points[temp].y <= rect.hy)
			{
				result.push_back(temp);
				result_count++;
			}
		}
		return result;
	}
	std::vector<int> xy_search(const std::vector<int>& input_vector, const Rect& rect, int count)
	{
		int result_count = 0;
		int vector_size = input_vector.size();
		std::vector<int> result;
		result.reserve(count);
		for (int i = 0; i < vector_size&&result_count < count; i++)
		{
			int temp = input_vector[i];
			Point hehe = points[temp];
			if (hehe.y >= rect.ly&&hehe.y <= rect.hy&&hehe.x>=rect.lx&&hehe.x<=rect.hx)
			{
				result.push_back(temp);
				result_count++;
			}
		}
		return result;
	}

};




