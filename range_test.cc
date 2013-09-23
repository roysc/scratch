#include <vector>
#include <typeinfo>
#include <cassert>

#include "range.hh"
#include "util.hh" // FIXME: this must be included after range.hh for << to work

#include "bench.hh"

using namespace util::io;
using namespace range;

int main()
{
    int nums[] = { 0, -1, 4, -3, 5, 8, -2 };
    const int N = sizeof(nums)/sizeof(int);

    auto ispos = [] (int x) { return 0 < x; };
    
    println(filter(ispos, nums, nums + N));
    println(map([] (int x) { return x * 2; }, filter(ispos, nums, nums + N)));
    
    std::vector<std::string> words = {
        "and", "a", "one", "and a", "two", "& a", "1 2 3"
    };
    println(filter([] (std::string s) { return s.size() == 3; },
                   as_range(words)));
                   // words.begin(), words.end()));
    
    std::vector<char> chars {'a', 'b', 'x', 'y'};
    println(chars);

    auto c2 = map([] (char c) -> char { return c + 1; },
                  // range::as_range(chars));
                  chars.begin(), chars.end());
    println(c2);

    // print(std::boolalpha);
    
}
