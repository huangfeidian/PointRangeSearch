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
		int* all_y;
		int* left_position;
		int* right_position;
		int node_size;
		FracTreeNode* left;
		FracTreeNode* right;
	};
public:
	std::vector<Point> input_points;
	int* points_x;
	int* points_y;
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
		points_y = new int[point_number];
		for (int i = 0; i < point_number; i++)
		{
			points_x[i] = i;
			points_y[i] = i;
		}
		std::make_heap(points_x, points_x + point_number, [&](int a, int b)->bool
		{
			return input_points[a].x < input_points[b].x;
		});
		std::sort_heap(points_x, points_x+point_number, [&](int a, int b)->bool
		{
			return input_points[a].x < input_points[b].x;
		});
		std::make_heap(points_y, points_y + point_number, [&](int a, int b)->bool
		{
			return input_points[a].y < input_points[b].y;
		});
		std::sort_heap(points_y, points_y+point_number, [&](int a, int b)->bool
		{
			return input_points[a].y < input_points[b].y;
		});
		head = new FracTreeNode();
		head->node_size = point_number;
		head->all_y = new int[point_number];
		for (int i = 0; i < point_number; i++)
		{
			head->all_y[i] = i;
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
		auto lower_bound = std::lower_bound(points_x, points_x+point_number, IN_x, [&](int a, float b)->bool
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
	inline int get_y_lower(float IN_y)const
	{
		auto lower_bound = std::lower_bound(points_y, points_y+point_number, IN_y, [&](int a, float b)->bool
		{
			return input_points[a].y < b;
		});
		return std::distance(points_y, lower_bound);
	}
	inline int get_y_upper(float IN_y)const
	{
		auto upper_bound = std::upper_bound(points_y, points_y+ point_number, IN_y, [&](float b, int a)->bool
		{
			return input_points[a].y - b < EPS;
		});
		return std::distance(points_y, upper_bound);
	}
	void split(FracTreeNode* father)
	{
		int middle = (father->begin_x + father->end_x - 1) / 2;
		father->left_position = new int[father->node_size];
		father->right_position=new int[father->node_size];
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
		for (int i = 0; i < father->node_size;i++)//统计左子树中的元素个数，用来预分配内存空间
		{
			if (input_points[points_y[father->all_y[i]]].x < middle_x)
			{
				left_y_size++;
			}
		}
		//开始预先分配空间，并直接赋予初始值0
		left_son->all_y = new int[left_y_size];
		right_son->all_y = new int[father->node_size - left_y_size];
		//然后开始划分
		for (int i = 0; i < father->node_size; i++)
		{
			if (input_points[points_y[father->all_y[i]]].x < middle_x)
			{
				left_son->all_y[left_son->node_size] = father->all_y[i];
				father->left_position[i] = left_son->node_size;
				father->right_position[i] = right_son->node_size;
				left_son->node_size++;
			}
			else
			{
				right_son->all_y[right_son->node_size] = father->all_y[i];
				father->left_position[i] = left_son->node_size;
				father->right_position[i] = right_son->node_size;
				right_son->node_size++;
			}
		}
		if (left_son->node_size>1024)//如果有多余1个的点，则继续分裂
		{
			split(left_son);
		}
		if (right_son->node_size>1024)//如果有多余1个的点，则继续分裂
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
			father->left_position = new int[father->node_size];
			father->right_position = new int[father->node_size];
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
				if (input_points[points_y[father->all_y[i]]].x < middle_x)
				{
					left_y_size++;
				}
			}
			//开始预先分配空间，并直接赋予初始值0
			left_son->all_y = new int[left_y_size];
			right_son->all_y = new int[father->node_size - left_y_size];
			//然后开始划分
			for (int i = 0; i < father->node_size; i++)
			{
				if (input_points[points_y[father->all_y[i]]].x < middle_x)
				{
					left_son->all_y[left_son->node_size] = father->all_y[i];
					father->left_position[i] = left_son->node_size;
					father->right_position[i] = right_son->node_size;
					left_son->node_size++;
				}
				else
				{
					right_son->all_y[right_son->node_size] = father->all_y[i];
					father->left_position[i] = left_son->node_size;
					father->right_position[i] = right_son->node_size;
					right_son->node_size++;
				}
			}
			if (left_son->node_size>1024)//如果有多余1个的点，则继续分裂
			{
				frac_node_queue.push(left_son);
			}
			if (right_son->node_size>1024)//如果有多余1个的点，则继续分裂
			{
				frac_node_queue.push(right_son);
			}
		}
	}
	std::vector<int> normal_query_hint(FracTreeNode* current, normal_rect range, int hint_begin, int hint_end)
	{
		std::vector<int> result;
		if (current->begin_x == range.lx&&current->end_x == range.hx)//如果查询区间刚好合适，则没有必要继续下去查询了，直接在当前返回结果
		{
			//这里对于hint_begin与hint_end有多种情况
			//hint_begin==hint_end!=current->all_y_size 则hint_end这个元素是否应该加进去需要查询他是否处在hy内部
			//hint_begin==hint_end==current->all_y_size 则此时没有返回值 为空
			//hint_begin<hint_end==current->all_y_size 则此时的返回值为all_y[hint_begin]到all_y[hint_end-1]
			//hint_begin<hint_end<current->all_y_size 此时all_y[hint_begin]到all_y[hint_end-1]之间的数都可以返回 而hint_end则需要进一步查询是否在hy内部
			result.reserve(hint_end - hint_begin + 1);//直接全部返回
			for (int i = hint_begin; (i < hint_end) && (i<current->node_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->node_size)//此时考虑hint_end 需要判断其是否在所求区间内
			{
				auto temp = current->all_y[hint_end];
				if (temp<range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
				{
					result.push_back(points_y[current->all_y[hint_end]]);
				}
			}
			//此时 result 里面存储的是input_points里面点的索引，这里我们直接先排好序
			std::sort(result.begin(), result.end());
			return result;//这样在多个vector合并的时候直接归并就行了
		}
		if (current->left == nullptr)//如果当前节点为叶子节点，无法继续查询，直接返回hint结果，与前面的结果一样
		{
			result.reserve(hint_end - hint_begin + 1);//直接全部返回
			float current_begin_x = input_points[points_x[range.lx]].x;
			float current_end_x = input_points[points_x[range.hx - 1]].x;
			for (int i = hint_begin; (i < hint_end) && (i<current->node_size); i++)
			{
				float temp = input_points[points_y[current->all_y[i]]].x;
				if (temp >= current_begin_x&&temp < current_end_x)
				{
					result.push_back(points_y[current->all_y[i]]);
				}
			}
			if (hint_end < current->node_size)//此时考虑hint_end 需要判断其是否在所求区间内
			{
				auto temp = current->all_y[hint_end];
				if (temp <= range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
				{
					float temp2 = input_points[points_y[temp]].x;
					if (temp2 >= current_begin_x&&temp2 < current_end_x)
					{
						result.push_back(points_y[current->all_y[hint_end]]);
					}
				}
			}
			//此时 result 里面存储的是input_points里面点的索引，这里我们直接先排好序
			std::sort(result.begin(), result.end());
			return result;//这样在多个vector合并的时候直接归并就行了
		}
		//否则，分为两个部分递归查询，因为当前树的形状保证了每个内部节点一定有两个子节点
		//hint_begin和hint_end都可能为current->all_y_size 
		//此时会导致current->left_position[hint_begin]和current->left_position[hint_end]访问越界，right也是同理
		//所以我们需要考虑这两个值的越界情况
		if (hint_begin == current->node_size)//如果等于越界的话，直接返回空值就行了
		{
			return result;
		}
		//如果hint_begin没有越界的话，才能做递归的查询
		normal_rect left_range;
		normal_rect right_range;
		std::vector<int> left_result;
		std::vector<int> right_result;
		left_range.lx = range.lx;
		left_range.hx = (current->begin_x + current->end_x - 1) / 2 + 1;
		left_range.ly = range.ly;
		left_range.hy = range.hy;
		right_range.lx = left_range.hx;
		right_range.hx = range.hx;
		right_range.ly = range.ly;
		right_range.hy = range.hy;
		int hint_left_begin = current->left_position[hint_begin];
		int hint_right_begin = current->right_position[hint_begin];
		//现在处理hint_end越界情况即 hint_end==current->all_y_size 则直接使用left_position[hint_end-1]即可
		int hint_left_end;
		int hint_right_end;
		if (hint_end != current->node_size)//如果不是相等的话，则需要赋值回来
		{
			hint_left_end = current->left_position[hint_end];
			hint_right_end = current->right_position[hint_end];
		}
		else
		{
			hint_left_end = current->left_position[hint_end - 1];
			hint_right_end = current->right_position[hint_end - 1];
		}
		if (range.lx < left_range.hx)//如果所查询区间与左区间有交集，则递归查询
		{
			left_range.hx = range.hx > left_range.hx ? left_range.hx : range.hx;
			left_result = normal_query_hint(current->left, left_range, hint_left_begin, hint_left_end);
		}
		if (right_range.lx < range.hx)//如果所查询区间与右区间有交集，则递归查询
		{
			right_range.lx = range.lx > right_range.lx ? range.lx : right_range.lx;
			right_result = normal_query_hint(current->right, right_range, hint_right_begin, hint_right_end);
		}
		//由于这两个集合的坐标是绝对不可能重叠的，所以直接归并
		if (left_result.empty())
		{
			if (right_result.empty())
			{
				return result;
			}
			else
			{
				return right_result;
			}
		}
		else
		{
			if (right_result.empty())
			{
				return left_result;
			}
			else
			{
				std::merge(left_result.cbegin(), left_result.cend(), right_result.cbegin(), right_result.cend(), std::back_inserter(result));
				return result;
			}
		}

	}
	std::vector<int> stack_normal_query_hint(FracTreeNode* INcurrent, normal_rect INrange, int INhint_begin, int INhint_end)
	{
		struct query__stack_struct
		{
			FracTreeNode* node;
			normal_rect range;
			int hint_begin;
			int hint_end;
		};
		std::vector<int> total_result;
		std::queue<query__stack_struct> task_queue;
		query__stack_struct head_stack_node;
		head_stack_node.node = INcurrent;
		head_stack_node.hint_begin = INhint_begin;
		head_stack_node.hint_end = INhint_end;
		head_stack_node.range = INrange;
		task_queue.push(head_stack_node);
		while (!task_queue.empty())
		{
			auto current_task_node = task_queue.front();
			task_queue.pop();
			FracTreeNode* current = current_task_node.node;
			int hint_begin = current_task_node.hint_begin;
			int hint_end = current_task_node.hint_end;
			normal_rect range = current_task_node.range;
			std::vector<int> result;
			if (current->begin_x == range.lx&&current->end_x == range.hx)//如果查询区间刚好合适，则没有必要继续下去查询了，直接在当前返回结果
			{
				//这里对于hint_begin与hint_end有多种情况
				//hint_begin==hint_end!=current->all_y_size 则hint_end这个元素是否应该加进去需要查询他是否处在hy内部
				//hint_begin==hint_end==current->all_y_size 则此时没有返回值 为空
				//hint_begin<hint_end==current->all_y_size 则此时的返回值为all_y[hint_begin]到all_y[hint_end-1]
				//hint_begin<hint_end<current->all_y_size 此时all_y[hint_begin]到all_y[hint_end-1]之间的数都可以返回 而hint_end则需要进一步查询是否在hy内部
				result.reserve(hint_end - hint_begin + 1);//直接全部返回
				for (int i = hint_begin; (i < hint_end) && (i<current->node_size); i++)
				{
					result.push_back(points_y[current->all_y[i]]);
				}
				if (hint_end < current->node_size)//此时考虑hint_end 需要判断其是否在所求区间内
				{
					auto temp = current->all_y[hint_end];
					if (temp<range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
					{
						result.push_back(points_y[current->all_y[hint_end]]);
					}
				}
				//此时 result 里面存储的是input_points里面点的索引，这里我们直接先排好序
				std::sort(result.begin(), result.end());
				std::copy(result.cbegin(), result.cend(), std::back_inserter(total_result));
				continue;
			}
			if (current->left == nullptr)//如果当前节点为叶子节点，无法继续查询，直接返回hint结果，与前面的结果一样
			{
				result.reserve(hint_end - hint_begin + 1);//直接全部返回
				float current_begin_x = input_points[points_x[range.lx]].x;
				float current_end_x = input_points[points_x[range.hx - 1]].x;
				for (int i = hint_begin; (i < hint_end) && (i<current->node_size); i++)
				{
					float temp = input_points[points_y[current->all_y[i]]].x;
					if (temp >= current_begin_x&&temp < current_end_x)
					{
						result.push_back(points_y[current->all_y[i]]);
					}
				}
				if (hint_end < current->node_size)//此时考虑hint_end 需要判断其是否在所求区间内
				{
					auto temp = current->all_y[hint_end];
					if (temp <= range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
					{
						float temp2 = input_points[points_y[temp]].x;
						if (temp2 >= current_begin_x&&temp2 < current_end_x)
						{
							result.push_back(points_y[current->all_y[hint_end]]);
						}
					}
				}
				//此时 result 里面存储的是input_points里面点的索引，这里我们直接先排好序
				std::sort(result.begin(), result.end());
				std::copy(result.cbegin(), result.cend(), std::back_inserter(total_result));
				continue;
			}
			//否则，分为两个部分递归查询，因为当前树的形状保证了每个内部节点一定有两个子节点
			//hint_begin和hint_end都可能为current->all_y_size 
			//此时会导致current->left_position[hint_begin]和current->left_position[hint_end]访问越界，right也是同理
			//所以我们需要考虑这两个值的越界情况
			if (hint_begin == current->node_size)//如果等于越界的话，直接返回空值就行了
			{
				continue;
			}
			//如果hint_begin没有越界的话，才能做递归的查询
			normal_rect left_range;
			normal_rect right_range;
			query__stack_struct left_stack_node;
			query__stack_struct right_stack_node;
			std::vector<int> left_result;
			std::vector<int> right_result;
			left_range.lx = range.lx;
			left_range.hx = (current->begin_x + current->end_x - 1) / 2 + 1;
			left_range.ly = range.ly;
			left_range.hy = range.hy;
			right_range.lx = left_range.hx;
			right_range.hx = range.hx;
			right_range.ly = range.ly;
			right_range.hy = range.hy;
			int hint_left_begin = current->left_position[hint_begin];
			int hint_right_begin = current->right_position[hint_begin];
			//现在处理hint_end越界情况即 hint_end==current->all_y_size 则直接使用left_position[hint_end-1]即可
			int hint_left_end;
			int hint_right_end;
			if (hint_end != current->node_size)//如果不是相等的话，则需要赋值回来
			{
				hint_left_end = current->left_position[hint_end];
				hint_right_end = current->right_position[hint_end];
			}
			else
			{
				hint_left_end = current->left_position[hint_end - 1];
				hint_right_end = current->right_position[hint_end - 1];
			}
			if (range.lx < left_range.hx)//如果所查询区间与左区间有交集，则递归查询
			{
				left_range.hx = range.hx > left_range.hx ? left_range.hx : range.hx;
				left_stack_node.range = left_range;
				left_stack_node.hint_begin = hint_left_begin;
				left_stack_node.hint_end = hint_left_end;
				left_stack_node.node = current->left;
				task_queue.push(left_stack_node);
			}
			if (right_range.lx < range.hx)//如果所查询区间与右区间有交集，则递归查询
			{
				right_range.lx = range.lx > right_range.lx ? range.lx : right_range.lx;
				right_stack_node.range = right_range;
				right_stack_node.hint_begin = hint_right_begin;
				right_stack_node.hint_end = hint_right_end;
				right_stack_node.node = current->right;
				task_queue.push(right_stack_node);
			}
		}
		return total_result;
	}
	std::vector<int> normal_query_hint(FracTreeNode* current, normal_rect range, int hint_begin, int hint_end, int nth)
	{
		std::vector<int> result;
		if (current->begin_x == range.lx&&current->end_x == range.hx)//如果查询区间刚好合适，则没有必要继续下去查询了，直接在当前返回结果
		{
			//这里对于hint_begin与hint_end有多种情况
			//hint_begin==hint_end!=current->all_y_size 则hint_end这个元素是否应该加进去需要查询他是否处在hy内部
			//hint_begin==hint_end==current->all_y_size 则此时没有返回值 为空
			//hint_begin<hint_end==current->all_y_size 则此时的返回值为all_y[hint_begin]到all_y[hint_end-1]
			//hint_begin<hint_end<current->all_y_size 此时all_y[hint_begin]到all_y[hint_end-1]之间的数都可以返回 而hint_end则需要进一步查询是否在hy内部
			result.reserve(hint_end - hint_begin + 1);//直接全部返回
			for (int i = hint_begin; (i < hint_end) && (i<current->node_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->node_size)//此时考虑hint_end 需要判断其是否在所求区间内
			{
				auto temp = current->all_y[hint_end];
				if (temp<range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
				{
					result.push_back(points_y[current->all_y[hint_end]]);
				}
			}
			//此时 result 里面存储的是input_points里面点的索引，这里我们直接先排好序
			std::sort(result.begin(), result.end());
			std::vector<int> nth_result;
			nth_result.reserve(nth);
			for (int i = 0; i < nth&&i < result.size(); i++)
			{
				nth_result.push_back(result[i]);
			}
			return nth_result;//这样在多个vector合并的时候直接归并就行了
		}
		if (current->left == nullptr)//如果当前节点为叶子节点，无法继续查询，直接返回hint结果，与前面的结果一样
		{
			result.reserve(hint_end - hint_begin + 1);//直接全部返回
			float current_begin_x = input_points[points_x[range.lx]].x;
			float current_end_x = input_points[points_x[range.hx - 1]].x;
			for (int i = hint_begin; (i < hint_end) && (i<current->node_size); i++)
			{
				float temp = input_points[points_y[current->all_y[i]]].x;
				if (temp >= current_begin_x&&temp < current_end_x)
				{
					result.push_back(points_y[current->all_y[i]]);
				}
			}
			if (hint_end < current->node_size)//此时考虑hint_end 需要判断其是否在所求区间内
			{
				auto temp = current->all_y[hint_end];
				if (temp <= range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
				{
					float temp2 = input_points[points_y[temp]].x;
					if (temp2 >= current_begin_x&&temp2 < current_end_x)
					{
						result.push_back(points_y[current->all_y[hint_end]]);
					}
				}
			}
			//此时 result 里面存储的是input_points里面点的索引，这里我们直接先排好序
			std::sort(result.begin(), result.end());
			std::vector<int> nth_result;
			nth_result.reserve(nth);
			for (int i = 0; i < nth&&i < result.size(); i++)
			{
				nth_result.push_back(result[i]);
			}
			return nth_result;//这样在多个vector合并的时候直接归并就行了
		}
		//否则，分为两个部分递归查询，因为当前树的形状保证了每个内部节点一定有两个子节点
		//hint_begin和hint_end都可能为current->all_y_size 
		//此时会导致current->left_position[hint_begin]和current->left_position[hint_end]访问越界，right也是同理
		//所以我们需要考虑这两个值的越界情况
		if (hint_begin == current->node_size)//如果等于越界的话，直接返回空值就行了
		{
			return result;
		}
		//如果hint_begin没有越界的话，才能做递归的查询
		normal_rect left_range;
		normal_rect right_range;
		std::vector<int> left_result;
		std::vector<int> right_result;
		left_range.lx = range.lx;
		left_range.hx = (current->begin_x + current->end_x - 1) / 2 + 1;
		left_range.ly = range.ly;
		left_range.hy = range.hy;
		right_range.lx = left_range.hx;
		right_range.hx = range.hx;
		right_range.ly = range.ly;
		right_range.hy = range.hy;
		int hint_left_begin = current->left_position[hint_begin];
		int hint_right_begin = current->right_position[hint_begin];
		//现在处理hint_end越界情况即 hint_end==current->all_y_size 则直接使用left_position[hint_end-1]即可
		int hint_left_end;
		int hint_right_end;
		if (hint_end != current->node_size)//如果不是相等的话，则需要赋值回来
		{
			hint_left_end = current->left_position[hint_end];
			hint_right_end = current->right_position[hint_end];
		}
		else
		{
			hint_left_end = current->left_position[hint_end - 1];
			hint_right_end = current->right_position[hint_end - 1];
		}
		if (range.lx < left_range.hx)//如果所查询区间与左区间有交集，则递归查询
		{
			left_range.hx = range.hx > left_range.hx ? left_range.hx : range.hx;
			left_result = normal_query_hint(current->left, left_range, hint_left_begin, hint_left_end, nth);
		}
		if (right_range.lx < range.hx)//如果所查询区间与右区间有交集，则递归查询
		{
			right_range.lx = range.lx > right_range.lx ? range.lx : right_range.lx;
			right_result = normal_query_hint(current->right, right_range, hint_right_begin, hint_right_end, nth);
		}
		//由于这两个集合的坐标是绝对不可能重叠的，所以直接归并
		if (left_result.empty())
		{
			if (right_result.empty())
			{
				return result;
			}
			else
			{
				return right_result;
			}
		}
		else
		{
			if (right_result.empty())
			{
				return left_result;
			}
			else
			{
				std::merge(left_result.cbegin(), left_result.cend(), right_result.cbegin(), right_result.cend(), std::back_inserter(result));//现在要注意截断到nth
				std::vector<int> nth_result;
				nth_result.reserve(nth);
				for (int i = 0; i < nth&&i < result.size(); i++)
				{
					nth_result.push_back(result[i]);
				}
				return nth_result;
			}
		}
	}
	std::vector<int> stack_normal_query_hint(FracTreeNode* INcurrent, normal_rect INrange, int INhint_begin, int INhint_end, int nth)
	{
		struct query__stack_struct
		{
			FracTreeNode* node;
			normal_rect range;
			int hint_begin;
			int hint_end;
		};
		mini_k_heap result_heap(nth);
		std::queue<query__stack_struct> task_queue;
		query__stack_struct head_stack_node;
		head_stack_node.node = INcurrent;
		head_stack_node.hint_begin = INhint_begin;
		head_stack_node.hint_end = INhint_end;
		head_stack_node.range = INrange;
		task_queue.push(head_stack_node);
		while (!task_queue.empty())
		{
			auto current_task_node = task_queue.front();
			task_queue.pop();
			FracTreeNode* current = current_task_node.node;
			int hint_begin = current_task_node.hint_begin;
			int hint_end = current_task_node.hint_end;
			normal_rect range = current_task_node.range;
			if (current->begin_x == range.lx&&current->end_x == range.hx)//如果查询区间刚好合适，则没有必要继续下去查询了，直接在当前返回结果
			{
				//这里对于hint_begin与hint_end有多种情况
				//hint_begin==hint_end!=current->all_y_size 则hint_end这个元素是否应该加进去需要查询他是否处在hy内部
				//hint_begin==hint_end==current->all_y_size 则此时没有返回值 为空
				//hint_begin<hint_end==current->all_y_size 则此时的返回值为all_y[hint_begin]到all_y[hint_end-1]
				//hint_begin<hint_end<current->all_y_size 此时all_y[hint_begin]到all_y[hint_end-1]之间的数都可以返回 而hint_end则需要进一步查询是否在hy内部
				for (int i = hint_begin; (i < hint_end) && (i<current->node_size); i++)
				{
					result_heap.push_back(points_y[current->all_y[i]]);
				}
				if (hint_end < current->node_size)//此时考虑hint_end 需要判断其是否在所求区间内
				{
					auto temp = current->all_y[hint_end];
					if (temp<range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
					{
						result_heap.push_back(points_y[current->all_y[hint_end]]);
					}
				}
				continue;
			}
			if (current->left == nullptr)//如果当前节点为叶子节点，无法继续查询，直接返回hint结果，与前面的结果一样
			{
				float current_begin_x = input_points[points_x[range.lx]].x;
				float current_end_x = input_points[points_x[range.hx - 1]].x;
				for (int i = hint_begin; (i < hint_end) && (i<current->node_size); i++)
				{
					float temp = input_points[points_y[current->all_y[i]]].x;
					if (temp >= current_begin_x&&temp < current_end_x)
					{
						result_heap.push_back(points_y[current->all_y[i]]);
					}
				}
				if (hint_end < current->node_size)//此时考虑hint_end 需要判断其是否在所求区间内
				{
					auto temp = current->all_y[hint_end];
					if (temp <= range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
					{
						float temp2 = input_points[points_y[temp]].x;
						if (temp2 >= current_begin_x&&temp2 < current_end_x)
						{
							result_heap.push_back(points_y[current->all_y[hint_end]]);
						}
					}
				}
				continue;
			}
			//否则，分为两个部分递归查询，因为当前树的形状保证了每个内部节点一定有两个子节点
			//hint_begin和hint_end都可能为current->all_y_size 
			//此时会导致current->left_position[hint_begin]和current->left_position[hint_end]访问越界，right也是同理
			//所以我们需要考虑这两个值的越界情况
			if (hint_begin == current->node_size)//如果等于越界的话，直接返回空值就行了
			{
				continue;
			}
			//如果hint_begin没有越界的话，才能做递归的查询
			normal_rect left_range;
			normal_rect right_range;
			query__stack_struct left_stack_node;
			query__stack_struct right_stack_node;
			std::vector<int> left_result;
			std::vector<int> right_result;
			left_range.lx = range.lx;
			left_range.hx = (current->begin_x + current->end_x - 1) / 2 + 1;
			left_range.ly = range.ly;
			left_range.hy = range.hy;
			right_range.lx = left_range.hx;
			right_range.hx = range.hx;
			right_range.ly = range.ly;
			right_range.hy = range.hy;
			int hint_left_begin = current->left_position[hint_begin];
			int hint_right_begin = current->right_position[hint_begin];
			//现在处理hint_end越界情况即 hint_end==current->all_y_size 则直接使用left_position[hint_end-1]即可
			int hint_left_end;
			int hint_right_end;
			if (hint_end != current->node_size)//如果不是相等的话，则需要赋值回来
			{
				hint_left_end = current->left_position[hint_end];
				hint_right_end = current->right_position[hint_end];
			}
			else
			{
				hint_left_end = current->left_position[hint_end - 1];
				hint_right_end = current->right_position[hint_end - 1];
			}
			if (range.lx < left_range.hx)//如果所查询区间与左区间有交集，则递归查询
			{
				left_range.hx = range.hx > left_range.hx ? left_range.hx : range.hx;
				left_stack_node.range = left_range;
				left_stack_node.hint_begin = hint_left_begin;
				left_stack_node.hint_end = hint_left_end;
				left_stack_node.node = current->left;
				task_queue.push(left_stack_node);
			}
			if (right_range.lx < range.hx)//如果所查询区间与右区间有交集，则递归查询
			{
				right_range.lx = range.lx > right_range.lx ? range.lx : right_range.lx;
				right_stack_node.range = right_range;
				right_stack_node.hint_begin = hint_right_begin;
				right_stack_node.hint_end = hint_right_end;
				right_stack_node.node = current->right;
				task_queue.push(right_stack_node);
			}
		}
		std::sort_heap(result_heap.result.begin(), result_heap.result.end());
		return result_heap.result;
	}

	std::vector<int>  query(Rect range)//处理查询
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		int lower_y = get_y_lower(range.ly);
		int upper_y = get_y_upper(range.hy);
		if (lower_x == point_number)
		{
			return result;
		}
		if (lower_y == point_number)
		{
			return result;
		}
		if (upper_x == 0)
		{
			return result;
		}
		if (upper_y == 0)
		{
			return result;
		}
		normal_rect new_range;
		new_range.hx = upper_x;
		new_range.lx = lower_x;
		new_range.hy = upper_y;
		new_range.ly = lower_y;
		return normal_query_hint(head, new_range, lower_y, upper_y);

	}
	std::vector<int> stack_query(Rect range)
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		int lower_y = get_y_lower(range.ly);
		int upper_y = get_y_upper(range.hy);
		if (lower_x == point_number)
		{
			return result;
		}
		if (lower_y == point_number)
		{
			return result;
		}
		if (upper_x == 0)
		{
			return result;
		}
		if (upper_y == 0)
		{
			return result;
		}
		normal_rect new_range;
		new_range.hx = upper_x;
		new_range.lx = lower_x;
		new_range.hy = upper_y;
		new_range.ly = lower_y;
		return stack_normal_query_hint(head, new_range, lower_y, upper_y);
	}
	std::vector<int>  query(Rect range, int nth)//处理查询
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		int lower_y = get_y_lower(range.ly);
		int upper_y = get_y_upper(range.hy);
		if (lower_x == point_number)
		{
			return result;
		}
		if (lower_y == point_number)
		{
			return result;
		}
		if (upper_x == 0)
		{
			return result;
		}
		if (upper_y == 0)
		{
			return result;
		}
		normal_rect new_range;
		new_range.hx = upper_x;
		new_range.lx = lower_x;
		new_range.hy = upper_y;
		new_range.ly = lower_y;
		return normal_query_hint(head, new_range, lower_y, upper_y, nth);

	}
	std::vector<int> stack_query(Rect range, int nth)
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		int lower_y = get_y_lower(range.ly);
		int upper_y = get_y_upper(range.hy);
		if (lower_x == point_number)
		{
			return result;
		}
		if (lower_y == point_number)
		{
			return result;
		}
		if (upper_x == 0)
		{
			return result;
		}
		if (upper_y == 0)
		{
			return result;
		}
		normal_rect new_range;
		new_range.hx = upper_x;
		new_range.lx = lower_x;
		new_range.hy = upper_y;
		new_range.ly = lower_y;
		return stack_normal_query_hint(head, new_range, lower_y, upper_y, nth);
	}
	void query_out(Rect range)
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		int lower_y = get_y_lower(range.ly);
		int upper_y = get_y_upper(range.hy);
		if (lower_x == point_number)
		{
			return;
		}
		if (lower_y == point_number)
		{
			return;
		}
		if (upper_x == 0)
		{
			return;
		}
		if (upper_y == 0)
		{
			return;
		}
		normal_rect new_range;
		new_range.hx = upper_x;
		new_range.lx = lower_x;
		new_range.hy = upper_y;
		new_range.ly = lower_y;
		result = normal_query_hint(head, new_range, lower_y, upper_y);
		for (auto i : result)
		{
			std::cout << input_points[i].x << " " << input_points[i].y << std::endl;
		}
	}
	void stack_query_out(Rect range)
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		int lower_y = get_y_lower(range.ly);
		int upper_y = get_y_upper(range.hy);
		if (lower_x == point_number)
		{
			return;
		}
		if (lower_y == point_number)
		{
			return;
		}
		if (upper_x == 0)
		{
			return;
		}
		if (upper_y == 0)
		{
			return;
		}
		normal_rect new_range;
		new_range.hx = upper_x;
		new_range.lx = lower_x;
		new_range.hy = upper_y;
		new_range.ly = lower_y;
		result = stack_normal_query_hint(head, new_range, lower_y, upper_y);
		for (auto i : result)
		{
			std::cout << input_points[i].x << " " << input_points[i].y << std::endl;
		}
	}
	void query_out(Rect range, int nth)
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		int lower_y = get_y_lower(range.ly);
		int upper_y = get_y_upper(range.hy);
		if (lower_x == point_number)
		{
			return;
		}
		if (lower_y == point_number)
		{
			return;
		}
		if (upper_x == 0)
		{
			return;
		}
		if (upper_y == 0)
		{
			return;
		}
		normal_rect new_range;
		new_range.hx = upper_x;
		new_range.lx = lower_x;
		new_range.hy = upper_y;
		new_range.ly = lower_y;
		result = normal_query_hint(head, new_range, lower_y, upper_y, nth);
		for (auto i : result)
		{
			std::cout << input_points[i].x << " " << input_points[i].y << std::endl;
		}
	}
	void stack_query_out(Rect range, int nth)
	{
		std::vector<int> result;
		int lower_x = get_x_lower(range.lx);
		int upper_x = get_x_upper(range.hx);
		int lower_y = get_y_lower(range.ly);
		int upper_y = get_y_upper(range.hy);
		if (lower_x == point_number)
		{
			return;
		}
		if (lower_y == point_number)
		{
			return;
		}
		if (upper_x == 0)
		{
			return;
		}
		if (upper_y == 0)
		{
			return;
		}
		normal_rect new_range;
		new_range.hx = upper_x;
		new_range.lx = lower_x;
		new_range.hy = upper_y;
		new_range.ly = lower_y;
		result = stack_normal_query_hint(head, new_range, lower_y, upper_y, nth);
		for (auto i : result)
		{
			std::cout << input_points[i].x << " " << input_points[i].y << std::endl;
		}
	}
	//下面的都是一些调试信息
	void output_single_node(FracTreeNode* current)
	{
		std::cout << "current x range is " << input_points[points_x[current->begin_x]].x << "---" << input_points[points_x[current->end_x - 1]].x << std::endl;
		std::cout << "current pivot x is " << input_points[points_x[(current->begin_x + current->end_x - 1) / 2]].x << std::endl;
		std::cout << "there are " << current->node_size << " nodes in this node" << std::endl;
		std::cout << "current y node is ";
		for (int a = 0; a < current->node_size;a++)
		{
			int i = current->all_y[a];
			std::cout << input_points[points_y[i]].y << " ";
		}
		std::cout << std::endl;
		std::cout << "new node " << std::endl;
	}
	void output_tree_structure()
	{
		std::queue<FracTreeNode*> output_queue;
		output_queue.push(head);
		while (!output_queue.empty())
		{
			auto current_top = output_queue.front();
			output_queue.pop();
			output_single_node(current_top);
			if (current_top->left != nullptr)
			{
				output_queue.push(current_top->left);
				output_queue.push(current_top->right);
			}
		}
	}
	void free_node(FracTreeNode* current)
	{
		delete [] current->all_y;
		delete [] current->left_position;
		delete [] current->right_position;
		if (current->left != nullptr)
		{
			free_node(current->left);
			free_node(current->right);
			delete current->left;
			delete current->right;
		}

	}
	void release()
	{
		free_node(head);
		delete head;
	}
	~SearchContext()
	{
		release();
		delete[] points_x;
		delete[] points_y;
	}
};
//extern "C" __declspec(dllexport) SearchContext* __stdcall create(const Point* points_begin, const Point* points_end)
//{
//	return new SearchContext(points_begin, points_end);
//}
//extern "C" __declspec(dllexport) int __stdcall search(SearchContext* sc, const Rect rect, const int count, Point* out_points)
//{
//	auto result = sc->query(rect, count);
//	for (auto i : result)
//	{
//		*out_points = sc->input_points[i];
//		out_points++;
//	}
//	return result.size();
//}
//extern "C" __declspec(dllexport) SearchContext* __stdcall destory(SearchContext* sc)
//{
//	delete(sc);
//	return NULL;
//}
