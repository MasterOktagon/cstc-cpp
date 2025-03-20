#pragma once
//#include <llvm-14/llvm/IR/Value.h>
#include <string>
#include <vector>
#include "../../lexer/lexer.hpp"

class AST {
    protected:
    std::vector<lexer::Token> tokens;

    public:
    AST(){}
    virtual bool is_const(){return false;} // do constant folding or not
    virtual ~AST(){}
    virtual int nodeSize(){return 1;} // how many nodes to to do
    virtual std::string emit_ll(int*, std::string){return "";}
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */
    //virtual llvm::Value* codegen(){return nullptr;}
    /*
        Emit llvm-bitcode to be compiled later
    */

    virtual std::string emit_cst(){return "";}
    /*
        Emit C* code
    */
    
    virtual std::string get_type(){return "@unknown";}
    virtual std::string get_ll_type(){return "";}
    virtual void force_type(std::string){}
    /*
        Try to enforce a specific type
    */
};

extern std::string max_prec_type(std::string a, std::string b);
extern std::string intab(std::string);
extern std::string insert(std::string val, std::string target);
extern std::string rinsert(std::string val, std::string target);

