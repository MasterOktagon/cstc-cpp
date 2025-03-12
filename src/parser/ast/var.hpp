#pragma once
#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include "../parser.hpp"
#include <string>

class VarDeclAST : public AST {

    std::string name = "";
    AST* type = nullptr;

    public:
    VarDeclAST(std::string name, AST* type);
    virtual bool is_const(){return false;} // do constant folding or not
    virtual ~VarDeclAST(){}
    virtual std::string emit_ll(int locc=0){return "";}
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */
    //virtual llvm::Value* codegen(){return nullptr;}
    /*
        Emit llvm-bitcode to be compiled later
    */

    virtual std::string emit_cst(){return type->get_type() + " " + name + ";";}
    /*
        Emit C* code
    */
    
    virtual std::string get_type(){return name;}
    virtual std::string get_ll_type(){return "";}
    virtual void force_type(std::string type){}
    /*
        Try to enforce a specific type
    */

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};

class VarInitlAST : public AST {

    std::string name = "";
    AST* type = nullptr;
    AST* expression = nullptr;

    public:
    VarInitlAST(std::string name, AST* type, AST* expr);
    virtual bool is_const(){return false;} // do constant folding or not
    virtual ~VarInitlAST(){}
    virtual std::string emit_ll(int locc=0){return "";}
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */
    //virtual llvm::Value* codegen(){return nullptr;}
    /*
        Emit llvm-bitcode to be compiled later
    */

    virtual std::string emit_cst(){return type->get_type() + " " + name + " = " + expression->emit_cst() + ";";}
    /*
        Emit C* code
    */
    
    virtual std::string get_type(){return name;}
    virtual std::string get_ll_type(){return "";}
    virtual void force_type(std::string type){}
    /*
        Try to enforce a specific type
    */

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};

class VarAccesAST : public AST {
    std::string name = "";
    symbol::SymbolReference* var = nullptr;

    public:
    VarAccesAST(std::string name, symbol::SymbolReference* sr);
    virtual bool is_const(){return false;} // do constant folding or not
    virtual ~VarAccesAST(){}
    virtual std::string emit_ll(int locc=0){return "";}
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */
    //virtual llvm::Value* codegen(){return nullptr;}
    /*
        Emit llvm-bitcode to be compiled later
    */

    virtual std::string emit_cst(){return name;}
    /*
        Emit C* code
    */
    
    virtual std::string get_type(){return name;}
    virtual std::string get_ll_type(){return "";}
    virtual void force_type(std::string type);
    /*
        Try to enforce a specific type
    */

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};

