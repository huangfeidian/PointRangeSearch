#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;
vector<int> hehe = { 2, 5, 7, 8, 12, 15, 17, 21, 33, 41, 52, 58, 67, 93 };
int get_y_upper(float IN_y)
{
	auto upper_bound = std::upper_bound(hehe.cbegin(), hehe.cend(), IN_y, [&](int a, float b)->bool
	{
		if (a < b)
		{
			return true;
		}
		else
		{
			return false;
		}
	});
	return std::distance(hehe.cbegin(), upper_bound);
}
int main()
{
	vector<int> hehe = { 2, 5, 7, 8, 12, 15, 17, 21, 33, 41, 52, 58, 67, 93 };
	float c;
	cin >> c;
	while (c > 0)
	{
		cout << get_y_upper(c) << endl;
		cin >> c;
	}

}