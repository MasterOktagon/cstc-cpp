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

std::string insert(std::string val, std::string target){
    size_t pos = target.find_first_of("{}");
    if (pos != std::string::npos){
        target.replace(pos, 2, val);
    }
    return target;
}

std::string rinsert(std::string val, std::string target){
    size_t pos = target.rfind("{}");
    if (pos != std::string::npos){
        target.replace(pos, 2, val);
    }
    return target;
}