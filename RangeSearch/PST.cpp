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
#include <immintrin.h>
using namespace std;
#define FRAC 4
#define MINK 20
#define NSIZE 256
#define MASK 4095
//#define File_DBG
#define _mm256_extract_epi32(mm, i) _mm_extract_epi32(_mm256_extractf128_si256(mm, i >= 4), i % 4)
struct PST
{
private:

	struct QuadTreeNode
	{
		float begin_x;
		float end_x;
		float begin_y;
		float end_y;
		int node_begin;
		int node_end;
		float* aligned_allx;
		float* aligned_ally;
		int* aligned_index;
		float split_value;
		int node_size;
		bool is_x;
		QuadTreeNode* left;
		QuadTreeNode* right;
		QuadTreeNode()
		{
			left = nullptr;
			right = nullptr;
		}
#ifdef File_DBG
		int father_id;
		int my_id;
#endif
	};




private:

	int result[MINK];
	QuadTreeNode* head;
	const int point_number;
	int total_size;
	int id;
	queue<QuadTreeNode*> frac_node_queue;
	stack<QuadTreeNode*> task_queue;
	int tail_count;
	int head_count;
	int* all_node;
	//rank_queue task_queue;
public:
	std::vector<Point> input_points;
	PST(const Point* begin, const Point* end) :point_number(end - begin)
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
		all_node = (int*) malloc(sizeof(int)*point_number);//256bit aligned
		head = new QuadTreeNode();
		head->node_size = point_number;
		head->node_begin = 0;
		head->node_end = point_number;
		for (int i = 0; i < point_number; i++)
		{
			all_node[i] = i;
		}
		auto min_max_x = std::minmax_element(std::begin(input_points), std::end(input_points), [](const Point& a, const Point& b)->bool
		{
			return a.x < b.x;
		});
		auto min_max_y = std::minmax_element(std::begin(input_points), std::end(input_points), [](const Point& a, const Point& b)->bool
		{
			return a.y < b.y;
		});
		head->begin_x = min_max_x.first->x;
		head->end_x = min_max_x.second->x;
		head->begin_y = min_max_y.first->y;;
		head->end_y = min_max_y.second->y;
		head->is_x = true;
		if (point_number >NSIZE)
		{
			stack_split(head);
		}
		free(all_node);
		all_node = nullptr;
	}
private:
	//inline int get_x_lower(float IN_x)const
	//{
	//	auto lower_bound = std::lower_bound(points_x.cbegin(), points_x.cend(), IN_x, [&](int a, float b)->bool
	//	{
	//		return input_points[a].x < b;
	//	});
	//	return std::distance(points_x.cbegin(), lower_bound);
	//}
	//inline int get_x_upper(float IN_x)const
	//{
	//	auto upper_bound = std::upper_bound(points_x.cbegin(), points_x.cend(), IN_x, [&](float b, int a)->bool
	//	{
	//		return b<input_points[a].x;
	//	});
	//	return std::distance(points_x.cbegin(), upper_bound);
	//}
	//inline int get_y_upper(float IN_y)const
	//{
	//	auto upper_bound = std::upper_bound(points_y.cbegin(), points_y.cend(), IN_y, [&](float b, int a)->bool
	//	{
	//		return b<input_points[a].y;
	//	});
	//	return std::distance(points_y.cbegin(), upper_bound);
	//}
	//inline int get_y_lower(float IN_y)const
	//{

	//	auto lower_bound = std::lower_bound(points_y.cbegin(), points_y.cend(), IN_y, [&](int a, float b)->bool
	//	{
	//		return input_points[a].y < b;
	//	});
	//	return std::distance(points_y.cbegin(), lower_bound);
	//}

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
			if (temp->node_size < NSIZE)
			{
				temp->aligned_allx = (float*) _mm_malloc(sizeof(float)*(temp->node_size), 32);
				temp->aligned_ally = (float*) _mm_malloc(sizeof(float)*(temp->node_size), 32);
				temp->aligned_index = (int*) _mm_malloc(sizeof(int)*(temp->node_size), 32);
				temp->left = nullptr;
				temp->right = nullptr;
				std::sort(all_node + temp->node_begin, all_node + temp->node_end);
				for (int i = 0; i < temp->node_size; i++)
				{
					temp->aligned_index[i] = all_node[temp->node_begin + i];
					temp->aligned_allx[i] = input_points[all_node[temp->node_begin + i]].x;
					temp->aligned_ally[i] = input_points[all_node[temp->node_begin + i]].y;
				}
			}
			else
			{
				split(temp);
			}
		}
#ifdef File_DBG
		dbg_file.close();
#endif
	}
	inline void push_heap(int temp)
	{
		if (total_size < MINK)
		{
			result[total_size++] = temp;
			if (total_size == MINK)
			{
				std::sort(result, result + MINK);
			}
		}
		else
		{
			result[MINK-1] = temp;
			int i = MINK - 1;
			while (i > 0 && result[i - 1]>result[i])
			{
				std::swap(result[i - 1], result[i]);
				i--;
			}
		}
	}
	void split(QuadTreeNode* father)
	{
		std::partial_sort(all_node + father->node_begin, all_node + father->node_begin + NSIZE, all_node + father->node_end);
		//vector<int> unique_x, unique_y;
		//unique_x.reserve(father->node_end - father->node_begin - 32);
		//unique_y.reserve(father->node_end - father->node_begin - 32);
		//unique_copy(all_node + father->node_begin + 32, all_node + father->node_end, back_inserter(unique_x), [&](int a, int b)->bool
		//{
		//	return input_points[a].x < input_points[b].x;
		//});
		//unique_copy(all_node + father->node_begin + 32, all_node + father->node_end, back_inserter(unique_y), [&](int a, int b)->bool
		//{
		//	return input_points[a].y < input_points[b].y;
		//});
		int* partition_position;
		father->aligned_allx = (float*) _mm_malloc(sizeof(float) * NSIZE, 32);
		father->aligned_ally = (float*) _mm_malloc(sizeof(float) * NSIZE, 32);
		father->aligned_index = (int*) _mm_malloc(sizeof(int) * NSIZE, 32);
		for (int i = 0; i < NSIZE; i++)
		{
			father->aligned_index[i] = all_node[father->node_begin + i];
			father->aligned_allx[i] = input_points[all_node[father->node_begin + i]].x;
			father->aligned_ally[i] = input_points[all_node[father->node_begin + i]].y;

		}
		if (father->is_x)
		{
			std::nth_element(all_node + father->node_begin + NSIZE, all_node + (father->node_end + father->node_begin + NSIZE) / 2, all_node + father->node_end, [&](int a, int b)->bool
			{
				return input_points[a].x < input_points[b].x;
			});
			father->split_value = input_points[*(all_node + (father->node_end + father->node_begin + NSIZE) / 2)].x;
			partition_position = std::partition(all_node + father->node_begin + NSIZE, all_node + father->node_end, [&](int a)->bool
			{
				return input_points[a].x < father->split_value;
			});
		}
		else
		{
			std::nth_element(all_node + father->node_begin + NSIZE, all_node + (father->node_end + father->node_begin + NSIZE) / 2, all_node + father->node_end, [&](int a, int b)->bool
			{
				return input_points[a].y< input_points[b].y;
			});
			father->split_value = input_points[*(all_node + (father->node_end + father->node_begin + NSIZE) / 2)].y;
			partition_position = std::partition(all_node + father->node_begin + NSIZE, all_node + father->node_end, [&](int a)->bool
			{
				return input_points[a].y < father->split_value;
			});
		}
		father->left = new QuadTreeNode();
		father->right = new QuadTreeNode();
		father->left->node_begin = father->node_begin + NSIZE;
		father->left->node_end = partition_position - all_node;
		father->left->node_size = father->left->node_end - father->left->node_begin;
		auto min_max_x = std::minmax_element(all_node + father->left->node_begin, all_node + father->left->node_end, [&](int a, int b)->bool
		{
			return input_points[a].x < input_points[b].x;
		});
		auto min_max_y = std::minmax_element(all_node + father->left->node_begin, all_node + father->left->node_end, [&](int a, int b)->bool
		{
			return input_points[a].y < input_points[b].y;
		});
		father->left->begin_x = input_points[*min_max_x.first].x;
		father->left->end_x = input_points[*min_max_x.second].x;
		father->left->begin_y = input_points[*min_max_y.first].y;
		father->left->end_y = input_points[*min_max_y.second].y;
		father->right->node_begin = partition_position - all_node;
		father->right->node_end = father->node_end;
		father->right->node_size = father->right->node_end - father->right->node_begin;
		min_max_x = std::minmax_element(all_node + father->right->node_begin, all_node + father->right->node_end, [&](int a, int b)->bool
		{
			return input_points[a].x < input_points[b].x;
		});
		min_max_y = std::minmax_element(all_node + father->right->node_begin, all_node + father->right->node_end, [&](int a, int b)->bool
		{
			return input_points[a].y < input_points[b].y;
		});
		father->right->begin_x = input_points[*min_max_x.first].x;
		father->right->end_x = input_points[*min_max_x.second].x;
		father->right->begin_y = input_points[*min_max_y.first].y;
		father->right->end_y = input_points[*min_max_y.second].y;
		father->node_size = NSIZE;
		father->left->is_x = father->right->is_x = !father->is_x;
		frac_node_queue.push(father->left);
		frac_node_queue.push(father->right);
	}
	void linear_search(Rect range, int nth)
	{
		total_size = 0;
		for (int i = 0; i < point_number; i++)
		{
			if (input_points[i].x <= range.hx&&input_points[i].x >= range.lx&&input_points[i].y <= range.hy&&input_points[i].y >= range.ly)
			{
				result[total_size++] = i;
				if (total_size == nth)
				{
					break;
				}
			}
		}
	}
public:
	void stack_normal_query_MINK(Rect rect)
	{
		
		total_size = 0;
		head_count = tail_count = 0;
		QuadTreeNode* temp = head;
		task_queue.push(head);
		while (!task_queue.empty())
		{
			temp = task_queue.top();
			task_queue.pop();
			if (temp->begin_x > rect.hx || temp->begin_y > rect.hy || temp->end_x < rect.lx || temp->end_y < rect.ly || *(temp->aligned_index + 0)>result[MINK - 1])
			{
				continue;
			}
			else
			{
				__m256 rect_lx = _mm256_broadcast_ss(&rect.lx);
				__m256 rect_hx = _mm256_broadcast_ss(&rect.hx);
				__m256 rect_ly = _mm256_broadcast_ss(&rect.ly);
				__m256 rect_hy = _mm256_broadcast_ss(&rect.hy);
				__m256 m_zero = _mm256_setzero_ps();
				int block_size = temp->node_size / 8;
				for (int i = 0; i < block_size; i++)
				{
					//__m256 result_rear = _mm256_broadcast_ss((float*)(result) + MINK-1);
					__m256 x = _mm256_load_ps(temp->aligned_allx + i * 8);
					__m256 y = _mm256_load_ps(temp->aligned_ally + i * 8);
					//__m256 rank = _mm256_load_ps((float*)(temp->aligned_index + i * 8));
					__m256 x_in = _mm256_and_ps(_mm256_cmp_ps(rect_lx, x, _CMP_LE_OQ), _mm256_cmp_ps(rect_hx, x, _CMP_GE_OQ));
					__m256 y_in = _mm256_and_ps(_mm256_cmp_ps(rect_ly, y, _CMP_LE_OQ), _mm256_cmp_ps(rect_hy, y, _CMP_GE_OQ));
					//__m256 rank_in = _mm256_cmp_ps(rank, result_rear,_CMP_LE_OQ);
					__m256 result_in = _mm256_and_ps(x_in, y_in);
					if (!_mm256_testz_ps(x_in, y_in))
					{
						__m256i mask = _mm256_castps_si256(_mm256_and_ps(x_in, y_in));
						if (_mm256_extract_epi32(mask, 0) && *(temp->aligned_index + i * 8 + 0) <result[MINK-1])
						{
							push_heap(*(temp->aligned_index + i * 8 + 0));
						}
						if (_mm256_extract_epi32(mask, 1) && *(temp->aligned_index + i * 8 + 1) <result[MINK - 1])
						{
							push_heap(*(temp->aligned_index + i * 8 + 1));
						}
						if (_mm256_extract_epi32(mask, 2) && *(temp->aligned_index + i * 8 + 2) <result[MINK - 1])
						{
							push_heap(*(temp->aligned_index + i * 8 + 2));
						}
						if (_mm256_extract_epi32(mask, 3) && *(temp->aligned_index + i * 8 + 3) <result[MINK - 1])
						{
							push_heap(*(temp->aligned_index + i * 8 + 3));
						}
						if (_mm256_extract_epi32(mask, 4) && *(temp->aligned_index + i * 8 + 4) <result[MINK - 1])
						{
							push_heap(*(temp->aligned_index + i * 8 + 4));
						}
						if (_mm256_extract_epi32(mask, 5) && *(temp->aligned_index + i * 8 + 5) <result[MINK - 1])
						{
							push_heap(*(temp->aligned_index + i * 8 + 5));
						}
						if (_mm256_extract_epi32(mask, 6) && *(temp->aligned_index + i * 8 + 6) <result[MINK - 1])
						{
							push_heap(*(temp->aligned_index + i * 8 + 6));
						}
						if (_mm256_extract_epi32(mask, 7) && *(temp->aligned_index + i * 8 + 7) <result[MINK - 1])
						{
							push_heap(*(temp->aligned_index + i * 8 + 7));
						}
					}
				}
				for (int i = 8 * block_size; i < temp->node_size; i++)
				{
					if (temp->aligned_allx[i] >= rect.lx&&temp->aligned_allx[i] <= rect.hx&&temp->aligned_ally[i] >= rect.ly&&temp->aligned_ally[i] <= rect.hy&&temp->aligned_index[i] <result[MINK - 1])
					{
						push_heap(temp->aligned_index[i]);
					}
				}
				if (temp->left != nullptr)
				{
					task_queue.push(temp->left);
				}
				if (temp->right != nullptr)
				{
					task_queue.push (temp->right);
				}
			}
		}

	}
	std::vector<Point> stack_query(Rect range, int nth)
	{
		if (point_number == 0)
		{
			return std::vector<Point>();
		}
		for (int i = 0; i<MINK; i++)
		{
			result[i] = point_number;
		}
		try
		{
			stack_normal_query_MINK(range);
		}
		catch (const exception& e)
		{
			cout << e.what() << endl;
		}
		std::vector<Point> point_result;
		int result_size = total_size > nth ? nth : total_size;
		std::sort(result, result + result_size);
		point_result.reserve(result_size);
		for (int i = 0; i < result_size; i++)
		{
			point_result.push_back(input_points[result[i]]);
		}
		return point_result;
	}
	int32_t search(const Rect rect, const int32_t count, Point* out_points)
	{
		auto result_vector =stack_query(rect, count);
		for (auto i : result_vector)
		{
			*(out_points) = i;
			out_points++;
		}
		return result_vector.size();
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
				_mm_free(current_front->aligned_allx);
				_mm_free(current_front->aligned_ally);
				_mm_free(current_front->aligned_index);
				if (current_front->left != nullptr)
				{
					frac_node_queue.push(current_front->left);
					frac_node_queue.push(current_front->right);

				}
				delete(current_front);
			}
		}

	}
};