#pragma once
#include <string>

class AST {

    public:
    AST(){}
    virtual ~AST(){}
    virtual std::string emit_ll(int locc=0){return "";}
    /*
        Emit llvm IR code in human-readable form

        [param locc] local variable name counter
    */

    virtual std::string emit_cst(){return "";}
    /*
        Emit C* code
    */
    
    virtual std::string get_type(){return "@unknown";}
    virtual std::string get_ll_type(){return "";}
};

extern std::string max_prec_type(std::string a, std::string b);
extern std::string intab(std::string);

