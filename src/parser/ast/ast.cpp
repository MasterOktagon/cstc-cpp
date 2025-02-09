#include "ast.hpp"
//#include <cstddef>
#include <string>

std::string max_prec_type(std::string a, std::string b){
    if (a == "@unknown") return b;
    if (b == "@unknown") return a;
    if (a == "@int") return b;
    if (b == "@int") return a;
    if (a == "@float") return b;
    if (b == "@float") return a;

    return a;
}

std::string intab(std::string i){
    //int j = 0;
    size_t pos = i.find('\n', 0);
    while (pos != std::string::npos){
        i.replace(pos, 1, "\n\t");
        pos = i.find('\n', pos+1);
    }
    return std::string("\t") + i;
}