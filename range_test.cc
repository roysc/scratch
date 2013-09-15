#include <vector>
#include <typeinfo>
#include <cassert>

#include "range.hh"
#include "util.hh" // FIXME: this must be included after range.hh for << to work

using namespace util::io;
using namespace range;

int main()
{
    int nums[] = { 0, -1, 4, -3, 5, 8, -2 };
    const int N = sizeof(nums)/sizeof(int);

    auto pos = range::filter([] (int x) { return 0 < x; },
                             nums, nums + N);
    println(pos);

    auto numsr = as_range(nums, nums + N);
    println(filter([] (int x) { return 0 < x; }, numsr));
    
    std::vector<std::string> words = {
        "and", "a", "one", "and a", "two", "& a", "1 2 3"
    };
    println(range::filter([] (std::string s) { return s.size() == 3; },
                          words.begin(), words.end()));
    
    std::vector<char> chars {'a', 'b', 'x', 'y'};
    println(chars);


    // print(std::boolalpha);

    auto isr = range::is_range<decltype(pos)>::value;
    assert(isr && "is_iterable<FilterRange>");

    auto isi = util::is_iterable<decltype(pos)>::value;
    assert(isi && "is_range<FilterRange>");
}
