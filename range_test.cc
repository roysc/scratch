#include <vector>
#include <typeinfo>
#include <iterator>

#include <cassert>

#include "util/range.hh"
#include "util/util.hh"

#include "util/bench.hh"

template <class T>
struct is_itbl
{
  template <class T_ = std::decay_t<T>,
            class B = decltype(std::begin(std::declval<T_>())),
            class E = decltype(std::end(std::declval<T_>()))> 
  static std::enable_if_t<
    std::is_same<B, E>::value,
    std::true_type> test(int);
  static std::false_type test(...);
  static const bool value = decltype(test(0))::value;
};

int
main()
{
  // using namespace util;
  using namespace range;
  
  int nums[] = { 0, -1, 4, -3, 5, 8, -2 };
  const int N = sizeof(nums)/sizeof(int);

  auto is_pos = [] (int x) { return 0 < x; };
  auto dub = [] (int x) { return x * 2; };

  auto filt = filter(is_pos, nums, nums + N);
  auto mapt = map(dub, filt);

  // char x = std::begin(nums);
  
  // auto isi = util::is_iterable<decltype(filt)>::value;
  auto isi = is_itbl<decltype(filt)>::value;
  assert(isi);
  auto isr = range::is_range<decltype(filt)>::value;
  assert(isr);
  
  // std::cout << filt << '\n';
  
  // util::println(filter(is_pos, nums, nums + N));
  // util::println(map(dub, filter(ispos, nums, nums + N)));
    
  // std::vector<std::string> words = {
  //   "and", "a", "one", "and a", "two", "& a", "1 2 3"
  // };
  // println(filter([] (std::string s) { return s.size() == 3; }, as_range(words)));
  // words.begin(), words.end()));
    
  // std::vector<char> chars {'a', 'b', 'x', 'y'};
  // println(chars);

  // auto c2 = map([] (char c) -> char { return c + 1; },
  //               // range::as_range(chars));
  //               chars.begin(), chars.end());
  // println(c2);

  // print(std::boolalpha);
    
}
