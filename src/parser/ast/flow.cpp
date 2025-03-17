#include <string>
#include "flow.hpp"
#include "ast.hpp"
#include "base_math.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
//#include <iostream>
#include "../parser.hpp"
#include "var.hpp"

std::string SubBlockAST::emit_cst(){
    std::string ret = "";
    for (AST* a : contents) {
        ret += a->emit_cst();
        if (a == dynamic_cast<ExpressionAST*>(a)){
            ret += ";"; // Expressions don't End on semicolons, therefore add one
        }
        ret += "\n";
    }
    return ret;
}

AST* SubBlockAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string){
    if (tokens.size() == 0) return new SubBlockAST;
    std::vector<AST*> contents;

    while (tokens.size() > 0){
        int split = parser::rsplitStack(tokens, {lexer::Token::TokenType::END_CMD, lexer::Token::TokenType::BLOCK_CLOSE}, local);
        std::vector<lexer::Token> buffer = parser::subvector(tokens, 0,1,split+1);
        AST* expr = parser::parseOneOf(buffer, {
            VarInitlAST::parse,
            VarDeclAST::parse,
            parseStatement,
        }, local, sr, "void");

        if (expr == nullptr){
            parser::error("Expected expression", tokens[0], tokens[split], "Expected a valid expression (Did you forget a ';'?)", 31);
        }
        else {
            contents.push_back(expr);
        }
        tokens = parser::subvector(tokens, split+1,1,tokens.size());
    }
    auto b = new SubBlockAST();
    b->contents = contents;

    return b;
}

