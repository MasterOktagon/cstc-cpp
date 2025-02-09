#include <string>
#include "base_math.hpp"
#include "literal.hpp"

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

