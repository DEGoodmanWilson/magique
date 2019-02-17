#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#pragma once

// http://coliru.stacked-crooked.com/a/0d56f604931a7441
// https://stackoverflow.com/questions/5288396/c-ostream-out-manipulation/5289170#5289170

namespace TextUtils {
/*! note: delimiter cannot contain NUL characters
 */
template <typename Range, typename Value = typename Range::value_type>
std::string Join(Range const& elements, const char *const delimiter) {
    std::ostringstream os;
    auto b = begin(elements), e = end(elements);

    if (b != e) {
        std::copy(b, prev(e), std::ostream_iterator<Value>(os, delimiter));
        b = prev(e);
    }
    if (b != e) {
        os << *b;
    }

    return os.str();
}

/*! note: imput is assumed to not contain NUL characters
 */
template <typename Input, typename Output, typename Value = typename Output::value_type>
void Split(char delimiter, Output &output, Input const& input) {
    using namespace std;
    for (auto cur = begin(input), beg = cur; ; ++cur) {
        if (cur == end(input) || *cur == delimiter || !*cur) {
            output.insert(output.end(), Value(beg, cur));
            if (cur == end(input) || !*cur)
                break;
            else
                beg = next(cur);
        }
    }
}




// https://thispointer.com/how-to-remove-substrings-from-a-string-in-c/

/*
 * Erase First Occurrence of given  substring from main string.
 */
void eraseSubStr(std::string & mainStr, const std::string & toErase);

/*
 * Erase all Occurrences of given substring from main string.
 */
void eraseAllSubStr(std::string & mainStr, const std::string & toErase);

/*
 * Erase all Occurrences of all given substrings from main string using C++11 stuff
 */
void eraseSubStrings(std::string & mainStr, const std::vector<std::string> & strList);

} // namespace TextUtils