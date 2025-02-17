#pragma once
#include <string>
#include "../symboltable.hpp"
#include "../../lexer/lexer.hpp"
#include "../parser.hpp"

#include "ast.hpp"

class LiteralAST : public AST {
    public:
    virtual ~LiteralAST(){}
    virtual std::string get_value(){return "";}
};

class IntLiteralAST : public LiteralAST {

    std::string value = "0"; // Integer value
    int bits = 32;           // Integer Bit size
    bool tsigned = true;

    public:

    IntLiteralAST(int bits, std::string value, bool tsigned=true);
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
    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
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

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
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

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};

class CharLiteralAST : public LiteralAST {

    std::string value = "a";    // Float value

    public:

    CharLiteralAST(std::string value);
    virtual ~CharLiteralAST(){}
    std::string get_type(){return "char";}
    std::string get_ll_type(){return "i16";};
    std::string get_value();

    std::string emit_ll(int locc=0){return get_value();}
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */

    std::string emit_cst(){return std::string("\"") + value + "\"";};
    /*
        Emit C* code
    */

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};
