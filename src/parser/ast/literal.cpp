
#include "literal.hpp"
#include "../symboltable.hpp"
#include "../../lexer/lexer.hpp"
#include <string>

IntLiteralAST::IntLiteralAST(int bits, std::string value){
    this->bits  = bits;
    this->value = value;
}

std::string IntLiteralAST::emit_ll(int locc){
    return std::string("%") + std::to_string(locc) + " = i" + std::to_string(bits) + " " + value + "\n";
}

std::string IntLiteralAST::emit_cst(){
    return value;
}

IntLiteralAST* IntLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() != 1) return nullptr;
    

    return nullptr;
}



BoolLiteralAST::BoolLiteralAST(bool value){
    this->value = value;
}

std::string BoolLiteralAST::emit_ll(int locc){
    return std::string("%") + std::to_string(locc) + " = i1 " + std::to_string(value) + "\n";
}

std::string BoolLiteralAST::emit_cst(){
    return value? "true" : "false";
}

FloatLiteralAST::FloatLiteralAST(int bits, std::string value){
    this->bits  = bits;
    this->value = value;
}

std::string FloatLiteralAST::emit_ll(int locc){
    return std::string("%") + std::to_string(locc) + " = " + get_ll_type() + " " + value + "\n";
}

std::string FloatLiteralAST::emit_cst(){
    return value;
}

std::string FloatLiteralAST::get_ll_type(){
    if (bits < 32)       return "half";
    else if (bits < 64)  return "float";
    else if (bits < 128) return "double";
    else                 return "ppc_fp128";
}