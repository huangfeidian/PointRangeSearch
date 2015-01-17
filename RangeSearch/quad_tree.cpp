#include "point_search.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <queue>
#include <functional>
#include <iterator>
#include <stack>

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
				std::pop_heap(result.begin(), result.end());
				result[mini_k - 1] = new_rank;
				std::push_heap(result.begin(), result.end());
			}

		}
	}

};
struct SearchContext
{
public:

	struct QuadTreeNode
	{
		int begin_x;
		int end_x;

		int* all_node;
		int node_size;
		QuadTreeNode* node_1;
		QuadTreeNode* node_2;
		QuadTreeNode* node_3;
		QuadTreeNode* node_4;
	};
public:
	std::vector<Point> input_points;
	int* points_x;
	QuadTreeNode* head;
	const float EPS = 0.0000001f;
	int point_number;
	SearchContext(const Point* begin, const Point* end)
	{
		point_number = 0;
		head = nullptr;
		while (begin != end)
		{
			input_points.push_back(*begin);
			begin++;
			point_number++;
		}
		input_points.shrink_to_fit();
		std::make_heap(input_points.begin(), input_points.end(), [](const Point& a, const Point& b)->bool
		{
			return a.rank < b.rank;
		});
		std::sort_heap(input_points.begin(), input_points.end(), [](const Point& a, const Point& b)->bool
		{
			return a.rank < b.rank;
		});
		points_x = new int[point_number];
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
		head = new QuadTreeNode();
		head->node_size = point_number;
		head->all_node = new int[point_number];
		for (int i = 0; i < point_number; i++)
		{
			head->all_node[i] = i;
		}
		head->node_1 = nullptr;
		head->node_2 = nullptr;
		head->node_3 = nullptr;
		head->node_4 = nullptr;
		head->begin_x = 0;
		head->end_x = point_number;
		stack_split(head);
	}


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
			return b<input_points[a].x ;
		});
		return std::distance(points_x, upper_bound);
	}
	void split(QuadTreeNode* father)
	{
		int quad_clap = (father->end_x - father->begin_x + 1) / 4;
		int delimiter_1 = father->begin_x + quad_clap;
		int delimiter_2 = delimiter_1 + quad_clap;
		int delimiter_3 = delimiter_2 + quad_clap;
		QuadTreeNode* son_1;
		QuadTreeNode* son_2;
		QuadTreeNode* son_3;
		QuadTreeNode* son_4;
		son_1 = new QuadTreeNode();
		son_2 = new QuadTreeNode();
		son_3 = new QuadTreeNode();
		son_4 = new QuadTreeNode();
		son_1->node_1 = nullptr;
		son_1->node_2 = nullptr;
		son_1->node_3 = nullptr;
		son_1->node_4 = nullptr;
		son_1->node_size = 0;
		son_1->begin_x = father->begin_x;
		son_1->end_x = delimiter_1;
		son_2->node_1 = nullptr;
		son_2->node_2 = nullptr;
		son_2->node_3 = nullptr;
		son_2->node_4 = nullptr;
		son_2->node_size = 0;
		son_2->begin_x = delimiter_1;
		son_2->end_x = delimiter_2;
		son_3->node_1 = nullptr;
		son_3->node_2 = nullptr;
		son_3->node_3 = nullptr;
		son_3->node_4 = nullptr;
		son_3->node_size = 0;
		son_3->begin_x = delimiter_2;
		son_3->end_x = delimiter_3;
		son_4->node_1 = nullptr;
		son_4->node_2 = nullptr;
		son_4->node_3 = nullptr;
		son_4->node_4 = nullptr;
		son_4->node_size = 0;
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
		son_1->node_size = son_1_size;
		son_2->node_size = son_2_size;
		son_3->node_size = son_3_size;
		son_4->node_size = son_4_size;
		son_1->all_node = new int[son_1_size];
		son_2->all_node = new int[son_2_size];
		son_3->all_node = new int[son_3_size];
		son_4->all_node = new int[son_4_size];
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
		if (son_1->node_size > 1024)
		{
			split(son_1);
		}
		if (son_2->node_size > 1024)
		{
			split(son_2);
		}
		if (son_3->node_size > 1024)
		{
			split(son_3);
		}
		if (son_4->node_size > 1024)
		{
			split(son_4);
		}
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
			son_1 = new QuadTreeNode();
			son_2 = new QuadTreeNode();
			son_3 = new QuadTreeNode();
			son_4 = new QuadTreeNode();
			son_1->node_1 = nullptr;
			son_1->node_2 = nullptr;
			son_1->node_3 = nullptr;
			son_1->node_4 = nullptr;
			son_1->node_size = 0;
			son_1->begin_x = father->begin_x;
			son_1->end_x = delimiter_1;
			son_2->node_1 = nullptr;
			son_2->node_2 = nullptr;
			son_2->node_3 = nullptr;
			son_2->node_4 = nullptr;
			son_2->node_size = 0;
			son_2->begin_x = delimiter_1;
			son_2->end_x = delimiter_2;
			son_3->node_1 = nullptr;
			son_3->node_2 = nullptr;
			son_3->node_3 = nullptr;
			son_3->node_4 = nullptr;
			son_3->node_size = 0;
			son_3->begin_x = delimiter_2;
			son_3->end_x = delimiter_3;
			son_4->node_1 = nullptr;
			son_4->node_2 = nullptr;
			son_4->node_3 = nullptr;
			son_4->node_4 = nullptr;
			son_4->node_size = 0;
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
			son_1->all_node = new int[son_1_size];
			son_2->all_node = new int[son_2_size];
			son_3->all_node = new int[son_3_size];
			son_4->all_node = new int[son_4_size];
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
			if (son_1->node_size > 1024)
			{
				frac_node_queue.push(son_1);
			}
			if (son_2->node_size > 1024)
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
		struct query__stack_struct
		{
			QuadTreeNode* node;
			int begin_x;
			int end_x;
		};
		std::vector<int> total_result;
		mini_k_heap result_heap(nth);
		std::queue<query__stack_struct> task_queue;
		query__stack_struct head_stack_node;
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
			std::vector<int> result;
			if ((current->begin_x == begin_x&&current->end_x == end_x))
			{
				int return_node_size = 0;
				for (int i = 0; i < current->node_size&&return_node_size < nth; i++)
				{
					if (input_points[current->all_node[i]].y <= range.hy&&input_points[current->all_node[i]].y >= range.ly)
					{
						result.push_back(current->all_node[i]);
						return_node_size++;
					}

				}
				for (int i = 0; i < return_node_size; i++)
				{
					result_heap.push_back(result[i]);
				}
				continue;
			}
			if (current->node_1 == nullptr)
			{
				int return_node_size = 0;
				for (int i = 0; i < current->node_size&&return_node_size < nth; i++)
				{
					int temp = current->all_node[i];
					if (input_points[temp].y <= range.hy&&input_points[temp].y >= range.ly&&input_points[temp].x<=range.hx&&input_points[temp].x>=range.lx)
					{
						result.push_back(current->all_node[i]);
						return_node_size++;
					}

				}
				for (int i = 0; i < return_node_size; i++)
				{
					result_heap.push_back(result[i]);
				}
				continue;
			}
			query__stack_struct son_1_stack_node;
			query__stack_struct son_2_stack_node;
			query__stack_struct son_3_stack_node;
			query__stack_struct son_4_stack_node;
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
	std::vector<int> stack_query(Rect range, int nth)
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		if (lower_x == point_number)
		{
			return result;
		}
		if (upper_x == 0)
		{
			return result;
		}
		return stack_normal_query_nth(head, lower_x, upper_x, range, nth);
	}
	void clear_memory()
	{
		std::queue<QuadTreeNode*> frac_node_queue;
		frac_node_queue.push(head);
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
			delete [] current_front->all_node;
			delete current_front;
		}
	}
};