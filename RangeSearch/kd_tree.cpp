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
#include <memory>
using namespace std;
#define FRAC 4
#define MINK 20
#define NSIZE 256
struct SearchContext
{
private:

	struct QuadTreeNode
	{
		int begin_x;
		int end_x;
		int begin_y;
		int end_y;
		vector<int> all_node;
		int node_size;
		bool is_x;
		shared_ptr<QuadTreeNode> son_nodes[FRAC];

	};
	/*struct mini_k_heap
	{
	public:
		std::vector<int> result;
		mini_k_heap()
		{
			result.reserve(MINK);
		}
		inline bool push_back(int new_rank)
		{
			if (result.size() < MINK)
			{
				result.push_back(new_rank);
				if (result.size() == MINK)
				{
					std::make_heap(result.begin(), result.end());
				}
				return true;
			}
			else
			{
				if (new_rank < result[0])
				{
					result[0] = new_rank;
					int i = 0;
					while (i < MINK / 2)
					{
						int less_index = 2 * i + 1;
						if (2 * i <= MINK - 3)
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
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		void resize()
		{
			result.clear();
			result.reserve(MINK);
		}
	};*/
	struct Range
	{
		int lx;
		int hx;
		int ly;
		int hy;
	};
	struct query_stack_struct
	{
		shared_ptr<QuadTreeNode> node;
		Range  range;
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

	int result[MINK];
	shared_ptr<QuadTreeNode> head;
	const int point_number;
	int total_size;
	queue<shared_ptr<QuadTreeNode>> frac_node_queue;
	queue<query_stack_struct> task_queue;
	//rank_queue task_queue;
public:
	std::vector<Point> input_points;
	vector<int> points_x;
	vector<int> points_y;
	SearchContext(const Point* begin, const Point* end) :point_number(end - begin)
	{
		if (end == begin)
		{
			return;
		}
		input_points.reserve(point_number);
		while (begin != end)
		{
			input_points.push_back(*begin);
			begin++;
		}
		sort(input_points.begin(), input_points.end(), [](const Point& a, const Point& b)->bool
		{
			return a.rank < b.rank;
		});
		points_x.reserve(point_number);
		points_y.reserve(point_number);
		for (int i = 0; i < point_number; i++)
		{
			points_x.push_back(i);
			points_y.push_back(i);
		}
		sort(points_x.begin(), points_x.end(), [&](int a, int b)->bool
		{
			return input_points[a].x < input_points[b].x;
		});
		sort(points_y.begin(), points_y.end(), [&](int a, int b)->bool
		{
			return input_points[a].y < input_points[b].y;
		});
		head = make_shared<QuadTreeNode>();
		head->node_size = point_number;
		head->all_node.reserve(point_number);
		for (int i = 0; i < point_number; i++)
		{
			head->all_node.push_back(i);
		}
		head->begin_x = 0;
		head->end_x = point_number;
		head->begin_y = 0;
		head->end_y = point_number;
		head->is_x = true;
		if (point_number >NSIZE)
		{
			stack_split(head);
		}

	}
private:
	inline int get_x_lower(float IN_x)const
	{
		auto lower_bound = std::lower_bound(points_x.cbegin(), points_x.cend(), IN_x, [&](int a, float b)->bool
		{
			return input_points[a].x < b;
		});
		return std::distance(points_x.cbegin(), lower_bound);
	}
	inline int get_x_upper(float IN_x)const
	{
		auto upper_bound = std::upper_bound(points_x.cbegin(), points_x.cend(), IN_x, [&](float b, int a)->bool
		{
			return b<input_points[a].x;
		});
		return std::distance(points_x.cbegin(), upper_bound);
	}
	inline int get_y_upper(float IN_y)const
	{
		auto upper_bound = std::upper_bound(points_y.cbegin(), points_y.cend(), IN_y, [&](float b, int a)->bool
		{
			return b<input_points[a].y;
		});
		return std::distance(points_y.cbegin(), upper_bound);
	}
	inline int get_y_lower(float IN_y)const
	{

		auto lower_bound = std::lower_bound(points_y.cbegin(), points_y.cend(), IN_y, [&](int a, float b)->bool
		{
			return input_points[a].y < b;
		});
		return std::distance(points_y.cbegin(), lower_bound);
	}

	void stack_split(shared_ptr<QuadTreeNode> INfather)
	{
		frac_node_queue.push(INfather);
		while (!frac_node_queue.empty())
		{
			auto temp = frac_node_queue.front();
			frac_node_queue.pop();
			if (temp->is_x)
			{
				split_x(temp);
			}
			else
			{
				split_y(temp);
			}
		}
	}
	inline void push_heap(int temp)
	{
		if (total_size < MINK)
		{
			result[total_size++] = temp;
			if (total_size == MINK)
			{
				make_heap(result, result + MINK);
			}
		}
		else
		{
			result[0] = temp;
			int i = 0;
			while (i < MINK / 2)
			{
				int less_index = 2 * i + 1;
				if (2 * i <= MINK - 3)
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
	void split_x(shared_ptr<QuadTreeNode> father)
	{
		int clip = (father->end_x - father->begin_x + FRAC - 1) / FRAC;
		shared_ptr<QuadTreeNode> sons[FRAC];
		for (int i = 0; i < FRAC; i++)
		{
			sons[i] = make_shared<QuadTreeNode>();
			sons[i]->begin_y = father->begin_y;
			sons[i]->end_y = father->end_y;
			sons[i]->is_x = false;
			father->son_nodes[i] = sons[i];
		}
		for (int i = 0; i < FRAC - 1; i++)
		{
			sons[i]->begin_x = father->begin_x + i*clip;
			sons[i]->end_x = father->begin_x + (i + 1)*clip;
		}
		sons[FRAC - 1]->begin_x = father->begin_x + (FRAC - 1)*clip;
		sons[FRAC - 1]->end_x = father->end_x;
		int son_size[FRAC] = { 0 };
		float deli[FRAC];
		for (int i = 0; i < FRAC; i++)
		{
			deli[i] = input_points[points_x[sons[i]->begin_x]].x;
		}
		for (int i = 0; i < father->node_size; i++)
		{
			auto temp = upper_bound(begin(deli), end(deli), input_points[father->all_node[i]].x);
			int index = distance(begin(deli), temp) - 1;
			son_size[index]++;
		}
		for (int i = 0; i < FRAC; i++)
		{
			sons[i]->all_node.reserve(son_size[i]);
			sons[i]->node_size = son_size[i];
		}
		for (int i = 0; i < father->node_size; i++)
		{
			auto temp = upper_bound(begin(deli), end(deli), input_points[father->all_node[i]].x);
			int index = distance(begin(deli), temp) - 1;
			sons[index]->all_node.push_back(father->all_node[i]);
		}
		for (int i = 0; i < FRAC; i++)
		{
			if (son_size[i]>NSIZE)
			{
				frac_node_queue.push(sons[i]);
			}
		}
		vector<int> temp_result;
		temp_result.reserve(MINK);
		for (int i = 0; i < MINK; i++)
		{
			temp_result.push_back(father->all_node[i]);
		}
		father->all_node.swap(temp_result);
	}
	void split_y(shared_ptr<QuadTreeNode> father)
	{
		int clip = (father->end_y - father->begin_y + FRAC - 1) / FRAC;
		shared_ptr<QuadTreeNode> sons[FRAC];
		for (int i = 0; i < FRAC; i++)
		{
			sons[i] = make_shared<QuadTreeNode>();
			sons[i]->begin_x = father->begin_x;
			sons[i]->end_x = father->end_x;
			sons[i]->is_x = true;
			father->son_nodes[i] = sons[i];
		}
		for (int i = 0; i < FRAC - 1; i++)
		{
			sons[i]->begin_y = father->begin_y + i*clip;
			sons[i]->end_y = father->begin_y + (i + 1)*clip;
		}
		sons[FRAC - 1]->begin_y = father->begin_y + (FRAC - 1)*clip;
		sons[FRAC - 1]->end_y = father->end_y;
		int son_size[FRAC] = { 0 };
		float deli[FRAC];
		for (int i = 0; i < FRAC; i++)
		{
			deli[i] = input_points[points_y[sons[i]->begin_y]].y;
		}
		for (int i = 0; i < father->node_size; i++)
		{
			auto temp = upper_bound(begin(deli), end(deli), input_points[father->all_node[i]].y);
			int index = distance(begin(deli), temp) - 1;
			son_size[index]++;
		}
		for (int i = 0; i < FRAC; i++)
		{
			sons[i]->all_node.reserve(son_size[i]);
			sons[i]->node_size = son_size[i];
		}
		for (int i = 0; i < father->node_size; i++)
		{
			auto temp = upper_bound(begin(deli), end(deli), input_points[father->all_node[i]].y);
			int index = distance(begin(deli), temp) - 1;
			sons[index]->all_node.push_back(father->all_node[i]);
		}
		for (int i = 0; i < FRAC; i++)
		{
			if (son_size[i]>NSIZE)
			{
				frac_node_queue.push(sons[i]);
			}
		}
		vector<int> temp_result;
		temp_result.reserve(MINK);
		for (int i = 0; i < MINK; i++)
		{
			temp_result.push_back(father->all_node[i]);
		}
		father->all_node.swap(temp_result);
	}
	void stack_normal_query_MINK(const Range& rect)
	{
		
		query_stack_struct head_stack_node;
		head_stack_node.node = head;
		head_stack_node.range = rect;
		task_queue.push(head_stack_node);
		Range new_range;
		query_stack_struct new_query;
		shared_ptr<QuadTreeNode> cur_node;
		Range cur_range;
		while (!task_queue.empty())
		{
			auto current_task_node = task_queue.front();
			task_queue.pop();
			cur_node= current_task_node.node;
			cur_range= current_task_node.range;
			if (cur_node->begin_x == cur_range.lx&&cur_node->end_x == cur_range.hx&&cur_node->begin_y == cur_range.ly&&cur_node->end_y == cur_range.hy)
			{
				for (auto temp : cur_node->all_node)
				{
					if (total_size < MINK || temp < result[0])
					{
						push_heap(temp);
					}
					else
					{
						break;
					}
				}
				continue;
			}
			if (cur_node->node_size<=NSIZE)
			{
				query_leaf(current_task_node);
				continue;
			}
			if (cur_node->is_x)
			{
				new_range.hy = cur_range.hy;
				new_range.ly = cur_range.ly;
				int clip = (cur_node->end_x - cur_node->begin_x + FRAC - 1) / FRAC;
				int begin_index = (cur_range.lx - cur_node->begin_x) / clip;
				for (int i = begin_index; i < FRAC; i++)
				{
					if (cur_node->begin_x + i*clip < cur_range.lx)
					{
						new_range.lx = cur_range.lx;
					}
					else
					{
						new_range.lx = cur_node->begin_x + i*clip;
					}
					if (cur_node->begin_x + (i + 1)*clip < cur_range.hx)
					{
						new_range.hx = cur_node->begin_x + (i + 1)*clip;
						new_query.range = new_range;
						new_query.node = cur_node->son_nodes[i];
						task_queue.push(new_query);
					}
					else
					{
						new_range.hx = cur_range.hx;
						new_query.range = new_range;
						new_query.node = cur_node->son_nodes[i];
						task_queue.push(new_query);
						break;
					}
				}
			}
			else
			{
				new_range.hx = cur_range.hx;
				new_range.lx = cur_range.lx;
				int clip = (cur_node->end_y - cur_node->begin_y + FRAC - 1) / FRAC;
				int begin_index = (cur_range.ly - cur_node->begin_y) / clip;
				for (int i = begin_index; i < FRAC; i++)
				{
					new_query.node = cur_node->son_nodes[i];
					if (cur_node->begin_y + i*clip < cur_range.ly)
					{
						new_range.ly = cur_range.ly;
					}
					else
					{
						new_range.ly = cur_node->begin_y + i*clip;
					}
					if (cur_node->begin_y + (i + 1)*clip < cur_range.hy)
					{
						new_range.hy = cur_node->begin_y + (i + 1)*clip;
						new_query.range = new_range;
						task_queue.push(new_query);
					}
					else
					{
						new_range.hy = cur_range.hy;
						new_query.range = new_range;
						task_queue.push(new_query);
						break;
					}
				}
			}
		}
		std::make_heap(result, result+total_size);
		std::sort_heap(result, result+total_size);
	}
	void query_leaf(query_stack_struct cur_query)
	{
		auto cur_node = cur_query.node;
		auto cur_range = cur_query.range;
		float hx, hy, lx, ly;
		if (cur_node->end_x == cur_range.hx)
		{
			if (cur_node->begin_x == cur_range.lx)
			{
				if (cur_node->end_y == cur_range.hy)
				{
					//这里就不需要再去判断begin_y了 之前的判断已经判断完了
					ly = input_points[points_y[cur_range.ly]].y;
					for (int i = 0; i < cur_node->node_size; i++)
					{
						int temp = cur_node->all_node[i];

						if (total_size < MINK || temp < result[0])
						{
							float temp_y = input_points[temp].y;
							if (temp_y >= ly)
							{
								push_heap(temp);
							}
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					hy = input_points[points_y[cur_range.hy]].y;
					if (cur_node->begin_y == cur_range.ly)
					{
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								if (temp_y < hy)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						ly = input_points[points_y[cur_range.ly]].y;
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								if (temp_y < hy&&temp_y >= ly)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
				}
			}
			else
			{
				lx = input_points[points_x[cur_range.lx]].x;
				if (cur_node->end_y == cur_range.hy)
				{
					//这里需要再去判断begin_y了 之前的判断已经判断完了
					if (cur_node->begin_y == cur_range.ly)
					{
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_x = input_points[temp].x;
								if (temp_x >= lx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						ly = input_points[points_y[cur_range.ly]].y;
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y >= ly&&temp_x >= lx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
				}
				else
				{
					hy = input_points[points_y[cur_range.hy]].y;
					if (cur_node->begin_y == cur_range.ly)
					{
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp <result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y < hy&&temp_x >= lx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						ly = input_points[points_y[cur_range.ly]].y;
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y < hy&&temp_y >= ly&&temp_x >= lx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			hx = input_points[points_x[cur_range.hx]].x;
			if (cur_node->begin_x == cur_range.lx)
			{
				if (cur_node->end_y == cur_range.hy)
				{
					//这里就需要再去判断begin_y 
					if (cur_node->begin_y == cur_range.ly)
					{
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_x = input_points[temp].x;
								if (temp_x<hx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						ly = input_points[points_y[cur_range.ly]].y;
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y >= ly&&temp_x<hx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
				}
				else
				{
					hy = input_points[points_y[cur_range.hy]].y;
					if (cur_node->begin_y == cur_range.ly)
					{
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp <result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y < hy&&temp_x<hx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						ly = input_points[points_y[cur_range.ly]].y;
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y < hy&&temp_y >= ly&&temp_x<hx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
				}
			}
			else
			{
				lx = input_points[points_x[cur_range.lx]].x;
				if (cur_node->end_y == cur_range.hy)
				{
					if (cur_node->begin_y == cur_range.ly)
					{
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_x = input_points[temp].x;
								if (temp_x >= lx&&temp_x<hx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						ly = input_points[points_y[cur_range.ly]].y;
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y >= ly&&temp_x >= lx&&temp_x<hx)
								{
									push_heap(temp);
									
								}
							}
							else
							{
								break;
							}
						}
					}
				}
				else
				{
					hy = input_points[points_y[cur_range.hy]].y;
					if (cur_node->begin_y == cur_range.ly)
					{
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y < hy&&temp_x >= lx&&temp_x<hx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						ly = input_points[points_y[cur_range.ly]].y;
						for (int i = 0; i < cur_node->node_size; i++)
						{
							int temp = cur_node->all_node[i];

							if (total_size < MINK || temp < result[0])
							{
								float temp_y = input_points[temp].y;
								float temp_x = input_points[temp].x;
								if (temp_y < hy&&temp_y >= ly&&temp_x<hx&&temp_x >= lx)
								{
									push_heap(temp);
								}
							}
							else
							{
								break;
							}
						}
					}
				}
			}
		}
	}
	void stack_query_x(query_stack_struct cur_query)
	{
		auto cur_node = cur_query.node;
		auto cur_range = cur_query.range;
		Range new_range;
		query_stack_struct new_query;
		new_range.hy = cur_range.hy;
		new_range.ly = cur_range.ly;
		int clip = (cur_node->end_x - cur_node->begin_x + FRAC - 1) / FRAC;
		int begin_index = (cur_range.lx - cur_node->begin_x) / clip;
		for (int i = begin_index; i < FRAC; i++)
		{
			if (cur_node->begin_x + i*clip < cur_range.lx)
			{
				new_range.lx = cur_range.lx;
			}
			else
			{
				new_range.lx = cur_node->begin_x + i*clip;
			}
			if (cur_node->begin_x + (i + 1)*clip < cur_range.hx)
			{
				new_range.hx = cur_node->begin_x + (i + 1)*clip;
				new_query.range = new_range;
				new_query.node = cur_node->son_nodes[i];
				task_queue.push(new_query);
			}
			else
			{
				new_range.hx = cur_range.hx;
				new_query.range = new_range;
				new_query.node = cur_node->son_nodes[i];
				task_queue.push(new_query);
				break;
			}
		}
	}
	void stack_query_y(query_stack_struct cur_query)
	{
		auto cur_node = cur_query.node;
		auto cur_range = cur_query.range;
		Range new_range;
		query_stack_struct new_query;
		new_range.hx = cur_range.hx;
		new_range.lx = cur_range.lx;
		int clip = (cur_node->end_y - cur_node->begin_y + FRAC - 1) / FRAC;
		int begin_index = (cur_range.ly - cur_node->begin_y) / clip;
		for (int i = begin_index; i < FRAC; i++)
		{
			if (cur_node->begin_y + i*clip < cur_range.ly)
			{
				new_range.ly = cur_range.ly;
			}
			else
			{
				new_range.ly = cur_node->begin_y + i*clip;
			}
			if (cur_node->begin_y + (i + 1)*clip < cur_range.hy)
			{
				new_range.hy = cur_node->begin_y + (i + 1)*clip;
				new_query.range = new_range;
				new_query.node = cur_node->son_nodes[i];
				task_queue.push(new_query);
			}
			else
			{
				new_range.hy = cur_range.hy;
				new_query.range = new_range;
				new_query.node = cur_node->son_nodes[i];
				task_queue.push(new_query);
				break;
			}
		}
	}
public:
	std::vector<Point> stack_query(Rect range, int nth)
	{
		if (point_number == 0)
		{
			return std::vector<Point>();
		}
		Range cur_range;
		total_size = 0;
		cur_range.ly = get_y_lower(range.ly);
		cur_range.hy = get_y_upper(range.hy);
		cur_range.lx = get_x_lower(range.lx);
		cur_range.hx = get_x_upper(range.hx);
		if (cur_range.lx == point_number || cur_range.hy == 0 || cur_range.ly == point_number || cur_range.hy == 0)
		{
			return std::vector<Point>();
		}
		stack_normal_query_MINK(cur_range);
		std::vector<Point> point_result;
		point_result.reserve(total_size);
		for (int i = 0; i < total_size;i++)
		{
			point_result.push_back(input_points[result[i]]);
		}
		return point_result;
	}
};