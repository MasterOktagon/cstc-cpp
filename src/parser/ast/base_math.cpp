#include <string>
#include "base_math.hpp"
#include "ast.hpp"
#include "literal.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include <iostream>


// AddAST

AddAST::AddAST(AST* left, AST* right){
    this->left = left;
    this->right = right;
}

AddAST::~AddAST(){
    delete left;
    delete right;
}
std::string AddAST::get_ll_type(){
    return left->get_ll_type();
}

std::string AddAST::emit_ll(int locc){
    std::string op = left->get_ll_type()[0] == 'i'? "add" : "fadd";
    std::string out = std::string("%var.") + std::to_string(locc) + " = " + op + " " + left->get_ll_type() + " ";
    std::string prec = "";
    
    if (left == dynamic_cast<LiteralAST*>(left)){
        out += ((LiteralAST*) left)->get_value() + ", ";
    } else {
        prec += left->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + ", ";
    }
    if (right == dynamic_cast<LiteralAST*>(right)){
        out += ((LiteralAST*) right)->get_value() + "\n";
    } else {
        prec += right->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + "\n";
    }

    return prec + out;
}

std::string AddAST::emit_cst(){
    return std::string("(") + left->emit_cst() + " + " + right->emit_cst() + ")";
}

AST* AddAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 1) return nullptr;
    lexer::Token first; bool first_is_sub = false;
    if (tokens[0].type == lexer::Token::TokenType::SUB){
        first = tokens[0]; first_is_sub = true;
        tokens = parser::subvector(tokens, 1,1,tokens.size());
    }
    int split = parser::rsplitStack(tokens, {lexer::Token::TokenType::ADD, lexer::Token::TokenType::SUB}, local);
    if (first_is_sub) tokens.insert(tokens.begin(), first);
    if (tokens.size() > 2 && split != 0 && split != tokens.size()){
        #ifdef DEBUG
            std::cout << "AddAST::parse:\tfirst_is_sub:\t" << first_is_sub << std::endl;
        #endif
        lexer::Token op = tokens[split+first_is_sub];
        AST* left = IntLiteralAST::parse(parser::subvector(tokens, 0,1,split + first_is_sub), local, sr, expected_type);
        if (left == nullptr){
            parser::error("Expression expected", tokens[0], tokens[split+first_is_sub-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            return new AST();
        }
        AST* right = IntLiteralAST::parse(parser::subvector(tokens, split + first_is_sub+1,1,tokens.size()), local, sr, expected_type);
        if (right == nullptr){
            parser::error("Expression expected", tokens[split+first_is_sub], tokens[tokens.size()-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            delete left;
            return new AST();
        }
        return new AddAST(left, right);
    }

    return nullptr;
}

// SubAST

SubAST::SubAST(AST* left, AST* right){
    this->left = left;
    this->right = right;
}

SubAST::~SubAST(){
    delete left;
    delete right;
}
std::string SubAST::get_ll_type(){
    return left->get_ll_type();
}

std::string SubAST::emit_ll(int locc){
    std::string op = left->get_ll_type()[0] == 'i'? "sub" : "fsub";
    std::string out = std::string("%var.") + std::to_string(locc) + " = " + op + " " + left->get_ll_type() + " ";
    std::string prec = "";
    
    if (left == dynamic_cast<LiteralAST*>(left)){
        out += ((LiteralAST*) left)->get_value() + ", ";
    } else {
        prec += left->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + ", ";
    }
    if (right == dynamic_cast<LiteralAST*>(right)){
        out += ((LiteralAST*) right)->get_value() + "\n";
    } else {
        prec += right->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + "\n";
    }

    return prec + out;
}

std::string SubAST::emit_cst(){
    return std::string("(") + left->emit_cst() + " - " + right->emit_cst() + ")";
}

// MulAST

MulAST::MulAST(AST* left, AST* right){
    this->left = left;
    this->right = right;
}

MulAST::~MulAST(){
    delete left;
    delete right;
}
std::string MulAST::get_ll_type(){
    return left->get_ll_type();
}

std::string MulAST::emit_ll(int locc){
    std::string op = left->get_ll_type()[0] == 'i'? "mul" : "fmul";
    std::string out = std::string("%var.") + std::to_string(locc) + " = " + op + " " + left->get_ll_type() + " ";
    std::string prec = "";
    
    if (left == dynamic_cast<LiteralAST*>(left)){
        out += ((LiteralAST*) left)->get_value() + ", ";
    } else {
        prec += left->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + ", ";
    }
    if (right == dynamic_cast<LiteralAST*>(right)){
        out += ((LiteralAST*) right)->get_value() + "\n";
    } else {
        prec += right->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + "\n";
    }

    return prec + out;
}

std::string MulAST::emit_cst(){
    return std::string("(") + left->emit_cst() + " * " + right->emit_cst() + ")";
}

// DivAST

DivAST::DivAST(AST* left, AST* right){
    this->left = left;
    this->right = right;
}

DivAST::~DivAST(){
    delete left;
    delete right;
}
std::string DivAST::get_ll_type(){
    return left->get_ll_type();
}

std::string DivAST::emit_ll(int locc){
    std::string op = left->get_ll_type()[0] == 'i'? left->get_type()[0] == 'u' ? "udiv" : "sdiv" : "fdiv";
    std::string out = std::string("%var.") + std::to_string(locc) + " = " + op + " " + left->get_ll_type() + " ";
    std::string prec = "";
    
    if (left == dynamic_cast<LiteralAST*>(left)){
        out += ((LiteralAST*) left)->get_value() + ", ";
    } else {
        prec += left->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + ", ";
    }
    if (right == dynamic_cast<LiteralAST*>(right)){
        out += ((LiteralAST*) right)->get_value() + "\n";
    } else {
        prec += right->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + "\n";
    }

    return prec + out;
}

std::string DivAST::emit_cst(){
    return std::string("(") + left->emit_cst() + " / " + right->emit_cst() + ")";
}

// ModAST

ModAST::ModAST(AST* left, AST* right){
    this->left = left;
    this->right = right;
}

ModAST::~ModAST(){
    delete left;
    delete right;
}
std::string ModAST::get_ll_type(){
    return left->get_ll_type();
}

std::string ModAST::emit_ll(int locc){
    std::string op = left->get_ll_type()[0] == 'i'? left->get_type()[0] == 'u' ? "urem" : "srem" : "frem";
    std::string out = std::string("%var.") + std::to_string(locc) + " = " + op + " " + left->get_ll_type() + " ";
    std::string prec = "";
    
    if (left == dynamic_cast<LiteralAST*>(left)){
        out += ((LiteralAST*) left)->get_value() + ", ";
    } else {
        prec += left->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + ", ";
    }
    if (right == dynamic_cast<LiteralAST*>(right)){
        out += ((LiteralAST*) right)->get_value() + "\n";
    } else {
        prec += right->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + "\n";
    }

    return prec + out;
}

std::string ModAST::emit_cst(){
    return std::string("(") + left->emit_cst() + " % " + right->emit_cst() + ")";
}

// AddrOfAST

AddrOfAST::AddrOfAST(AST* of){
    this->of = of;
}

AddrOfAST::~AddrOfAST(){
    delete of;
}
std::string AddrOfAST::get_ll_type(){
    return "ptr";
}

std::string AddrOfAST::emit_cst(){
    return std::string("#") + of->emit_cst();
}
std::string AddrOfAST::emit_ll(int locc){
    std::string op = "store";
    std::string out = std::string("%var.") + std::to_string(locc) + " = " + op + " " + of->get_ll_type() + " ";
    std::string prec = "";

    if (of == dynamic_cast<LiteralAST*>(of)){
        out += ((LiteralAST*) of)->get_value() + ", ptr poison\n";
    } else {
        prec += of->emit_ll(++locc);
        out  += std::string("%var.") + std::to_string(locc) + ", ptr poison\n";
    }

    return prec + out;
}

