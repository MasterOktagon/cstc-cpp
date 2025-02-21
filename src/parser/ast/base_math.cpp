#include <string>
#include "base_math.hpp"
#include "ast.hpp"
#include "literal.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include <iostream>
#include <regex>
#include "../parser.hpp"

#define DEBUG
// AddAST

AddAST::AddAST(AST* left, AST* right, std::vector<lexer::Token> tokens){
    this->left = left;
    this->right = right;
    this->tokens = tokens;
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
    auto t = tokens;
    if (tokens.size() < 1) return nullptr;
    lexer::Token first; bool first_is_sub = false;
    if (tokens[0].type == lexer::Token::TokenType::SUB){
        first = tokens[0]; first_is_sub = true;
        tokens = parser::subvector(tokens, 1,1,tokens.size());
    }
    size_t split = parser::splitStack(tokens, {lexer::Token::TokenType::ADD, lexer::Token::TokenType::SUB}, local);
    if (first_is_sub) tokens.insert(tokens.begin(), first);
    if (tokens.size() > 2 && split != 0 && split < tokens.size()-1){
        #ifdef DEBUG
            std::cout << "AddAST::parse:\tfirst_is_sub:\t" << first_is_sub << std::endl;
            std::cout << "AddAST::parse:\tvalue size:\t" << tokens.size() << std::endl;
            std::cout << "AddAST::parse:\tsplit:\t" << split << std::endl;
        #endif
        lexer::Token op = tokens[split+first_is_sub];
        AST* left = math::parse(parser::subvector(tokens, 0,1,split + first_is_sub), local, sr, expected_type);
        if (left == nullptr){
            /*if(op.type == lexer::Token::TokenType::SUB){
                AST* right = math::parse(parser::subvector(tokens, split + first_is_sub+1,1,tokens.size()), local, sr, expected_type);
                if (right == dynamic_cast<IntLiteralAST*>(right)){
                    if (((IntLiteralAST*) right)->get_value()[0] != '-'){
                        return nullptr;
                    }
                }
            }*/
            parser::error("Expression expected", tokens[0], tokens[split+first_is_sub-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            return new AST();
        }
        AST* right = math::parse(parser::subvector(tokens, split + first_is_sub+1,1,tokens.size()), local, sr, expected_type);
        if (right == nullptr){
            parser::error("Expression expected", tokens[split+first_is_sub], tokens[tokens.size()-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            delete left;
            return new AST();
        }
        if (op.type == lexer::Token::TokenType::ADD) return new AddAST(left, right, t);
        else if (op.type == lexer::Token::TokenType::SUB) return new SubAST(left, right, t);
    }

    return nullptr;
}

void AddAST::force_type(std::string type){
    std::regex i("u?int(8|16|32|64|128)");
    std::regex f("float(16|32|64|128)");
    bool int_required = std::regex_match(type, i);
    bool float_required = std::regex_match(type, f);
    if (std::regex_match(left->get_type(), i) && int_required){
        left->force_type(type);
        right->force_type(type);
    }
    if (std::regex_match(left->get_type(), i) && float_required){
        left->force_type(type);
        right->force_type(type);
    }

    std::string ret = parser::hasOp(left->get_type(), right->get_type(), lexer::Token::TokenType::ADD);
    if (ret != ""){
        if (ret != type) parser::error("Mismatiching types", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator + (" + right->get_type() + ") yields " + ret + " (expected \e[1m" + type + "\e[0m)", 18);
    }
    else parser::error("Unknown operator", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator + (" + right->get_type() + ") is not implemented.", 18);

}

// SubAST

SubAST::SubAST(AST* left, AST* right, std::vector<lexer::Token> tokens){
    this->left = left;
    this->right = right;
    this->tokens = tokens;
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

void SubAST::force_type(std::string type){
    std::regex i("u?int(8|16|32|64|128)");
    std::regex f("float(16|32|64|128)");
    bool int_required = std::regex_match(type, i);
    bool float_required = std::regex_match(type, f);
    if (std::regex_match(left->get_type(), i) && int_required){
        left->force_type(type);
        right->force_type(type);
    }
    if (std::regex_match(left->get_type(), i) && float_required){
        left->force_type(type);
        right->force_type(type);
    }

    std::string ret = parser::hasOp(left->get_type(), right->get_type(), lexer::Token::TokenType::SUB);
    if (ret != ""){
        if (ret != type) parser::error("Mismatiching types", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator - (" + right->get_type() + ") yields " + ret + " (expected \e[1m" + type + "\e[0m)", 18);
    }
    else parser::error("Unknown operator", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator - (" + right->get_type() + ") is not implemented.", 18);

}

// MulAST

MulAST::MulAST(AST* left, AST* right, std::vector<lexer::Token> tokens){
    this->left = left;
    this->right = right;
    this->tokens = tokens;
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

AST* MulAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 1) return nullptr;
    auto t = tokens;
    size_t split = parser::splitStack(tokens, {lexer::Token::TokenType::MUL, lexer::Token::TokenType::DIV, lexer::Token::TokenType::MOD}, local);
    if (tokens.size() > 2 && split != 0 && split < tokens.size()){
        #ifdef DEBUG
            std::cout << "MulAST::parse:\tsplit:\t" << split << std::endl;
        #endif
        lexer::Token op = tokens[split];
        AST* left = math::parse(parser::subvector(tokens, 0,1,split), local, sr, expected_type);
        if (left == nullptr){
            parser::error("Expression expected", tokens[0], tokens[split-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            return new AST();
        }
        AST* right = math::parse(parser::subvector(tokens, split+1,1,tokens.size()), local, sr, expected_type);
        if (right == nullptr){
            parser::error("Expression expected", tokens[split], tokens[tokens.size()-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            delete left;
            return new AST();
        }
        if (op.type == lexer::Token::TokenType::MUL) return new MulAST(left, right, t);
        else if (op.type == lexer::Token::TokenType::DIV) return new DivAST(left, right, t);
        else if (op.type == lexer::Token::TokenType::MOD) return new ModAST(left, right, t);
    }

    return nullptr;
}

void MulAST::force_type(std::string type){
    std::regex i("u?int(8|16|32|64|128)");
    std::regex f("float(16|32|64|128)");
    bool int_required = std::regex_match(type, i);
    bool float_required = std::regex_match(type, f);
    if (std::regex_match(left->get_type(), i) && int_required){
        left->force_type(type);
        right->force_type(type);
    }
    if (std::regex_match(left->get_type(), i) && float_required){
        left->force_type(type);
        right->force_type(type);
    }

    std::string ret = parser::hasOp(left->get_type(), right->get_type(), lexer::Token::TokenType::MUL);
    if (ret != ""){
        if (ret != type) parser::error("Mismatiching types", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator * (" + right->get_type() + ") yields " + ret + " (expected \e[1m" + type + "\e[0m)", 18);
    }
    else parser::error("Unknown operator", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator * (" + right->get_type() + ") is not implemented.", 18);

}


// DivAST

DivAST::DivAST(AST* left, AST* right, std::vector<lexer::Token> tokens){
    this->left = left;
    this->right = right;
    this->tokens = tokens;
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

void DivAST::force_type(std::string type){
    std::regex i("u?int(8|16|32|64|128)");
    std::regex f("float(16|32|64|128)");
    bool int_required = std::regex_match(type, i);
    bool float_required = std::regex_match(type, f);
    if (std::regex_match(left->get_type(), i) && int_required){
        left->force_type(type);
        right->force_type(type);
    }
    if (std::regex_match(left->get_type(), i) && float_required){
        left->force_type(type);
        right->force_type(type);
    }

    std::string ret = parser::hasOp(left->get_type(), right->get_type(), lexer::Token::TokenType::DIV);
    if (ret != ""){
        if (ret != type) parser::error("Mismatiching types", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator / (" + right->get_type() + ") yields " + ret + " (expected \e[1m" + type + "\e[0m)", 18);
    }
    else parser::error("Unknown operator", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator / (" + right->get_type() + ") is not implemented.", 18);

}

// ModAST

ModAST::ModAST(AST* left, AST* right, std::vector<lexer::Token> tokens){
    this->left = left;
    this->right = right;
    this->tokens = tokens;
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

void ModAST::force_type(std::string type){
    std::regex i("u?int(8|16|32|64|128)");
    std::regex f("float(16|32|64|128)");
    bool int_required = std::regex_match(type, i);
    bool float_required = std::regex_match(type, f);
    if (std::regex_match(left->get_type(), i) && int_required){
        left->force_type(type);
        right->force_type(type);
    }
    if (std::regex_match(left->get_type(), i) && float_required){
        left->force_type(type);
        right->force_type(type);
    }

    std::string ret = parser::hasOp(left->get_type(), right->get_type(), lexer::Token::TokenType::MOD);
    if (ret != ""){
        if (ret != type) parser::error("Mismatiching types", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator % (" + right->get_type() + ") yields " + ret + " (expected \e[1m" + type + "\e[0m)", 18);
    }
    else parser::error("Unknown operator", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator % (" + right->get_type() + ") is not implemented.", 18);

}

// LorAST

LorAST::LorAST(AST* left, AST* right, std::vector<lexer::Token> tokens){
    this->left = left;
    this->right = right;
    this->tokens = tokens;
}

LorAST::~LorAST(){
    delete left;
    delete right;
}
std::string LorAST::get_ll_type(){
    return left->get_ll_type();
}

std::string LorAST::emit_ll(int locc){
    std::string op = "or";
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

std::string LorAST::emit_cst(){
    return std::string("(") + left->emit_cst() + " || " + right->emit_cst() + ")";
}

AST* LorAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 1) return nullptr;
    auto t = tokens;
    size_t split = parser::splitStack(tokens, {lexer::Token::TokenType::LOR}, local);
    if (tokens.size() > 2 && split != 0 && split < tokens.size()){
        #ifdef DEBUG
            std::cout << "LorAST::parse:\tsplit:\t" << split << std::endl;
        #endif
        lexer::Token op = tokens[split];
        AST* left = math::parse(parser::subvector(tokens, 0,1,split), local, sr, expected_type);
        if (left == nullptr){
            parser::error("Expression expected", tokens[0], tokens[split-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            return new AST();
        }
        AST* right = math::parse(parser::subvector(tokens, split+1,1,tokens.size()), local, sr, expected_type);
        if (right == nullptr){
            parser::error("Expression expected", tokens[split], tokens[tokens.size()-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            delete left;
            return new AST();
        }
        if (op.type == lexer::Token::TokenType::LOR) return new LorAST(left, right, t);
    }

    return nullptr;
}

void LorAST::force_type(std::string type){

    std::string ret = parser::hasOp(left->get_type(), right->get_type(), lexer::Token::TokenType::LOR);
    if (ret != ""){
        if (ret != type) parser::error("Mismatiching types", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator || (" + right->get_type() + ") yields " + ret + " (expected \e[1m" + type + "\e[0m)", 18);
    }
    else parser::error("Unknown operator", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator || (" + right->get_type() + ") is not implemented.", 18);

}

// LandAST

LandAST::LandAST(AST* left, AST* right, std::vector<lexer::Token> tokens){
    this->left = left;
    this->right = right;
    this->tokens = tokens;
}

LandAST::~LandAST(){
    delete left;
    delete right;
}
std::string LandAST::get_ll_type(){
    return left->get_ll_type();
}

std::string LandAST::emit_ll(int locc){
    std::string op = "and";
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

std::string LandAST::emit_cst(){
    return std::string("(") + left->emit_cst() + " && " + right->emit_cst() + ")";
}

AST* LandAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 1) return nullptr;
    auto t = tokens;
    size_t split = parser::splitStack(tokens, {lexer::Token::TokenType::LAND}, local);
    if (tokens.size() > 2 && split != 0 && split < tokens.size()){
        #ifdef DEBUG
            std::cout << "LandAST::parse:\tsplit:\t" << split << std::endl;
        #endif
        lexer::Token op = tokens[split];
        AST* left = math::parse(parser::subvector(tokens, 0,1,split), local, sr, expected_type);
        if (left == nullptr){
            parser::error("Expression expected", tokens[0], tokens[split-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            return new AST();
        }
        AST* right = math::parse(parser::subvector(tokens, split+1,1,tokens.size()), local, sr, expected_type);
        if (right == nullptr){
            parser::error("Expression expected", tokens[split], tokens[tokens.size()-1], std::string("Expected espression of type \e[1m") + expected_type + "\e[0m", 111);
            delete left;
            return new AST();
        }
        if (op.type == lexer::Token::TokenType::LAND) return new LandAST(left, right, t);
    }

    return nullptr;
}

void LandAST::force_type(std::string type){

    std::string ret = parser::hasOp(left->get_type(), right->get_type(), lexer::Token::TokenType::LAND);
    if (ret != ""){
        if (ret != type) parser::error("Mismatiching types", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator && (" + right->get_type() + ") yields " + ret + " (expected \e[1m" + type + "\e[0m)", 18);
    }
    else parser::error("Unknown operator", tokens[0], tokens[tokens.size()-1], left->get_type() + "::operator && (" + right->get_type() + ") is not implemented.", 18);

}

// AddrOfAST

AddrOfAST::AddrOfAST(AST* of){
    this->of = of;
}

AddrOfAST::~AddrOfAST(){
    delete of;
}
std::string AddrOfAST::get_ll_type(){
    return of->get_ll_type() + "*";
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

AST* math::parse_pt(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 2) return nullptr;
    if (tokens[0].type == lexer::Token::TokenType::PT_OPEN){
        if (tokens[tokens.size()-1].type == lexer::Token::TokenType::PT_CLOSE){
            #ifdef DEBUG
                std::cout << "parse_pt" << std::endl;
            #endif
            return math::parse(parser::subvector(tokens, 1,1,tokens.size()-1), local+1, sr, expected_type);
        }
    }

    return nullptr;
}


AST* math::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    return parser::parseOneOf(tokens, {
        IntLiteralAST::parse,
        FloatLiteralAST::parse,
        BoolLiteralAST::parse,
        CharLiteralAST::parse,

        AddAST::parse,
        MulAST::parse,
        LandAST::parse,
        LorAST::parse,
        
        parse_pt}, local, sr, expected_type);
}


