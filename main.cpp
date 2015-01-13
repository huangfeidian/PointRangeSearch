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
		//��������ĵ���begin_x<=x<end_x
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
		for (auto i : father->all_y)//ͳ���������е�Ԫ�ظ���������Ԥ�����ڴ�ռ�
		{
			if (input_points[points_y[i]].x <= middle_x)
			{
				left_y_size++;
			}
		}
		//��ʼԤ�ȷ���ռ䣬��ֱ�Ӹ����ʼֵ0
		left_son->all_y.swap(std::vector<int>(left_y_size, 0));
		left_son->left_position.swap(std::vector<int>(left_y_size, 0));
		left_son->right_posiiton.swap(std::vector<int>(left_y_size, 0));
		right_son->all_y.swap(std::vector<int>(father->all_y_size - left_y_size, 0));
		right_son->left_position.swap(std::vector<int>(father->all_y_size - left_y_size, 0));
		right_son->right_posiiton.swap(std::vector<int>(father->all_y_size - left_y_size, 0));
		//Ȼ��ʼ����
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
		if (left_son->end_x != left_son->begin_x + 1)//����ж���1���ĵ㣬���������
		{
			split(left_son);
		}
		if (right_son->end_x != right_son->begin_x + 1)//����ж���1���ĵ㣬���������
		{
			split(right_son);
		}
	}
	std::vector<int> normal_query_hint(FracTreeNode* current, normal_rect range, int hint_begin, int hint_end)
	{
		std::vector<int> result;
		if (current->begin_x == range.lx&&current->end_x == range.hx)//�����ѯ����պú��ʣ���û�б�Ҫ������ȥ��ѯ�ˣ�ֱ���ڵ�ǰ���ؽ��
		{
			//�������hint_begin��hint_end�ж������
			//hint_begin==hint_end!=current->all_y_size ��hint_end���Ԫ���Ƿ�Ӧ�üӽ�ȥ��Ҫ��ѯ���Ƿ���hy�ڲ�
			//hint_begin==hint_end==current->all_y_size ���ʱû�з���ֵ Ϊ��
			//hint_begin<hint_end==current->all_y_size ���ʱ�ķ���ֵΪall_y[hint_begin]��all_y[hint_end-1]
			//hint_begin<hint_end<current->all_y_size ��ʱall_y[hint_begin]��all_y[hint_end-1]֮����������Է��� ��hint_end����Ҫ��һ����ѯ�Ƿ���hy�ڲ�
			result.reserve(hint_end - hint_begin + 1);//ֱ��ȫ������
			for (int i = hint_begin; (i < hint_end) && (i<current->all_y_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->all_y_size)//��ʱ����hint_end ��Ҫ�ж����Ƿ�������������
			{
				auto temp = current->all_y[hint_end];
				if (temp<range.hy)//����hy��ӵ��ͬ��yֵ���������������ֻ��Ҫ�Ƚ���������
				{
					result.push_back(points_y[current->all_y[hint_end]]);
				}
			}
			//��ʱ result ����洢����input_points��������������������ֱ�����ź���
			std::sort(result.begin(), result.end());
			return result;//�����ڶ��vector�ϲ���ʱ��ֱ�ӹ鲢������
		}
		if (current->left == nullptr)//�����ǰ�ڵ�ΪҶ�ӽڵ㣬�޷�������ѯ��ֱ�ӷ���hint�������ǰ��Ľ��һ��
		{
			result.reserve(hint_end - hint_begin + 1);//ֱ��ȫ������
			for (int i = hint_begin; (i < hint_end) && (i<current->all_y_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->all_y_size)//��ʱ����hint_end ��Ҫ�ж����Ƿ�������������
			{
				auto temp = current->all_y[hint_end];
				if (temp <= range.hy)//����hy��ӵ��ͬ��yֵ���������������ֻ��Ҫ�Ƚ���������
				{
					result.push_back(points_y[current->all_y[hint_end]]);
				}
			}
			//��ʱ result ����洢����input_points��������������������ֱ�����ź���
			std::sort(result.begin(), result.end());
			return result;//�����ڶ��vector�ϲ���ʱ��ֱ�ӹ鲢������
		}
		//���򣬷�Ϊ�������ֵݹ��ѯ����Ϊ��ǰ������״��֤��ÿ���ڲ��ڵ�һ���������ӽڵ�
		//hint_begin��hint_end������Ϊcurrent->all_y_size 
		//��ʱ�ᵼ��current->left_position[hint_begin]��current->left_position[hint_end]����Խ�磬rightҲ��ͬ��
		//����������Ҫ����������ֵ��Խ�����
		if (hint_begin == current->all_y_size)//�������Խ��Ļ���ֱ�ӷ��ؿ�ֵ������
		{
			return result;
		}
		//���hint_beginû��Խ��Ļ����������ݹ�Ĳ�ѯ
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
		//���ڴ���hint_endԽ������� hint_end==current->all_y_size ��ֱ��ʹ��left_position[hint_end-1]����
		int hint_left_end;
		int hint_right_end;
		if (hint_end != current->all_y_size)//���������ȵĻ�������Ҫ��ֵ����
		{
			hint_left_end = current->left_position[hint_end];
			hint_right_end = current->right_posiiton[hint_end];
		}
		else
		{
			hint_left_end = current->left_position[hint_end - 1];
			hint_right_end = current->right_posiiton[hint_end - 1];
		}
		if (range.lx < left_range.hx)//�������ѯ�������������н�������ݹ��ѯ
		{
			left_range.hx = range.hx > left_range.hx ? left_range.hx : range.hx;
			left_result = normal_query_hint(current->left, left_range, hint_left_begin, hint_left_end);
		}
		if (right_range.lx < range.hx)//�������ѯ�������������н�������ݹ��ѯ
		{
			right_range.lx = range.lx > right_range.lx ? range.lx : right_range.lx;
			right_result = normal_query_hint(current->right, right_range, hint_right_begin, hint_right_end);
		}
		//�������������ϵ������Ǿ��Բ������ص��ģ�����ֱ�ӹ鲢
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
		if (current->begin_x == range.lx&&current->end_x == range.hx)//�����ѯ����պú��ʣ���û�б�Ҫ������ȥ��ѯ�ˣ�ֱ���ڵ�ǰ���ؽ��
		{
			//�������hint_begin��hint_end�ж������
			//hint_begin==hint_end!=current->all_y_size ��hint_end���Ԫ���Ƿ�Ӧ�üӽ�ȥ��Ҫ��ѯ���Ƿ���hy�ڲ�
			//hint_begin==hint_end==current->all_y_size ���ʱû�з���ֵ Ϊ��
			//hint_begin<hint_end==current->all_y_size ���ʱ�ķ���ֵΪall_y[hint_begin]��all_y[hint_end-1]
			//hint_begin<hint_end<current->all_y_size ��ʱall_y[hint_begin]��all_y[hint_end-1]֮����������Է��� ��hint_end����Ҫ��һ����ѯ�Ƿ���hy�ڲ�
			result.reserve(hint_end - hint_begin + 1);//ֱ��ȫ������
			for (int i = hint_begin; (i < hint_end) && (i<current->all_y_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->all_y_size)//��ʱ����hint_end ��Ҫ�ж����Ƿ�������������
			{
				auto temp = current->all_y[hint_end];
				if (temp<range.hy)//����hy��ӵ��ͬ��yֵ���������������ֻ��Ҫ�Ƚ���������
				{
					result.push_back(points_y[current->all_y[hint_end]]);
				}
			}
			//��ʱ result ����洢����input_points��������������������ֱ�����ź���
			std::sort(result.begin(), result.end());
			std::vector<int> nth_result;
			nth_result.reserve(nth);
			for (int i = 0; i < nth&&i < result.size(); i++)
			{
				nth_result.push_back(result[i]);
			}
			return nth_result;//�����ڶ��vector�ϲ���ʱ��ֱ�ӹ鲢������
		}
		if (current->left == nullptr)//�����ǰ�ڵ�ΪҶ�ӽڵ㣬�޷�������ѯ��ֱ�ӷ���hint�������ǰ��Ľ��һ��
		{
			result.reserve(hint_end - hint_begin + 1);//ֱ��ȫ������
			for (int i = hint_begin; (i < hint_end) && (i<current->all_y_size); i++)
			{
				result.push_back(points_y[current->all_y[i]]);
			}
			if (hint_end < current->all_y_size)//��ʱ����hint_end ��Ҫ�ж����Ƿ�������������
			{
				auto temp = current->all_y[hint_end];
				if (temp <= range.hy)//����hy��ӵ��ͬ��yֵ���������������ֻ��Ҫ�Ƚ���������
				{
					result.push_back(points_y[current->all_y[hint_end]]);
				}
			}
			//��ʱ result ����洢����input_points��������������������ֱ�����ź���
			std::sort(result.begin(), result.end());
			std::vector<int> nth_result;
			nth_result.reserve(nth);
			for (int i = 0; i < nth&&i < result.size(); i++)
			{
				nth_result.push_back(result[i]);
			}
			return nth_result;//�����ڶ��vector�ϲ���ʱ��ֱ�ӹ鲢������
		}
		//���򣬷�Ϊ�������ֵݹ��ѯ����Ϊ��ǰ������״��֤��ÿ���ڲ��ڵ�һ���������ӽڵ�
		//hint_begin��hint_end������Ϊcurrent->all_y_size 
		//��ʱ�ᵼ��current->left_position[hint_begin]��current->left_position[hint_end]����Խ�磬rightҲ��ͬ��
		//����������Ҫ����������ֵ��Խ�����
		if (hint_begin == current->all_y_size)//�������Խ��Ļ���ֱ�ӷ��ؿ�ֵ������
		{
			return result;
		}
		//���hint_beginû��Խ��Ļ����������ݹ�Ĳ�ѯ
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
		//���ڴ���hint_endԽ������� hint_end==current->all_y_size ��ֱ��ʹ��left_position[hint_end-1]����
		int hint_left_end;
		int hint_right_end;
		if (hint_end != current->all_y_size)//���������ȵĻ�������Ҫ��ֵ����
		{
			hint_left_end = current->left_position[hint_end];
			hint_right_end = current->right_posiiton[hint_end];
		}
		else
		{
			hint_left_end = current->left_position[hint_end - 1];
			hint_right_end = current->right_posiiton[hint_end - 1];
		}
		if (range.lx < left_range.hx)//�������ѯ�������������н�������ݹ��ѯ
		{
			left_range.hx = range.hx > left_range.hx ? left_range.hx : range.hx;
			left_result = normal_query_hint(current->left, left_range, hint_left_begin, hint_left_end, nth);
		}
		if (right_range.lx < range.hx)//�������ѯ�������������н�������ݹ��ѯ
		{
			right_range.lx = range.lx > right_range.lx ? range.lx : right_range.lx;
			right_result = normal_query_hint(current->right, right_range, hint_right_begin, hint_right_end, nth);
		}
		//�������������ϵ������Ǿ��Բ������ص��ģ�����ֱ�ӹ鲢
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
				std::merge(left_result.cbegin(), left_result.cend(), right_result.cbegin(), right_result.cend(), std::back_inserter(result));//����Ҫע��ضϵ�nth
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
	std::vector<int>  query(Rect range)//�����ѯ
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
	std::vector<int>  query(Rect range, int nth)//�����ѯ
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
	//����Ķ���һЩ������Ϣ
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