#pragma once
#include "ast.hpp"
#include <vector>
#include "../../lexer/lexer.hpp"
#include "../symboltable.hpp"
#include "base_math.hpp"
#include <string>

AST* parseStatement(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
extern std::string parse_name(std::vector<lexer::Token>);

class VarDeclAST : public AST {

    std::string name = "";
    AST* type = nullptr;

    public:
    VarDeclAST(std::string name, AST* type);
    virtual bool is_const(){return false;} // do constant folding or not
    virtual ~VarDeclAST(){delete type;}
    virtual int nodeSize(){return 1;} // how many nodes to to do
    virtual std::string emit_ll(int*, std::string);
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
    virtual ~VarInitlAST(){delete type; delete expression;}
    virtual int nodeSize(){return expression->nodeSize() + 1;} // how many nodes to to do
    virtual std::string emit_ll(int*, std::string);
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
    virtual int nodeSize(){return 1;} // how many nodes to to do
    virtual std::string emit_ll(int*, std::string);
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
    
    virtual std::string get_type(){return var->find("");}
    virtual std::string get_ll_type(){return "";}
    virtual void force_type(std::string type);
    /*
        Try to enforce a specific type
    */

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};

class VarSetAST : public ExpressionAST {
    std::string name = "";
    symbol::SymbolReference* var = nullptr;
    AST* expr = nullptr;

    public:
    VarSetAST(std::string name, symbol::SymbolReference* sr, AST* expr);
    virtual bool is_const(){return false;} // do constant folding or not
    virtual ~VarSetAST(){delete expr;}
    virtual int nodeSize(){return expr->nodeSize() + 1;} // how many nodes to to do
    virtual std::string emit_ll(int*, std::string);
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */
    //virtual llvm::Value* codegen(){return nullptr;}
    /*
        Emit llvm-bitcode to be compiled later
    */

    virtual std::string emit_cst(){return std::string("(") + name + " = " + expr->emit_cst() + ")";}
    /*
        Emit C* code
    */
    
    virtual std::string get_type(){return var->find("");}
    virtual std::string get_ll_type(){return "";}
    virtual void force_type(std::string type);
    /*
        Try to enforce a specific type
    */

    static AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
};

