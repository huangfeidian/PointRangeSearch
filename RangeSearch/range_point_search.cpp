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
			if (new_rank < result.back())
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
			return input_points[a].x - b < EPS;
		});
		return std::distance(points_x, upper_bound);
	}
	void split(FracTreeNode* father)
	{
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
			if (input_points[father->all_node[i]].x <= middle_x)
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
			split(left_son);
		}
		if (right_son->node_size > 1024)//如果有多余1个的点，则继续分裂
		{
			split(right_son);
		}
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
	std::vector<int> stack_normal_query(FracTreeNode* INcurrent, int INbegin_x,int INend_x,float begin_y,float end_y)
	{
		struct query__stack_struct
		{
			FracTreeNode* node;
			int begin_x;
			int end_x;
		};
		std::vector<int> total_result;
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
			if (current->left==nullptr||(current->begin_x == begin_x&&current->end_x == end_x))//如果查询区间刚好合适，则没有必要继续下去查询了，直接在当前返回结果
			{
				for (int i = 0; i<current->node_size; i++)
				{
					if (input_points[current->all_node[i]].y < end_y&&input_points[current->all_node[i]].y >= begin_y)
					{
						result.push_back(current->all_node[i]);
					}
					
				}
				std::copy(result.cbegin(), result.cend(), std::back_inserter(total_result));
				continue;
			}
			//否则，分为两个部分递归查询，因为当前树的形状保证了每个内部节点一定有两个子节点
			//hint_begin和hint_end都可能为current->all_y_size 
			//此时会导致current->left_position[hint_begin]和current->left_position[hint_end]访问越界，right也是同理
			//所以我们需要考虑这两个值的越界情况
			//如果hint_begin没有越界的话，才能做递归的查询
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
		return total_result;
	}
	std::vector<int> stack_normal_query_nth(FracTreeNode* INcurrent, int INbegin_x, int INend_x, float begin_y, float end_y, int nth)
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
			if (current->left == nullptr || (current->begin_x == begin_x&&current->end_x == end_x))//如果查询区间刚好合适，则没有必要继续下去查询了，直接在当前返回结果
			{
				int return_node_size = 0;
				for (int i = 0; i<current->node_size&&return_node_size<nth; i++)
				{
					if (input_points[current->all_node[i]].y < end_y&&input_points[current->all_node[i]].y >= begin_y)
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
			//否则，分为两个部分递归查询，因为当前树的形状保证了每个内部节点一定有两个子节点
			//hint_begin和hint_end都可能为current->all_y_size 
			//此时会导致current->left_position[hint_begin]和current->left_position[hint_end]访问越界，right也是同理
			//所以我们需要考虑这两个值的越界情况
			//如果hint_begin没有越界的话，才能做递归的查询
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
		return total_result;
	}
	std::vector<int> stack_query(Rect range, int nth)
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_lower(range.hx);
		if (lower_x == point_number)
		{
			return result;
		}
		if (upper_x == 0)
		{
			return result;
		}
		return stack_normal_query_nth(head, lower_x, upper_x, range.ly,range.hy ,nth);
	}
};