#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include "../parser.hpp"
#include <string>
#include "var.hpp"
#include "base_math.hpp"
#include "type.hpp"
#include <iostream>

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
                if (parser::isAtomic(name)){
                    parser::error("Unsupported name", tokens[tokens.size()-2], std::string("The name ") + name + " refers to a scope or type and cannot be used as a variable name", 25);
                    return new AST;
                }
                if (!parser::is_snake_case(name)){
                    parser::warn("Wrong casing", tokens[tokens.size()-2], "Variable name should be snake_case", 16);
                }

                sr->add(name, new symbol::Var(name, type->get_type(), tokens[tokens.size()-2]));
                return new VarDeclAST(name, type);
            }
        }
    }
    return nullptr;
}

VarInitlAST::VarInitlAST(std::string name, AST* type, AST* expr){
    this->name = name;
    this->type = type;
    this->expression = expr;
}

AST* VarInitlAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 5) return nullptr;
    if (tokens[tokens.size()-1].type == lexer::Token::TokenType::END_CMD){
        std::string name = "";
        int split = parser::rsplitStack(tokens, {lexer::Token::TokenType::SET}, local);
        if (tokens[split-1].type == lexer::Token::TokenType::ID){
            name  = tokens[split-1].value;
            AST* type = Type::parse(parser::subvector(tokens, 0,1,split-1), local, sr);
            if (type == nullptr){
                parser::error("Expected type", tokens[0], tokens[split-1], "Expected a type before the identifier", 25);
                return new AST;
            }
            AST* expr = math::parse(parser::subvector(tokens, split+1, 1, tokens.size()-1), local, sr, type->get_type());
            if (expr == nullptr){
                parser::error("Expected expression", tokens[split+1], tokens[tokens.size()-1], "Expected an expression", 25);
                return new AST;
            }
            if (!parser::isAtomic(type->get_type())){
                parser::error("Unknown type", tokens[0], tokens[tokens.size()-3], "A type of this name is unknown in this scope", 19);
                return new AST;
            }
            if (sr->find(name) != ""){
                parser::error("Variable already defined", tokens[tokens.size()-2], "A variable of this name is already defined in this scope", 25);
                return new AST;
            }
            if (parser::isAtomic(name)){
                parser::error("Unsupported name", tokens[tokens.size()-2], std::string("The name ") + name + " refers to a scope or type and cannot be used as a variable name", 25);
                return new AST;
            }
            if (!parser::is_snake_case(name)){
                parser::warn("Wrong casing", tokens[split-1], "Variable name should be snake_case", 16);
            }
            expr->force_type(type->get_type());

            sr->add(name, new symbol::Var(name, type->get_type(), tokens[split-1]));
            return new VarInitlAST(name, type, expr);
        }
    }
    return nullptr;
}

VarAccesAST::VarAccesAST(std::string name, symbol::SymbolReference* sr){
    this->name = name;
    this->var = sr;
}

AST* VarAccesAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() == 0) return nullptr;
    std::string name = "";
    lexer::Token::TokenType last = lexer::Token::TokenType::SUBNS;
    if (tokens[0].type == lexer::Token::TokenType::SUBNS){
        parser::error("Expected Symbol", tokens[0], "module name or variable name expected", 30);
        return new AST;
    }
    for (lexer::Token t : tokens){
        if (last == lexer::Token::TokenType::SUBNS && t.type == lexer::Token::TokenType::ID){
            name += t.value;
        }
        else if (last == lexer::Token::TokenType::ID && t.type == lexer::Token::TokenType::SUBNS){
            name += "::";
        }
        else return nullptr;
        last = t.type;
    }
    std::string type = sr->find(name);
    if (type == ""){
        parser::error("Unknown variable", tokens[0], tokens[tokens.size()-1], "A variable of this name was not found in this scope", 20);
        return new AST;
    }
    return new VarAccesAST(name, sr->find_symbol(name));
}

void VarAccesAST::force_type(std::string type){
    if (var->find("") != type){
        parser::error("Type mismatch", tokens[0], tokens[tokens.size()-1] ,std::string("expected a \e[1m") + type + "\e[0m, got a variable of type " + var->find(""), 17, "Caused by");
    }
}

