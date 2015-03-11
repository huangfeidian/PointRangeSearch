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
#include <fstream>
using namespace std;
#define FRAC 4
#define MINK 20
#define NSIZE 1024
struct SearchContext
{
private:

	struct QuadTreeNode
	{
		int begin_x;
		int end_x;
		vector<int> all_node;
		vector<int> hint[FRAC];
		vector<int> sort_rank;
		int node_size;
		float* all_x;
		QuadTreeNode* son_nodes[FRAC];
		inline QuadTreeNode()
		{
			for (int i = 0; i < FRAC; i++)
			{
				son_nodes[i] = nullptr;
			}
		}
#ifdef File_DBG
		int father_id;
		int my_id;
#endif
	};
	struct Range
	{
		int lx;
		int hx;
		int ly;
		int hy;
	};

	struct query_stack_struct
	{
		QuadTreeNode* node;
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
	int result[MINK];
	QuadTreeNode* head;
	const int point_number;
	int total_size;
	int id;
	queue<QuadTreeNode*> frac_node_queue;
	query_stack_struct task_queue[2000];
	int task_count;
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
		head = new QuadTreeNode();
		head->node_size = point_number;
		head->all_node.reserve(point_number);
		for (int i = 0; i < point_number; i++)
		{
			head->all_node.push_back(i);
		}
		head->begin_x = 0;
		head->end_x = point_number;
		head->all_x = new float[point_number];
		for (int i = 0; i < point_number; i++)
		{
			head->all_x[i] = input_points[points_x[i]].x;
		}
		if (point_number > NSIZE)
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
			return b < input_points[a].x;
		});
		return std::distance(points_x.cbegin(), upper_bound);
	}
	inline int get_y_upper(float IN_y)const
	{
		auto upper_bound = std::upper_bound(points_y.cbegin(), points_y.cend(), IN_y, [&](float b, int a)->bool
		{
			return b < input_points[a].y;
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

	void stack_split(QuadTreeNode* INfather)
	{
#ifdef File_DBG
		INfather->father_id = 0;
		INfather->my_id = 1;
		id = 2;
#endif
		frac_node_queue.push(INfather);
#ifdef File_DBG
		ofstream dbg_file("dbg_file.txt");
#endif
		while (!frac_node_queue.empty())
		{
			auto temp = frac_node_queue.front();
#ifdef File_DBG
			dbg_file << "father" << temp->father_id << "  my" << temp->my_id << endl;
			dbg_file << temp->node_size << " " << temp->begin_x << " " << temp->end_x << " " << temp->begin_y << " " << temp->end_y << endl;
#endif
			frac_node_queue.pop();
			split(temp);
		}
#ifdef File_DBG
		dbg_file.close();
#endif
	}
	void split(QuadTreeNode* father)
	{
		if (input_points[points_x[father->begin_x]].x == input_points[points_x[father->end_x - 1]].x)
		{
			return;
		}
		int clip = (father->end_x - father->begin_x + FRAC - 1) / FRAC;
		QuadTreeNode* sons[FRAC];
		for (int i = 0; i < FRAC; i++)
		{
			sons[i] = new QuadTreeNode();
			father->son_nodes[i] = sons[i];
#ifdef File_DBG
			sons[i]->father_id = father->my_id;
			sons[i]->my_id = id++;
#endif
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
		vector<int> split_position(father->node_size, 0);
		for (int i = 0; i < FRAC; i++)
		{
			deli[i] = input_points[points_x[sons[i]->begin_x]].x;
		}
		for (int i = 0; i < father->node_size; i++)
		{
			int index = 0;
			float temp_x = father->all_x[i];
			while (index < FRAC - 1 && deli[index + 1] <= temp_x)
			{
				index++;
			}
			son_size[index]++;
			split_position[i] = index;
		}
		for (int i = 0; i < FRAC; i++)
		{
			sons[i]->all_x = new float[son_size[i]];
			sons[i]->node_size = son_size[i];
			sons[i]->all_node.swap(vector<int>(son_size[i], 0));
		}
		for (int i = 0; i < FRAC - 1; i++)
		{
			sons[i+1]->begin_x=sons[i]->end_x = sons[i]->begin_x + son_size[i];

		}
		int current_position[FRAC] = { 0 };
		for (int i = 0; i < father->node_size; i++)
		{
			sons[split_position[i]]->all_node[current_position[split_position[i]]] = father->all_node[i];
			sons[split_position[i]]->all_x[current_position[split_position[i]]] = father->all_x[i];
			for (int j = 0; j < FRAC; j++)
			{
				father->hint[j][i] = current_position[j];
			}
			current_position[split_position[i]]++;
		}
		for (int i = 0; i < FRAC; i++)
		{
			if (son_size[i]>NSIZE)
			{
				frac_node_queue.push(sons[i]);
			}
		}
		mini_k_heap current_heap(MINK);
		for (int i = 0; i < father->node_size; i++)
		{
			current_heap.push_back(father->all_node[i]);
		}
		delete [] father->all_x;
		make_heap(current_heap.result.begin(), current_heap.result.end());
		sort_heap(current_heap.result.begin(), current_heap.result.end());
		father->sort_rank = current_heap.result;
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
	void query_leaf(query_stack_struct current_leaf)
	{
		Range new_range=current_leaf.range;
		QuadTreeNode* new_node = current_leaf.node;
		if (new_node->begin_x == new_range.lx)
		{
			if (new_node->end_x = new_range.hx)
			{
				for (int i = new_range.ly; i < new_range.hy; i++)
				{
					int temp = points_y[i];
					if (total_size < MINK || temp < result[0])
					{
						push_heap(temp);
					}
				}
			}
			else
			{
				float lx, hx;
				lx = input_points[points_x[lx]].x;
				for (int i = new_range.ly; i < new_range.hy; i++)
				{
					int temp = points_y[i];
					float temp_x = new_node->all_x[i];
					if (temp_x>=lx&&(total_size < MINK || temp < result[0]))
					{
						push_heap(temp);
					}
				}
			}
		}
		else
		{
			if (new_node->end_x = new_range.hx)
			{
				float lx, hx;
				hx = input_points[points_x[new_range.hx]].x;
				for (int i = new_range.ly; i < new_range.hy; i++)
				{
					int temp = points_y[i];
					float temp_x = new_node->all_x[i];
					if (temp_x<hx&&(total_size < MINK || temp < result[0]))
					{
						push_heap(temp);
					}
				}
			}
			else
			{
				float lx, hx;
				lx = input_points[points_x[new_range.lx]].x;
				hx = input_points[points_x[new_range.hx]].x;
				for (int i = new_range.ly; i < new_range.hy; i++)
				{
					int temp = points_y[i];
					float temp_x = new_node->all_x[i];
					if (temp_x >= lx && temp_x<hx&&(total_size < MINK || temp < result[0]))
					{
						push_heap(temp);
					}
				}
			}
		}
	}
	void search_hint(Range rect)
	{
		query_stack_struct temp;
		temp.node = head;
		temp.range = rect;
		task_queue[task_count++] = temp;
		while (task_count)
		{
			temp = task_queue[task_count - 1];
			task_count--;
			auto cur_node = temp.node;
			auto cur_range = temp.range;
			if (cur_node->begin_x == cur_range.lx&&cur_node->end_x == cur_range.hx&&0 == cur_range.ly&&cur_node->node_size-1 == cur_range.hy)
			{
				int all_size = cur_node->sort_rank.size();
				for (int i = 0; i < all_size; i++)
				{
					int temp = cur_node->sort_rank[i];
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
			if (!cur_node->son_nodes[0])
			{
				query_leaf(temp);
				continue;
			}
			for (int i = 0; i < FRAC; i++)
			{
				Range new_range;
				query_stack_struct new_query;
				auto temp = cur_node->son_nodes[i];
				int ly, lx, hy, hx;
				ly = temp->hint[i][cur_range.ly];
				hy = temp->hint[i][cur_range.hy];
				lx = temp->begin_x;
				hx = temp->end_x;
				new_range.lx = lx > cur_range.lx ? lx : cur_range.lx;
				new_range.hx = hx < cur_range.hx ? hx : cur_range.hx;
				new_range.ly = ly;
				new_range.hy = hy;
				if (new_range.lx < new_range.hx&&new_range.ly < new_range.hy)
				{
					new_query.range = new_range;
					new_query.node = cur_node->son_nodes[i];
					task_queue[task_count++] = new_query;
				}

			}
			std::make_heap(result, result + total_size);
			std::sort_heap(result, result + total_size);
		}
	}
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
		search_hint(cur_range);
		std::vector<Point> point_result;
		point_result.reserve(total_size);
		for (int i = 0; i < total_size; i++)
		{
			point_result.push_back(input_points[result[i]]);
		}
		return point_result;
	}
	void clear_memory()
	{
		if (point_number != 0)
		{
			frac_node_queue.push(head);
			while (!frac_node_queue.empty())
			{
				auto current_front = frac_node_queue.front();
				frac_node_queue.pop();
				if (current_front->son_nodes[0] != nullptr)
				{
					for (auto i : current_front->son_nodes)
					{
						frac_node_queue.push(i);
					}

				}
				else
				{
					delete [] current_front->all_x;
				}
				delete(current_front);
			}
		}

	}
};