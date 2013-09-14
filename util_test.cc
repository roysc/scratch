#include <vector>
#include <array>

#include "util.hh"

using namespace util;

struct S { std::string to_string() { return std::string("ess"); }};

int main()
{
    std::vector<int> v {1,2,3,4,5};
    std::array<int, 5> a {1,2,3,4};
    println(v, a);

    // using util::io::to_string;
    S s;
    std::cout << to_string(s) << '\n';
    std::cout << s << '\n';
    println(s);
    
}
