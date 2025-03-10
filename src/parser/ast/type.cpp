#include "type.hpp"
#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include "../parser.hpp"
#include <string>
#include "var.hpp"

TypeAST::TypeAST(std::string name){
    this->name = name;
}

AST* TypeAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() != 1) return nullptr;
    if (tokens[0].type == lexer::Token::TokenType::ID) {
        return new TypeAST(tokens[0].value);
    }
    return nullptr;
}

AST* Type::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
        return parser::parseOneOf(tokens, {
        TypeAST::parse
        }, local, sr, expected_type);
}