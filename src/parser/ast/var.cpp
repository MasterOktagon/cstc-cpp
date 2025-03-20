#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include "../parser.hpp"
#include <string>
#include "var.hpp"
#include "base_math.hpp"
#include "literal.hpp"
#include "type.hpp"
#include <iostream>

std::string parse_name(std::vector<lexer::Token> tokens){
    if (tokens.size() == 0) return "";
    std::string name = "";
    lexer::Token::TokenType last = lexer::Token::TokenType::SUBNS;
    //std::cout << tokens.size() << std::endl;
    
    if (tokens[0].type == lexer::Token::TokenType::SUBNS){
        parser::error("Expected Symbol", tokens[0], "module name or variable name expected", 30);
        return "";
    }
    for (lexer::Token t : tokens){
        if (last == lexer::Token::TokenType::SUBNS && t.type == lexer::Token::TokenType::ID){
            name += t.value;
        }
        else if (last == lexer::Token::TokenType::ID && t.type == lexer::Token::TokenType::SUBNS){
            name += "::";
        }
        else return "null";
        last = t.type;
    }
    if (last == lexer::Token::TokenType::SUBNS){
        parser::error("Expected Symbol", tokens[tokens.size()-1], "module name or variable name expected", 30);
        return "";
    }
    return name; 
}

AST* parseStatement(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() == 0 || tokens[tokens.size()-1].type != lexer::Token::TokenType::END_CMD) return nullptr;
    return math::parse(parser::subvector(tokens, 0,1,tokens.size()-1), local, sr, expected_type);
}

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

std::string VarDeclAST::emit_ll(int*, std::string){
    return std::string("%") + name + " = alloca " + type->get_ll_type() + "\n";
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
        if (tokens[split-1].type == lexer::Token::TokenType::ID && split > 1){
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

std::string VarInitlAST::emit_ll(int* locc, std::string){
    std::string s = std::string("%") + name + " = alloca " + type->get_ll_type() + ", align 8\n" +
        "store " + expression->get_ll_type() + " {}, " + type->get_ll_type() + "* " + std::string("%") + name + ", align 8\n" ;
    std::string l = expression->emit_ll(locc, s);
    return l;
}





VarAccesAST::VarAccesAST(std::string name, symbol::SymbolReference* sr){
    this->name = name;
    this->var = sr;
}

AST* VarAccesAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() == 0) return nullptr;
    std::string name = parse_name(tokens);
    if (name == "") return new AST;
    if (name == "null") return nullptr;
    std::string type = sr->find(name);
    if (type == ""){
        parser::error("Unknown variable", tokens[0], tokens[tokens.size()-1], "A variable of this name was not found in this scope", 20);
        return new AST;
    }
    symbol::SymbolReference* p = sr->find_symbol(name);
    if (p == dynamic_cast<symbol::Var*>(p)) ((symbol::Var*) p)->used = true;
    return new VarAccesAST(name, p);
}

void VarAccesAST::force_type(std::string type){
    if (var->find("") != type){
        parser::error("Type mismatch", tokens[0], tokens[tokens.size()-1] ,std::string("expected a \e[1m") + type + "\e[0m, got a variable of type " + var->find(""), 17, "Caused by");
    }
}

std::string VarAccesAST::emit_ll(int* locc, std::string inp){
    std::string s = std::string("%") + std::to_string(++(*locc)) + " = load " + parser::ll_type(var->find("")) + ", " + parser::ll_type(var->find("")) + "* %" + name + ", align 8\n";
    inp = rinsert("%" + std::to_string(*locc), inp);
    return s + inp;
}






VarSetAST::VarSetAST(std::string name, symbol::SymbolReference* sr, AST* expr){
    this->name = name;
    this->var  = sr;
    this->expr = expr;
}

AST* VarSetAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() == 0) return nullptr;
    std::string name = "";
    lexer::Token::TokenType last = lexer::Token::TokenType::SUBNS;
    int split = parser::rsplitStack(tokens, {lexer::Token::TokenType::SET}, local);
    if (split == 0){
        parser::error("Expected Expression", tokens[tokens.size()-1], "Expected an expression after '='", 31);
        return new AST;
    }
    if (split == tokens.size()) return nullptr;
    auto varname = parser::subvector(tokens, 0,1,split);
    if (varname[0].type == lexer::Token::TokenType::SUBNS){
        parser::error("Expected Symbol", tokens[0], "module name or variable name expected", 30);
        return new AST;
    }
    for (lexer::Token t : varname){
        if (last == lexer::Token::TokenType::SUBNS && t.type == lexer::Token::TokenType::ID){
            name += t.value;
        }
        else if (last == lexer::Token::TokenType::ID && t.type == lexer::Token::TokenType::SUBNS){
            name += "::";
        }
        else return nullptr;
        last = t.type;
    }
    if (last == lexer::Token::TokenType::SUBNS) parser::error("Expected Symbol", varname[varname.size()-1], "module name or variable name expected", 30);
    
    AST* expr = math::parse(parser::subvector(tokens, split+1,1,tokens.size()), local, sr);
    if (expr == nullptr){
        parser::error("Expected Expression", tokens[tokens.size()-1], "Expected a valid expression after '='", 31);
        return new AST;
    }
    
    std::string type = sr->find(name);
    if (type == ""){
        parser::error("Unknown variable", tokens[0], tokens[tokens.size()-1], "A variable of this name was not found in this scope", 20);
        return new AST;
    }
    expr->force_type(type);
    symbol::SymbolReference* p = sr->find_symbol(name);
    return new VarSetAST(name, p, expr);
}

void VarSetAST::force_type(std::string type){
    if (var->find("") != type){
        if (var == dynamic_cast<symbol::Var*>(var)) ((symbol::Var*) var)->used = true;
        parser::error("Type mismatch", tokens[0], tokens[tokens.size()-1] ,std::string("expected a \e[1m") + type + "\e[0m, got a variable of type " + var->find(""), 17, "Caused by");
    }
}

std::string VarSetAST::emit_ll(int* locc, std::string inp){
    std::string s = std::string("store ") + parser::ll_type(var->find("")) + " {}, " + parser::ll_type(var->find("")) + "* %" + name + "\n";
    std::string l = expr->emit_ll(locc, s);
    if (expr != dynamic_cast<LiteralAST*>(expr)) inp = rinsert(std::string("%") + std::to_string(*locc), inp);
    else inp = expr->emit_ll(locc, inp);

    return l + inp;
}
