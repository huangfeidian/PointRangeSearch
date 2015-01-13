#include "../../point_search.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <queue>
#include <functional>
#include <iterator>
struct normal_rect
{
	int lx;
	int ly;
	int hx;
	int hy;
};
std::function<bool(const Point&, const Point&)>point_rank_predicate = [](const Point& a, const Point& b)->bool
{
	if (a.rank < b.rank)
	{
		return true;
	}
	else
	{
		return false;
	}
};
std::function<bool(const Point&, const Point&)>point_y_predicate = [](const Point& a, const Point& b)->bool
{
	if (a.y < b.y)
	{
		return true;
	}
	else
	{
		return false;
	}
};
std::function<bool(const Point&, const Point&)>point_x_predicate = [](const Point& a, const Point& b)->bool
{
	if (a.x< b.x)
	{
		return true;
	}
	else
	{
		return false;
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
		std::vector<int> all_y;
		std::vector<int> left_position;
		std::vector<int> right_posiiton;
		int all_y_size;
		FracTreeNode* left;
		FracTreeNode* right;
	};
public:
	std::vector<Point> input_points;
	std::vector<int> points_x;
	std::vector<int> points_y;
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
		std::sort(input_points.begin(), input_points.end(), point_rank_predicate);
		points_x.reserve(point_number);
		points_y.reserve(point_number);
		for (int i = 0; i < point_number; i++)
		{
			points_x.push_back(i);
			points_y.push_back(i);
		}
		std::sort(points_x.begin(), points_x.end(), std::bind(&SearchContext::sort_x, this, std::placeholders::_1, std::placeholders::_2));
		std::sort(points_y.begin(), points_y.end(), [&](int a, int b)->bool
		{
			return input_points[a].y < input_points[b].y;
		});
		head = new FracTreeNode();
		head->all_y_size = point_number;
		head->all_y.swap(std::vector<int>(point_number, 0));
		for (int i = 0; i < point_number; i++)
		{
			head->all_y[i] = i;
		}
		head->left_position.swap(std::vector<int>(point_number, 0));
		head->right_posiiton.swap(std::vector<int>(point_number, 0));
		head->left = nullptr;
		head->right = nullptr;
		head->begin_x = 0;
		head->end_x = point_number;
		split(head);
	}
	bool sort_x(int a, int b)const
	{
		if (input_points[a].x < input_points[b].x)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool sort_y(int a, int b)const
	{
		if (input_points[a].y < input_points[b].y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	int get_x_lower(float IN_x)const
	{
		Point temp{ 0, 0, IN_x, 0 };
		auto lower_bound = std::lower_bound(points_x.cbegin(), points_x.cend(), IN_x, [&](int a, float b)->bool
		{
			if (input_points[a].x <b)
			{
				return true;
			}
			else
			{
				return false;
			}
		});
		return std::distance(points_x.cbegin(), lower_bound);
	}
	int get_x_upper(float IN_x)const
	{
		Point temp{ 0, 0, IN_x, 0 };
		auto upper_bound = std::upper_bound(points_x.cbegin(), points_x.cend(), IN_x, [&](float b, int a)->bool
		{
			if (input_points[a].x - b<EPS)
			{
				return false;
			}
			else
			{
				return true;
			}
		});
		return std::distance(points_x.cbegin(), upper_bound);
	}
	int get_y_lower(float IN_y)const
	{
		auto lower_bound = std::lower_bound(points_y.cbegin(), points_y.cend(), IN_y, [&](int a, float b)->bool
		{
			if (input_points[a].y < b)
			{
				return true;
			}
			else
			{
				return false;
			}
		});
		return std::distance(points_y.cbegin(), lower_bound);
	}
	int get_y_upper(float IN_y)const
	{
		auto upper_bound = std::upper_bound(points_y.cbegin(), points_y.cend(), IN_y, [&](float b, int a)->bool
		{
			if (input_points[a].y - b<EPS)
			{
				return false;
			}
			else
			{
				return true;
			}
		});
		return std::distance(points_y.cbegin(), upper_bound);
	}
	void split(FracTreeNode* father)
	{
		int middle = (father->begin_x + father->end_x - 1) / 2;
		FracTreeNode* left_son;
		FracTreeNode* right_son;
		left_son = new FracTreeNode();
		right_son = new FracTreeNode();
		left_son->all_y_size = 0;
		left_son->left = nullptr;
		left_son->right = nullptr;
		right_son->all_y_size = 0;
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
		for (auto i : father->all_y)//统计左子树中的元素个数，用来预分配内存空间
		{
			if (input_points[points_y[i]].x <= middle_x)
			{
				left_y_size++;
			}
		}
		//开始预先分配空间，并直接赋予初始值0
		left_son->all_y.swap(std::vector<int>(left_y_size, 0));
		left_son->left_position.swap(std::vector<int>(left_y_size, 0));
		left_son->right_posiiton.swap(std::vector<int>(left_y_size, 0));
		right_son->all_y.swap(std::vector<int>(father->all_y_size - left_y_size, 0));
		right_son->left_position.swap(std::vector<int>(father->all_y_size - left_y_size, 0));
		right_son->right_posiiton.swap(std::vector<int>(father->all_y_size - left_y_size, 0));
		//然后开始划分
		for (int i = 0; i < father->all_y_size; i++)
		{
			if (input_points[points_y[father->all_y[i]]].x <= middle_x)
			{
				left_son->all_y[left_son->all_y_size] = father->all_y[i];
				father->left_position[i] = left_son->all_y_size;
				father->right_posiiton[i] = right_son->all_y_size;
				left_son->all_y_size++;
			}
			else
			{
				right_son->all_y[right_son->all_y_size] = father->all_y[i];
				father->left_position[i] = left_son->all_y_size;
				father->right_posiiton[i] = right_son->all_y_size;
				right_son->all_y_size++;
			}
		}
		if (left_son->end_x != left_son->begin_x + 1)//如果有多余1个的点，则继续分裂
		{
			split(left_son);
		}
		if (right_son->end_x != right_son->begin_x + 1)//如果有多余1个的点，则继续分裂
		{
			split(right_son);
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
			for (int i = hint_begin; (i < hint_end) && (i<current->all_y_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->all_y_size)//此时考虑hint_end 需要判断其是否在所求区间内
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
			for (int i = hint_begin; (i < hint_end) && (i<current->all_y_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->all_y_size)//此时考虑hint_end 需要判断其是否在所求区间内
			{
				auto temp = current->all_y[hint_end];
				if (temp <= range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
				{
					result.push_back(points_y[current->all_y[hint_end]]);
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
		if (hint_begin == current->all_y_size)//如果等于越界的话，直接返回空值就行了
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
		int hint_right_begin = current->right_posiiton[hint_begin];
		//现在处理hint_end越界情况即 hint_end==current->all_y_size 则直接使用left_position[hint_end-1]即可
		int hint_left_end;
		int hint_right_end;
		if (hint_end != current->all_y_size)//如果不是相等的话，则需要赋值回来
		{
			hint_left_end = current->left_position[hint_end];
			hint_right_end = current->right_posiiton[hint_end];
		}
		else
		{
			hint_left_end = current->left_position[hint_end - 1];
			hint_right_end = current->right_posiiton[hint_end - 1];
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
			for (int i = hint_begin; (i < hint_end) && (i<current->all_y_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->all_y_size)//此时考虑hint_end 需要判断其是否在所求区间内
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
			for (int i = hint_begin; (i < hint_end) && (i<current->all_y_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->all_y_size)//此时考虑hint_end 需要判断其是否在所求区间内
			{
				auto temp = current->all_y[hint_end];
				if (temp <= range.hy)//由于hy是拥有同样y值的最高索引，所以只需要比较索引就行
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
		//否则，分为两个部分递归查询，因为当前树的形状保证了每个内部节点一定有两个子节点
		//hint_begin和hint_end都可能为current->all_y_size 
		//此时会导致current->left_position[hint_begin]和current->left_position[hint_end]访问越界，right也是同理
		//所以我们需要考虑这两个值的越界情况
		if (hint_begin == current->all_y_size)//如果等于越界的话，直接返回空值就行了
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
		int hint_right_begin = current->right_posiiton[hint_begin];
		//现在处理hint_end越界情况即 hint_end==current->all_y_size 则直接使用left_position[hint_end-1]即可
		int hint_left_end;
		int hint_right_end;
		if (hint_end != current->all_y_size)//如果不是相等的话，则需要赋值回来
		{
			hint_left_end = current->left_position[hint_end];
			hint_right_end = current->right_posiiton[hint_end];
		}
		else
		{
			hint_left_end = current->left_position[hint_end - 1];
			hint_right_end = current->right_posiiton[hint_end - 1];
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
	//下面的都是一些调试信息
	void output_single_node(FracTreeNode* current)
	{
		std::cout << "current x range is " << input_points[points_x[current->begin_x]].x << "---" << input_points[points_x[current->end_x - 1]].x << std::endl;
		std::cout << "current pivot x is " << input_points[points_x[(current->begin_x + current->end_x - 1) / 2]].x << std::endl;
		std::cout << "there are " << current->all_y_size << " nodes in this node" << std::endl;
		std::cout << "current y node is ";
		for (auto i : current->all_y)
		{
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
};

extern "C" __declspec(dllexport)
SearchContext* __stdcall
create(const Point* points_begin, const Point* points_end)
{
	return new SearchContext(points_begin, points_end);
}

extern "C" __declspec(dllexport)
int32_t __stdcall
search(SearchContext* sc, const Rect rect, const int32_t count, Point* out_points)
{
	auto result = sc->query(rect, count);
	for (auto i : result)
	{
		*out_points = sc->input_points[i];
		out_points++;
	}
	return result.size();
}

extern "C" __declspec(dllexport)
SearchContext* __stdcall
destroy(SearchContext* sc)
{
	delete sc;
	return nullptr;
}