#include "point_search.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <queue>
#include <functional>
#include <iterator>
#include <stack>
struct normal_rect
{
	int lx;
	int ly;
	int hx;
	int hy;
};
//std::function<bool(const Point&, const Point&)>point_rank_predicate = [](const Point& a, const Point& b)->bool
//{
//	if (a.rank < b.rank)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//};
//std::function<bool(const Point&, const Point&)>point_y_predicate = [](const Point& a, const Point& b)->bool
//{
//	if (a.y < b.y)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//};
//std::function<bool(const Point&, const Point&)>point_x_predicate = [](const Point& a, const Point& b)->bool
//{
//	if (a.x< b.x)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//};
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
struct SearchContext
{
public:
	struct FracTreeNode
	{
		int begin_x;
		int end_x;
		//这里包含的点是begin_x<=x<end_x
		int* all_node;
		int node_size;
		FracTreeNode* left;
		FracTreeNode* right;
	};
public:
	std::vector<Point> input_points;
	int* points_x;
	FracTreeNode* head;
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
		head = new FracTreeNode();
		head->node_size = point_number;
		head->all_node = new int[point_number];
		for (int i = 0; i < point_number; i++)
		{
			head->all_node[i] = i;
		}
		head->left = nullptr;
		head->right = nullptr;
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
			return b<input_points[a].x;
		});
		return std::distance(points_x, upper_bound);
	}
	void stack_split(FracTreeNode* INfather)
	{
		std::queue<FracTreeNode*> frac_node_queue;
		frac_node_queue.push(INfather);
		while (!frac_node_queue.empty())
		{
			FracTreeNode* father = frac_node_queue.front();
			frac_node_queue.pop();
			int middle = (father->begin_x + father->end_x - 1) / 2;
			FracTreeNode* left_son;
			FracTreeNode* right_son;
			left_son = new FracTreeNode();
			right_son = new FracTreeNode();
			left_son->node_size = 0;
			left_son->left = nullptr;
			left_son->right = nullptr;
			right_son->node_size = 0;
			right_son->left = nullptr;
			right_son->right = nullptr;
			left_son->begin_x = father->begin_x;
			left_son->end_x = middle + 1;
			right_son->begin_x = middle + 1;
			right_son->end_x = father->end_x;
			father->left = left_son;
			father->right = right_son;
			float middle_x = input_points[points_x[middle]].x;
			int left_y_size = 0;
			for (int i = 0; i < father->node_size; i++)//统计左子树中的元素个数，用来预分配内存空间
			{
				if (input_points[father->all_node[i]].x < middle_x)
				{
					left_y_size++;
				}
			}
			//开始预先分配空间，并直接赋予初始值0
			left_son->all_node = new int[left_y_size];
			right_son->all_node = new int[father->node_size - left_y_size];
			//然后开始划分
			for (int i = 0; i < father->node_size; i++)
			{
				if (input_points[father->all_node[i]].x < middle_x)
				{
					left_son->all_node[left_son->node_size] = father->all_node[i];
					left_son->node_size++;
				}
				else
				{
					right_son->all_node[right_son->node_size] = father->all_node[i];
					right_son->node_size++;
				}
			}
			if (left_son->node_size > 1024)//如果有多余1个的点，则继续分裂
			{
				frac_node_queue.push(left_son);
			}
			if (right_son->node_size > 1024)//如果有多余1个的点，则继续分裂
			{
				frac_node_queue.push(right_son);
			}
		}
	}
	
	std::vector<int> stack_normal_query_nth(FracTreeNode* INcurrent, int INbegin_x, int INend_x, const Rect& range, int nth)
	{
		struct query__stack_struct
		{
			FracTreeNode* node;
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
			FracTreeNode* current = current_task_node.node;
			int begin_x = current_task_node.begin_x;
			int end_x = current_task_node.end_x;
			std::vector<int> result;
			if ( (current->begin_x == begin_x&&current->end_x == end_x))//如果查询区间刚好合适，则没有必要继续下去查询了，直接在当前返回结果
			{
				int return_node_size = 0;
				for (int i = 0; i<current->node_size&&return_node_size<nth; i++)
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
			if (current->left == nullptr)
			{
				int return_node_size = 0;
				for (int i = 0; i < current->node_size&&return_node_size < nth; i++)
				{
					int temp = current->all_node[i];
					if (input_points[temp].y <= range.hy&&input_points[temp].y >= range.ly&&input_points[temp].x <= range.hx&&input_points[temp].x >= range.lx)
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
			query__stack_struct left_stack_node;
			query__stack_struct right_stack_node;
			int middle_x_index = (current->begin_x + current->end_x - 1) / 2 + 1;
			left_stack_node.begin_x = begin_x;
			left_stack_node.end_x = current->left->end_x;
			left_stack_node.node = current->left;
			right_stack_node.begin_x = current->right->begin_x;
			right_stack_node.end_x = end_x;
			right_stack_node.node = current->right;
			//现在处理hint_end越界情况即 hint_end==current->all_y_size 则直接使用left_position[hint_end-1]即可
			if (left_stack_node.begin_x < left_stack_node.end_x)
			{
				if (end_x<left_stack_node.end_x)
				{
					left_stack_node.end_x = end_x;
				}
				task_queue.push(left_stack_node);
			}
			if (right_stack_node.begin_x < right_stack_node.end_x)
			{
				if (begin_x>right_stack_node.begin_x)
				{
					right_stack_node.begin_x = begin_x;
				}
				task_queue.push(right_stack_node);
			}
		}
		std::make_heap(result_heap.result.begin(), result_heap.result.end());
		std::sort_heap(result_heap.result.begin(), result_heap.result.end());
		return result_heap.result;
	}
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
			std::queue<FracTreeNode*> frac_node_queue;
			frac_node_queue.push(head);
			while (!frac_node_queue.empty())
			{
				auto current_front = frac_node_queue.front();
				frac_node_queue.pop();
				if (current_front->left != nullptr)
				{
					frac_node_queue.push(current_front->left);
					frac_node_queue.push(current_front->right);;
				}
				free(current_front->all_node);
				free(current_front);
			}
		}

	}
};