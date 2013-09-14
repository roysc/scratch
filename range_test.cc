#include <vector>
#include <typeinfo>

#include "util.hh"
#include "range.hh"

using namespace util::io;

int main()
{
    int nums[] = { 0, -1, 4, -3, 5, 8, -2 };
    const int N = sizeof(nums)/sizeof(int);

    auto pos = range::filter([] (int x) { return 0 < x; },
                             nums, nums + N);
    println(pos);

    std::vector<std::string> words = {
        "and", "a", "one", "and a", "two", "& a", "1 2 3"
    };
    println(range::filter([] (std::string s) { return s.size() == 3; },
                          words.begin(), words.end()));
    
    std::vector<char> chars {'a', 'b', 'x', 'y'};
    println(chars);


    // print(std::boolalpha);
    

}
