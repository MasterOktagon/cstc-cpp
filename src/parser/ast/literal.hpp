#pragma once
#include <string>
#include "../symboltable.hpp"
#include "../../lexer/lexer.hpp"

#include "ast.hpp"

class LiteralAST : public AST {
    public:
    virtual ~LiteralAST(){}
    virtual std::string get_value(){return "";}
};

class IntLiteralAST : public LiteralAST {

    std::string value = "0"; // Integer value
    int bits = 32;           // Integer Bit size

    public:

    IntLiteralAST(int bits, std::string value);
    virtual ~IntLiteralAST(){}
    std::string get_type(){return "@int";}
    std::string get_ll_type(){return std::string("i") + std::to_string(bits);}
    std::string get_value(){return value;}

    std::string emit_ll(int locc=0);
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */

    std::string emit_cst();
    /*
        Emit C* code
    */
    static IntLiteralAST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};

class BoolLiteralAST : public LiteralAST {

    bool value = false; // boolean value

    public:

    BoolLiteralAST(bool value);
    virtual ~BoolLiteralAST(){}
    std::string get_type(){return "bool";}
    std::string get_ll_type(){return "i1";}
    std::string get_value(){return std::to_string(value);}

    std::string emit_ll(int locc=0);
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */

    std::string emit_cst();
    /*
        Emit C* code
    */
};

class FloatLiteralAST : public LiteralAST {

    std::string value = "0";    // Float value
    int bits = 32; // Float size (name)

    public:

    FloatLiteralAST(int bits, std::string value);
    virtual ~FloatLiteralAST(){}
    std::string get_type(){return "@float";}
    std::string get_ll_type();
    std::string get_value(){return value;}

    std::string emit_ll(int locc=0);
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */

    std::string emit_cst();
    /*
        Emit C* code
    */
};
