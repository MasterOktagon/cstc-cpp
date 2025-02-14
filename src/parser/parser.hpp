#pragma once
#include <vector>
#include <string>
#include "../lexer/lexer.hpp"

namespace parser {
    extern unsigned int errc;

    void error(std::string name, lexer::Token pos, std::string msg, int code);
    void error(std::string name, lexer::Token from, lexer::Token to, std::string msg, int code);
    void warn(std::string name, lexer::Token pos, std::string msg, int code);
    void warn(std::string name, lexer::Token from, lexer::Token to, std::string msg, int code);

    extern int splitStack(std::vector<lexer::Token> tokens, std::initializer_list<lexer::Token::TokenType>, int local);
    extern int rsplitStack(std::vector<lexer::Token> tokens, std::initializer_list<lexer::Token::TokenType>, int local);

    template <typename T>
    std::vector<T> subvector(std::vector<T> v, int start, int step, int stop){
        auto s = v.begin() + start; auto end = v.begin() + stop;
        return std::vector<T>(s, end);
    }

    bool hasOp(std::string type1, std::string type2, std::string op);
}