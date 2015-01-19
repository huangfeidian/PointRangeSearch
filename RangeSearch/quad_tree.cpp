#include "point_search.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <queue>
#include <functional>
#include <iterator>
#include <stack>
#include <malloc.h>


struct SearchContext
{
private:

	struct QuadTreeNode
	{
		int begin_x;
		int end_x;
		int biggest_rank;
		int* all_node;
		int node_size;
		QuadTreeNode* node_1;
		QuadTreeNode* node_2;
		QuadTreeNode* node_3;
		QuadTreeNode* node_4;
	};
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

	};
	struct query_stack_struct
	{
		QuadTreeNode* node;
		int begin_x;
		int end_x;
	};
	//struct rank_queue
	//{
	//	std::vector<query_stack_struct> all_query;
	//	inline void push_back(query_stack_struct hehe)
	//	{
	//		all_query.push_back(hehe);
	//		std::push_heap(all_query.begin(), all_query.end(), [](const query_stack_struct& a, const query_stack_struct& b)->bool
	//		{
	//			return a.node->biggest_rank < b.node->biggest_rank;
	//		});
	//	}
	//	inline query_stack_struct pop()
	//	{
	//		std::pop_heap(all_query.begin(), all_query.end(), [](const query_stack_struct& a, const query_stack_struct& b)->bool
	//		{
	//			return a.node->biggest_rank < b.node->biggest_rank;
	//		});
	//		auto temp = all_query.back();
	//		all_query.pop_back();
	//		return temp;
	//	}
	//	inline bool empty()
	//	{
	//		return all_query.empty();
	//	}
	//};
private:


	QuadTreeNode* head;
	const float EPS = 0.0000001f;
	int point_number;
	std::queue<query_stack_struct> task_queue;
public:
	std::vector<Point> input_points;
	int* points_x;
	SearchContext(const Point* begin, const Point* end)
	{
		point_number = 0;
		head = nullptr;
		if (end == begin)
		{
			return;
		}
		point_number = end - begin;
		input_points.reserve(point_number);
		while (begin != end)
		{
			input_points.push_back(*begin);
			begin++;
		}
		std::make_heap(input_points.begin(), input_points.end(), [](const Point& a, const Point& b)->bool
		{
			return a.rank < b.rank;
		});
		std::sort_heap(input_points.begin(), input_points.end(), [](const Point& a, const Point& b)->bool
		{
			return a.rank < b.rank;
		});
		points_x = (int*) malloc(sizeof(int)*point_number);
		for (int i = 0; i < point_number; i++)
		{
			points_x[i] = i;
		}
		std::make_heap(points_x, points_x + point_number, [&](int a, int b)->bool
		{
			return input_points[a].x < input_points[b].x;
		});
		std::sort_heap(points_x, points_x + point_number, [&](int a, int b)->bool
		{
			return input_points[a].x < input_points[b].x;
		});
		head = (QuadTreeNode*) malloc(sizeof(QuadTreeNode));
		head->node_size = point_number;
		head->all_node = (int*) malloc(sizeof(int)*point_number);
		for (int i = 0; i < point_number; i++)
		{
			head->all_node[i] = i;
		}
		head->node_1 = nullptr;
		head->node_2 = nullptr;
		head->node_3 = nullptr;
		head->node_4 = nullptr;
		head->begin_x = 0;
		head->biggest_rank = point_number;
		head->end_x = point_number;
		stack_split(head);
	}
private:
	inline int get_x_lower(float IN_x)const
	{
		Point temp{ 0, 0, IN_x, 0 };
		auto lower_bound = std::lower_bound(points_x, points_x + point_number, IN_x, [&](int a, float b)->bool
		{
			return input_points[a].x < b;
		});
		return std::distance(points_x, lower_bound);
	}
	inline int get_x_upper(float IN_x)const
	{
		Point temp{ 0, 0, IN_x, 0 };
		auto upper_bound = std::upper_bound(points_x, points_x + point_number, IN_x, [&](float b, int a)->bool
		{
			return b<input_points[a].x;
		});
		return std::distance(points_x, upper_bound);
	}
	void stack_split(QuadTreeNode* INfather)
	{
		std::queue<QuadTreeNode*> frac_node_queue;
		frac_node_queue.push(INfather);
		while (!frac_node_queue.empty())
		{
			QuadTreeNode* father = frac_node_queue.front();
			frac_node_queue.pop();
			int quad_clap = (father->end_x - father->begin_x + 1) / 4;
			int delimiter_1 = father->begin_x + quad_clap;
			int delimiter_2 = delimiter_1 + quad_clap;
			int delimiter_3 = delimiter_2 + quad_clap;
			QuadTreeNode* son_1;
			QuadTreeNode* son_2;
			QuadTreeNode* son_3;
			QuadTreeNode* son_4;
			son_1 = (QuadTreeNode*) (malloc(sizeof(QuadTreeNode)));
			son_2 = (QuadTreeNode*) (malloc(sizeof(QuadTreeNode)));
			son_3 = (QuadTreeNode*) (malloc(sizeof(QuadTreeNode)));
			son_4 = (QuadTreeNode*) (malloc(sizeof(QuadTreeNode)));
			son_1->node_1 = nullptr;
			son_1->node_2 = nullptr;
			son_1->node_3 = nullptr;
			son_1->node_4 = nullptr;
			son_1->node_size = 0;
			son_1->biggest_rank = 0;
			son_1->begin_x = father->begin_x;
			son_1->end_x = delimiter_1;
			son_2->node_1 = nullptr;
			son_2->node_2 = nullptr;
			son_2->node_3 = nullptr;
			son_2->node_4 = nullptr;
			son_2->node_size = 0;
			son_2->biggest_rank = 0;
			son_2->begin_x = delimiter_1;
			son_2->end_x = delimiter_2;
			son_3->node_1 = nullptr;
			son_3->node_2 = nullptr;
			son_3->node_3 = nullptr;
			son_3->node_4 = nullptr;
			son_3->node_size = 0;
			son_3->biggest_rank = 0;
			son_3->begin_x = delimiter_2;
			son_3->end_x = delimiter_3;
			son_4->node_1 = nullptr;
			son_4->node_2 = nullptr;
			son_4->node_3 = nullptr;
			son_4->node_4 = nullptr;
			son_4->node_size = 0;
			son_4->biggest_rank = 0;
			son_4->begin_x = delimiter_3;
			son_4->end_x = father->end_x;
			father->node_1 = son_1;
			father->node_2 = son_2;
			father->node_3 = son_3;
			father->node_4 = son_4;
			int son_1_size, son_2_size, son_3_size, son_4_size;
			son_1_size = son_2_size = son_3_size = son_4_size = 0;
			float deli_1_x, deli_2_x, deli_3_x, deli_4_x;
			deli_1_x = input_points[points_x[delimiter_1]].x;
			deli_2_x = input_points[points_x[delimiter_2]].x;
			deli_3_x = input_points[points_x[delimiter_3]].x;
			int left_y_size = 0;
			for (int i = 0; i < father->node_size; i++)
			{
				if (input_points[father->all_node[i]].x <deli_1_x)
				{
					son_1_size++;
					continue;
				}
				if (input_points[father->all_node[i]].x < deli_2_x)
				{
					son_2_size++;
					continue;
				}
				if (input_points[father->all_node[i]].x < deli_3_x)
				{
					son_3_size++;
					continue;
				}
				son_4_size++;
			}
			son_1->all_node = (int*) malloc(sizeof(int)*son_1_size);
			son_2->all_node = (int*) malloc(sizeof(int)*son_2_size);
			son_3->all_node = (int*) malloc(sizeof(int)*son_3_size);
			son_4->all_node = (int*) malloc(sizeof(int)*son_4_size);
			for (int i = 0; i < father->node_size; i++)
			{
				if (input_points[father->all_node[i]].x < deli_1_x)
				{
					son_1->all_node[son_1->node_size] = father->all_node[i];
					son_1->node_size++;
					continue;
				}
				if (input_points[father->all_node[i]].x < deli_2_x)
				{
					son_2->all_node[son_2->node_size] = father->all_node[i];
					son_2->node_size++;
					continue;
				}
				if (input_points[father->all_node[i]].x < deli_3_x)
				{
					son_3->all_node[son_3->node_size] = father->all_node[i];
					son_3->node_size++;
					continue;
				}
				son_4->all_node[son_4->node_size] = father->all_node[i];
				son_4->node_size++;
				continue;
			}
			son_1->biggest_rank = son_1->all_node[son_1_size - 1];
			son_2->biggest_rank = son_2->all_node[son_2_size - 1];
			son_3->biggest_rank = son_3->all_node[son_3_size - 1];
			son_4->biggest_rank = son_4->all_node[son_4_size - 1];
			if (son_1->node_size > 1024)
			{
				frac_node_queue.push(son_1);
			}
			if (son_2->node_size >1024)
			{
				frac_node_queue.push(son_2);
			}
			if (son_3->node_size > 1024)
			{
				frac_node_queue.push(son_3);
			}
			if (son_4->node_size > 1024)
			{
				frac_node_queue.push(son_4);
			}
		}
	}
	std::vector<int> stack_normal_query_nth(QuadTreeNode* INcurrent, int INbegin_x, int INend_x, const Rect& range, int nth)
	{

		float ly = range.ly;
		float lx = range.lx;
		float hx = range.hx;
		float hy = range.hy;
		int total_size = 0;
		std::vector<int> total_result;
		mini_k_heap result_heap(nth);
		query_stack_struct head_stack_node;
		head_stack_node.node = INcurrent;
		head_stack_node.begin_x = INbegin_x;
		head_stack_node.end_x = INend_x;
		task_queue.push(head_stack_node);
		while (!task_queue.empty())
		{
			auto current_task_node = task_queue.front();
			task_queue.pop();
			QuadTreeNode* current = current_task_node.node;
			int begin_x = current_task_node.begin_x;
			int end_x = current_task_node.end_x;
			if (begin_x == end_x)
			{
				continue;
			}
			if ((current->begin_x == begin_x&&current->end_x == end_x))
			{
				for (int i = 0; i < current->node_size; i++)
				{
					int temp = current->all_node[i];
					if (total_size<nth || temp < result_heap.result[0])
					{
						float temp_y = input_points[temp].y;
						if (temp_y <= hy&&temp_y >= ly)
						{

							result_heap.push_back(temp);
							total_size++;
						}
					}
					else
					{
						break;
					}

				}

				continue;
			}
			if (current->node_1 == nullptr)
			{
				for (int i = 0; i < current->node_size; i++)
				{
					int temp = current->all_node[i];
					if (total_size<nth || temp < result_heap.result[0])
					{
						float temp_y = input_points[temp].y;
						float temp_x = input_points[temp].x;
						if (temp_y <= hy&&temp_y >= ly&&temp_x <= hx&&temp_x >= lx)
						{
							result_heap.push_back(temp);
							total_size++;
						}
					}
					else
					{
						break;
					}
				}

				continue;
			}
			query_stack_struct son_1_stack_node;
			query_stack_struct son_2_stack_node;
			query_stack_struct son_3_stack_node;
			query_stack_struct son_4_stack_node;
			int quad_clap = (current->end_x - current->begin_x + 1) / 4;
			int delimiter_1 = current->begin_x + quad_clap;
			int delimiter_2 = delimiter_1 + quad_clap;
			int delimiter_3 = delimiter_2 + quad_clap;
			son_1_stack_node.begin_x = begin_x;
			son_1_stack_node.end_x = delimiter_1;
			son_1_stack_node.node = current->node_1;
			son_2_stack_node.begin_x = delimiter_1;
			son_2_stack_node.end_x = delimiter_2;
			son_2_stack_node.node = current->node_2;
			son_3_stack_node.begin_x = delimiter_2;
			son_3_stack_node.end_x = delimiter_3;
			son_3_stack_node.node = current->node_3;
			son_4_stack_node.begin_x = delimiter_3;
			son_4_stack_node.end_x = end_x;
			son_4_stack_node.node = current->node_4;
			if (begin_x < son_1_stack_node.end_x)
			{

				if (end_x < son_1_stack_node.end_x)
				{
					son_1_stack_node.end_x = end_x;
				}
				task_queue.push(son_1_stack_node);
			}
			if (begin_x < son_2_stack_node.end_x&&end_x>son_2_stack_node.begin_x)
			{
				son_2_stack_node.begin_x = son_2_stack_node.begin_x > begin_x ? son_2_stack_node.begin_x : begin_x;
				son_2_stack_node.end_x = son_2_stack_node.end_x < end_x ? son_2_stack_node.end_x : end_x;
				task_queue.push(son_2_stack_node);
			}
			if (begin_x < son_3_stack_node.end_x&&end_x>son_3_stack_node.begin_x)
			{
				son_3_stack_node.begin_x = son_3_stack_node.begin_x > begin_x ? son_3_stack_node.begin_x : begin_x;
				son_3_stack_node.end_x = son_3_stack_node.end_x < end_x ? son_3_stack_node.end_x : end_x;
				task_queue.push(son_3_stack_node);
			}
			if (end_x>son_4_stack_node.begin_x)
			{
				son_4_stack_node.begin_x = son_4_stack_node.begin_x > begin_x ? son_4_stack_node.begin_x : begin_x;
				task_queue.push(son_4_stack_node);
			}
		}
		std::make_heap(result_heap.result.begin(), result_heap.result.end());
		std::sort_heap(result_heap.result.begin(), result_heap.result.end());
		return result_heap.result;
	}
public:
	std::vector<Point> stack_query(Rect range, int nth)
	{

		std::vector<int> result;
		if (point_number == 0)
		{
			return std::vector<Point>();
		}
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		if (lower_x == point_number)
		{
			return std::vector<Point>();
		}
		if (upper_x == 0)
		{
			return std::vector<Point>();
		}
		result = stack_normal_query_nth(head, lower_x, upper_x, range, nth);
		std::vector<Point> point_result;
		point_result.reserve(result.size());
		for (auto i : result)
		{
			point_result.push_back(input_points[i]);
		}
		return point_result;
	}
	void clear_memory()
	{
		if (point_number != 0)
		{
			std::queue<QuadTreeNode*> frac_node_queue;
			frac_node_queue.push(head);
			free(points_x);
			while (!frac_node_queue.empty())
			{
				auto current_front = frac_node_queue.front();
				frac_node_queue.pop();
				if (current_front->node_1 != nullptr)
				{
					frac_node_queue.push(current_front->node_1);
					frac_node_queue.push(current_front->node_2);
					frac_node_queue.push(current_front->node_3);
					frac_node_queue.push(current_front->node_4);
				}
				free(current_front->all_node);
				free(current_front);
			}
		}

	}
};