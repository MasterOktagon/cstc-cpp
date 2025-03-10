#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include "../parser.hpp"
#include <string>
#include "var.hpp"
#include "type.hpp"

VarDeclAST::VarDeclAST(std::string name, AST* type){
    this->name = name;
    this->type = type;
}

AST* VarDeclAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 3) return nullptr;
    if (tokens[tokens.size()-1].type == lexer::Token::TokenType::END_CMD){
        std::string name = "";
        if (tokens[tokens.size()-2].type == lexer::Token::TokenType::ID){
            name = tokens[tokens.size()-2].value;
            AST* type = Type::parse(parser::subvector(tokens, 0,1,tokens.size()-2), local, sr);
            if (type != nullptr){
                if (!parser::isAtomic(type->get_type())){
                    parser::error("Unknown type", tokens[0], tokens[tokens.size()-3], "A type of this name is unknown in this scope", 19);
                    return new AST;
                }
                if (sr->find(name) != ""){
                    parser::error("Variable already defined", tokens[tokens.size()-2], "A variable of this name is already defined in this scope", 25);
                    return new AST;
                }
                if (!parser::is_snake_case(name)){
                    parser::warn("Wrong casing", tokens[tokens.size()-2], "Variable name should be snake_case", 16);
                }

                sr->add(name, new symbol::Var(name, type->get_type()));
                return new VarDeclAST(name, type);
            }
        }
    }
    return nullptr;
}


