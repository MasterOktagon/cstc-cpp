#pragma once
#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"

namespace math {
    extern AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");

}

class AddAST : public AST {
    AST* left;
    AST* right;

    public:
    AddAST(AST* left, AST* right);
    virtual ~AddAST();
    std::string get_type(){
        return max_prec_type(left->get_type(), right->get_type());
    }
    std::string get_ll_type();

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
    // ALSO PARSES SUBAST
};

class SubAST : public AST {
    AST* left;
    AST* right;

    public:
    SubAST(AST* left, AST* right);
    virtual ~SubAST();
    std::string get_type(){
        return max_prec_type(left->get_type(), right->get_type());
    }
    std::string get_ll_type();

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

class MulAST : public AST {
    AST* left;
    AST* right;

    public:
    MulAST(AST* left, AST* right);
    virtual ~MulAST();
    std::string get_type(){
        return max_prec_type(left->get_type(), right->get_type());
    }
    std::string get_ll_type();

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

class DivAST : public AST {
    AST* left;
    AST* right;

    public:
    DivAST(AST* left, AST* right);
    virtual ~DivAST();
    std::string get_type(){
        return max_prec_type(left->get_type(), right->get_type());
    }
    std::string get_ll_type();

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

class ModAST : public AST {
    AST* left;
    AST* right;

    public:
    ModAST(AST* left, AST* right);
    virtual ~ModAST();
    std::string get_type(){
        return max_prec_type(left->get_type(), right->get_type());
    }
    std::string get_ll_type();

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

class AddrOfAST : public AST {
    AST* of;

    public:
    AddrOfAST(AST* of);
    virtual ~AddrOfAST();
    std::string get_type(){
        return of->get_type()+"*";
    }
    std::string get_ll_type();

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

