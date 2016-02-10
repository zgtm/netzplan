#ifndef STRNUMCONV_H
#define STRNUMCONV_H


#include <string>
#include <sstream>

template <typename T> // from http://www.cplusplus.com/forum/articles/9645/
inline std::string NumberToString(T Number) {
    std::stringstream ss;
    ss << Number;
    return ss.str();
}

template <typename T> // from http://www.cplusplus.com/forum/articles/9645/
inline T StringToNumber(const std::string &Text) {
    std::stringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}


#endif /* STRNUMCONV_H */