#ifndef GRID_STREAM_READER_H_
#define GRID_STREAM_READER_H_

#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace Grid {

namespace StreamReaderInternal {

#define sim template < class c
#define ris return * this
#define dor > StreamReader & operator <<
#define eni(x) sim > typename \
  std::enable_if<sizeof dud<c>(0) x 1, StreamReader&>::type operator<<(c i) {
sim > struct rge { c b, e; };
sim > rge<c> range(c i, c j) { return rge<c>{i, j}; }
sim > auto dud(c* x) -> decltype(std::cerr << *x, 0);
sim > char dud(...);

struct StreamReader {
StreamReader(std::function<void(const std::string&)> callback)
    : callback_(callback) {}
StreamReader(StreamReader&& sr) = default;
~StreamReader() { callback_(stream_.str()); }
eni(!=) stream_ << std::boolalpha << i; ris; }
eni(==) ris << range(std::begin(i), std::end(i)); }
sim, class b dor(std::pair < b, c > d) {
  ris << "(" << d.first << ", " << d.second << ")";
}
sim dor(rge<c> d) {
  *this << "[";
  for (auto it = d.b; it != d.e; ++it)
    *this << ", " + 2 * (it == d.b) << *it;
  ris << "]";
}
std::function<void(const std::string&)> callback_;
std::stringstream stream_;
};

#undef sim
#undef ris
#undef dor
#undef eni

}  // namespace StreamReaderInternal

using StreamReader = StreamReaderInternal::StreamReader;

}  // namespace Grid

#endif  // GRID_STREAM_READER_H_
