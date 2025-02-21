#pragma once
#include <vector>
#include <string>
#include "../lexer/lexer.hpp"
#include "ast/ast.hpp"
#include "symboltable.hpp"

template <typename T, typename ... K>
using fsignal = T (*) (K ... );

namespace parser {
    extern unsigned int errc;
    extern bool one_error;

    void error(std::string name, lexer::Token pos, std::string msg, int code, std::string appendix = "");
    void error(std::string name, lexer::Token from, lexer::Token to, std::string msg, int code, std::string appendix = "");
    void warn(std::string name, lexer::Token pos, std::string msg, int code);
    void warn(std::string name, lexer::Token from, lexer::Token to, std::string msg, int code);

    extern int splitStack(std::vector<lexer::Token> tokens, std::initializer_list<lexer::Token::TokenType>, int local, std::initializer_list<lexer::Token::TokenType> = {});
    extern int rsplitStack(std::vector<lexer::Token> tokens, std::initializer_list<lexer::Token::TokenType>, int local, std::initializer_list<lexer::Token::TokenType> = {});
    extern AST* parseOneOf(std::vector<lexer::Token> tokens, std::vector<fsignal<AST*, std::vector<lexer::Token>, int, symbol::Namespace*, std::string>> functions, int local, symbol::Namespace* sr, std::string expected_type);

    template <typename T>
    std::vector<T> subvector(std::vector<T> v, int start, int step, int stop){
        auto s = v.begin() + start; auto end = v.begin() + stop;
        return std::vector<T>(s, end);
    }

    extern bool typeEq(std::string a, std::string b);
    extern std::string hasOp(std::string type1, std::string type2, lexer::Token::TokenType op);
    extern bool isAtomic(std::string type);
}