#include "point_search.h"
#include <vector>

#include <malloc.h>
#include <memory>
#include <immintrin.h>
#include <algorithm>
using namespace std;
#define FRAC 4
#define MINK 20
#define NSIZE 1024
#define _mm256_extract_epi32(mm, i) _mm_extract_epi32(_mm256_extractf128_si256(mm, i >= 4), i % 4)
struct SearchContext
{

public:
	std::vector<Point> input_points;
	float* aligned_x;
	float* aligned_y;
	//int* aligned_index;
	const int point_number;
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
		//aligned_index = (int*) _mm_malloc(sizeof(int)*point_number, 32);
		aligned_x = (float*) _mm_malloc(sizeof(float)*point_number, 64);
		aligned_y = (float*) _mm_malloc(sizeof(float)*point_number, 64);
		for (int i = 0; i < point_number; i++)
		{
			aligned_x[i] = input_points[i].x;
			aligned_y[i] = input_points[i].y;
		}


	}

public:
	vector<int> linear_search(Rect range, int nth)
	{
		int result_size = 0;
		vector<int> result;
		for (int i = 0; i < point_number; i++)
		{
			if (input_points[i].x <= range.hx&&input_points[i].x >= range.lx&&input_points[i].y <= range.hy&&input_points[i].y >= range.ly)
			{
				result.push_back(i);
				result_size++;
				if (result_size == nth)
				{
					break;
				}
			}
		}
		return result;
	}
	vector<int> avx_linear_search(Rect rect, int count)
	{
		__m256 rect_lx = _mm256_broadcast_ss(&rect.lx);
		__m256 rect_hx = _mm256_broadcast_ss(&rect.hx);
		__m256 rect_ly = _mm256_broadcast_ss(&rect.ly);
		__m256 rect_hy = _mm256_broadcast_ss(&rect.hy);
		vector<int> result;
		int n = 0;
		for (int i = 0; i < point_number; i += 8)
		{
			__m256 x = _mm256_load_ps(aligned_x + i);
			__m256 y = _mm256_load_ps(aligned_y + i);

			__m256 x_in = _mm256_and_ps(_mm256_cmp_ps(rect_lx, x, _CMP_LE_OQ),
				_mm256_cmp_ps(rect_hx, x, _CMP_GE_OQ));
			__m256 y_in = _mm256_and_ps(_mm256_cmp_ps(rect_ly, y, _CMP_LE_OQ),
				_mm256_cmp_ps(rect_hy, y, _CMP_GE_OQ));

			if (!_mm256_testz_ps(x_in, y_in))
			{
				__m256i mask = _mm256_castps_si256(_mm256_and_ps(x_in, y_in));
				if (_mm256_extract_epi32(mask, 0))
				{
					result.push_back(i + 0);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 1))
				{
					result.push_back(i + 1);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 2))
				{
					result.push_back(i + 2);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 3))
				{
					result.push_back(i + 3);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 4))
				{
					result.push_back(i + 4);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 5))
				{
					result.push_back(i + 5);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 6))
				{
					result.push_back(i + 6);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 7))
				{
					result.push_back(i + 7);
					n++;
					if (n == count) return result;
				}
			}
		}
		return result;
	}
	vector<int> avx_pre_linear_search(Rect rect, int count)
	{
		__m256 rect_lx = _mm256_broadcast_ss(&rect.lx);
		__m256 rect_hx = _mm256_broadcast_ss(&rect.hx);
		__m256 rect_ly = _mm256_broadcast_ss(&rect.ly);
		__m256 rect_hy = _mm256_broadcast_ss(&rect.hy);
		vector<int> result;
		int n = 0;
		for (int i = 0; i < point_number; i += 8)
		{
			__m256 x = _mm256_load_ps(aligned_x + i);
			__m256 y = _mm256_load_ps(aligned_y + i);
			_mm_prefetch((char*) (aligned_x + i) + 64, _MM_HINT_T0);
			_mm_prefetch((char*) (aligned_y + i) + 64, _MM_HINT_T0);
			__m256 x_in = _mm256_and_ps(_mm256_cmp_ps(rect_lx, x, _CMP_LE_OQ),
				_mm256_cmp_ps(rect_hx, x, _CMP_GE_OQ));
			__m256 y_in = _mm256_and_ps(_mm256_cmp_ps(rect_ly, y, _CMP_LE_OQ),
				_mm256_cmp_ps(rect_hy, y, _CMP_GE_OQ));

			if (!_mm256_testz_ps(x_in, y_in))
			{
				__m256i mask = _mm256_castps_si256(_mm256_and_ps(x_in, y_in));
				if (_mm256_extract_epi32(mask, 0))
				{
					result.push_back(i + 0);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 1))
				{
					result.push_back(i + 1);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 2))
				{
					result.push_back(i + 2);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 3))
				{
					result.push_back(i + 3);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 4))
				{
					result.push_back(i + 4);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 5))
				{
					result.push_back(i + 5);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 6))
				{
					result.push_back(i + 6);
					n++;
					if (n == count) return result;
				}
				if (_mm256_extract_epi32(mask, 7))
				{
					result.push_back(i + 7);
					n++;
					if (n == count) return result;
				}
			}
		}
		return result;
	}
	std::vector<Point> stack_query(Rect range, int nth)
	{
		std::vector<Point> point_result;
		if (point_number == 0)
		{
			return std::vector<Point>();
		}
		std::vector<int> result_index = avx_linear_search(range, nth);
		for (auto i : result_index)
		{
			point_result.push_back(input_points[i]);
		}
		return point_result;
	}
	void clear_memory()
	{
		if (point_number != 0)
		{
			_mm_free(aligned_x);
			_mm_free(aligned_y);
		}

	}
};
//extern "C" __declspec(dllexport)SearchContext* __stdcall create(const Point* points_begin, const Point* points_end)
//{
//	return new SearchContext(points_begin, points_end);
//}
//
//extern "C" __declspec(dllexport)int32_t __stdcall search(SearchContext* sc, const Rect rect, const int32_t count, Point* out_points)
//{
//	auto result_vector = sc->stack_query(rect, count);
//	for (auto i : result_vector)
//	{
//		*(out_points) = i;
//		out_points++;
//	}
//	return result_vector.size();
//}
//
//extern "C" __declspec(dllexport)SearchContext* __stdcall destroy(SearchContext* sc)
//{
//	sc->clear_memory();
//	delete(sc);
//	return nullptr;
//}