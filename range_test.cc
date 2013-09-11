#include <vector>

#include "util.hh"
#include "range.hh"

int main()
{
    int nums[] = { 0, -1, 4, -3, 5, 8, -2 };
    const int N = sizeof(nums)/sizeof(int);

    auto is_pos = [] (int x) { return 0 < x; };
    auto filt = range::filter(is_pos, nums, nums + N);
    // auto filt_end = filter(is_pos, nums + N, nums + N);

    // for (auto it = filt.begin(); it != filt.end(); ++it)
    for (auto x : filt)
        util::print(x, ' ');
    util::println();
}
