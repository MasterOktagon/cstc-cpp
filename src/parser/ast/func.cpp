

#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include "../parser.hpp"
#include <string>
#include "func.hpp"
#include "base_math.hpp"
#include "literal.hpp"
#include "type.hpp"
#include "var.hpp"
#include <iostream>

#define DEBUG

AST* FuncCallAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 3) return nullptr;

    std::vector<AST*> params = {};
    int split = parser::rsplitStack(tokens, {lexer::Token::TokenType::PT_OPEN}, local);

    #ifdef DEBUG
        std::cout << "FuncCallAST::parse: split:\t" << split << std::endl;
    #endif

    if (tokens[tokens.size()-1].type != lexer::Token::TokenType::PT_CLOSE) return nullptr;
    if (split == 0) return nullptr;
    if ((uint) split >= tokens.size()-1) return nullptr;
    std::string name = parse_name(parser::subvector(tokens, 0,1,split));
    if (name == "") return new AST;
    auto tokens2 = parser::subvector(tokens, split+1,0,tokens.size()-1);
    while (tokens2.size() > 0 && (uint) split < tokens2.size()){
        split = parser::rsplitStack(tokens2, {lexer::Token::TokenType::COMMA},local+1);
        AST* a = math::parse(parser::subvector(tokens2, 0,1,split), local, sr);
        if (a == nullptr){
            parser::error("Expected Expression", tokens2[0], tokens2[split], "Expected a valid expression", 31);
            return new AST;
        }
        params.push_back(a);
        tokens2 = parser::subvector(tokens2, split,1,tokens2.size());
    }
    if (tokens2.size() > 0){
        split = tokens2.size();
        AST* a = math::parse(parser::subvector(tokens2, 0,1,split), local, sr);
        if (a == nullptr){
            parser::error("Expected Expression", tokens2[0], tokens2[split], "Expected a valid expression", 31);
            return new AST;
        }
        params.push_back(a);
    }

    symbol::Func* p = sr->find_fn(name)[0];
    return new FuncCallAST(name, params, p);
}

std::string FuncCallAST::emit_ll(int* locc, std::string inp){
    std::string s = "invoke " + parser::ll_type(fn->type) + " @" + name + "(";
    if (parser::ll_type(fn->type) != "void") s = "{} = " + s;
    for (AST* p : params){
        s += " ";
        s += parser::ll_type(p->get_type()) + " {}, ";
        s = p->emit_ll(locc, s);
    }
    if (s[s.size()-1] == ' ') s = s.substr(0,s.size()-2);
    s += " ) to label %exc\n";

    if (parser::ll_type(fn->type) != "void"){
        s = insert(std::string("%") + std::to_string(++(*locc)), s);
        inp = rinsert(std::string("%") + std::to_string(*locc), inp);
    }
    return s + inp;
}

std::string FuncCallAST::emit_cst(){
    std::string s = name + "(";
    for (AST* p : params){
        s += p->emit_cst();
    }
    s += ")";
    return s;
}
