#pragma once

#include <string>
#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"


class SubBlockAST : public AST {
    public:

    std::vector<AST*> contents = {};

    SubBlockAST(){}
    virtual bool is_const(){return false;} // do constant folding or not
    virtual ~SubBlockAST(){}
    virtual std::string emit_ll(int locc=0){return "";}
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */
    //virtual llvm::Value* codegen(){return nullptr;}
    /*
        Emit llvm-bitcode to be compiled later
    */

    virtual std::string emit_cst();
    /*
        Emit C* code
    */
    
    virtual std::string get_type(){return "void";}
    virtual std::string get_ll_type(){return "";}
    virtual void force_type(std::string type){}
    /*
        Try to enforce a specific type
    */

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};


