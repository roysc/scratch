#include <vector>

#include "util.hh"
#include "range.hh"

int main()
{
    int nums[] = { 0, -1, 4, -3, 5, 8, -2 };
    const int N = sizeof(nums)/sizeof(int);

    auto pos = range::filter(nums, nums + N, [] (int x) { return 0 < x; });

    util::println(pos);
}
