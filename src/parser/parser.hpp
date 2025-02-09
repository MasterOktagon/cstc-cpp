#pragma once
#include "../lexer/lexer.hpp"
#include "symboltable.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <initializer_list>


template <typename T, typename ... K>
using fsignal = T (*) (K ... );

namespace parser {

    extern std::string match_cpp_types(std::string name);
    
    extern void warn(std::string name, lexer::Token t, std::string msg, int code);
    extern void error(std::string name, lexer::Token t, std::string msg, int code);
    extern void error(std::string name, lexer::Token from, lexer::Token to, std::string msg, int code);
    
    class AST {
    public:
        virtual std::string to_cpp(){return "";}
        virtual std::string emit_ll(){return "";}
        virtual std::string emit_cst(){return "";}

        static AST* parse(){return new AST;}
    };

    class TypeAST : public AST{
    public:
        std::string name;
        virtual std::string to_cpp(){return match_cpp_types(name);}
        virtual std::string emit_cst(){return name;}

        TypeAST(std::string n){
            name = n;
        }
        TypeAST(){};
        virtual std::string get_type(){return name;}
    };
    
    class MultiTypeAST : public TypeAST{
        public:
        virtual std::string to_cpp(){
            return std::string("") + c->to_cpp() + "," + t->to_cpp();
        };
        
        TypeAST* t = nullptr;
        TypeAST* c = nullptr;

        MultiTypeAST(){};
        virtual std::string get_type(){return name + ", " + c->get_type();}
    };

    class ComposeTypeAST : public TypeAST{
    public:
        std::string name;
        virtual std::string to_cpp(){
            return std::string("") + match_cpp_types(name) + "<" + t->to_cpp() + ">";
        };
        
        TypeAST* t =  {};

        ComposeTypeAST(std::string n){
            name = n;
        }
        
        virtual std::string get_type(){
            if (name == "Optional") return t->get_type() + "?";
            if (name == "Vector") return t->get_type() + "[]";
            if (name == "ptr") return t->get_type() + "*";
            return name + "<" + t->get_type() + ">";
        }
    };

    class PointerTypeAST : public TypeAST{
    public:
        std::string name;
        virtual std::string to_cpp(){
            return std::string("") + t->to_cpp() + "*";
        };
        TypeAST* t;

        PointerTypeAST(std::string n){
            name = n;
        }
        virtual std::string get_type(){return name + "<" + t->get_type() + ">";}
    };

    class OptionalTypeAST : public TypeAST{
    public:
        std::string name;
        virtual std::string to_cpp(){
            return std::string("std::optional<") + t->to_cpp() + ">";
        };
        TypeAST* t;

        OptionalTypeAST(std::string n){
            name = n;
        }
        virtual std::string get_type(){return t->get_type() + "?";}
    };
    
    class ExpressionAST : public AST {
    public:
        std::string type = "";
        virtual std::string to_cpp(){return "<Expression>";}
        ExpressionAST(){};
    };

    class VarCallAST : public ExpressionAST{
    public:
        std::string loc;
        virtual std::string to_cpp(){
            return loc;
        };

        VarCallAST(std::string name, std::string t){
            loc = name;
            type = t;
        }
    };

    class FuncCallAST : public ExpressionAST {
        public:
        std::string name="";
        std::vector<ExpressionAST*> params;
        
        virtual std::string to_cpp(){
            std::string d = name + "(";
            for (ExpressionAST* a : params){
                d += a->to_cpp() + ",";
            }
            if(d != name + "(") d = d.substr(0, d.size()-1);
            d += ")";
            return d;
        };

        FuncCallAST(std::string n, std::vector<ExpressionAST*> p){name = n; params = p;}
    };

    class BoolLiteralAST : public ExpressionAST{
    public:
        std::string str;
        bool value;
        virtual std::string to_cpp(){
            return str;
        };

        BoolLiteralAST(std::string s, std::string t){
            str = s;
            type = t;
            value = (s == "true");
        }
    };
    
    class IntLiteralAST : public ExpressionAST{
    public:
        std::string str;
        long value;
        virtual std::string to_cpp(){
            return str;
        };

        IntLiteralAST(std::string s, std::string t){
            str = s;
            type = t;
            value = std::stol(s);
        }
    };
    
    class FloatLiteralAST : public ExpressionAST{
    public:
        std::string str;
        double value;
        virtual std::string to_cpp(){
            return str;
        };

        FloatLiteralAST(std::string s, std::string t){
            str = s;
            type = t;
            value = std::stod(s);
        }
    };
    
    class AddAST : public ExpressionAST{
    public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " + " + right->to_cpp() + ")";
        };

        AddAST(){};
    };
    
    class SubAST : public ExpressionAST{
    public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " - " + right->to_cpp() + ")";
        };

        SubAST(){};
    };
    
    class MulAST : public ExpressionAST{
    public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " * " + right->to_cpp() + ")";
        };

        MulAST(){};
    };
    
    class DivAST : public ExpressionAST{
    public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " / " + right->to_cpp() + ")";
        };

        DivAST(){};
    };

    class ModAST : public ExpressionAST{
    public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " % " + right->to_cpp() + ")";
        };

        ModAST(){};
    };

    class PowAST : public ExpressionAST{
    public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("std::pow(") + left->to_cpp() + " , " + right->to_cpp() + ")";
        };

        PowAST(){};
    };
    
    class TypeCastAST : public ExpressionAST{
    public:
        ExpressionAST* expr;
        TypeAST* type_ast;
        
        void updateType(){
            type = type_ast->get_type();
        }
        
        virtual std::string to_cpp(){
            return match_cpp_types(type) + "(" + expr->to_cpp() + ")";
        };

        TypeCastAST(){};
    };

    class ClampAST : public ExpressionAST {
        public:
        ExpressionAST* expr;

        virtual std::string to_cpp(){
            return std::string("(") + expr->to_cpp() + ")";
        };

        ClampAST(){};
    };

    class BNotAST : public ExpressionAST {
        public:
        ExpressionAST* expr;

        virtual std::string to_cpp(){
            return std::string("(~") + expr->to_cpp() + ")";
        };

        BNotAST(){};
    };

    class BAndAST : public ExpressionAST {
        public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " & " + right->to_cpp() + ")";
        };

        BAndAST(){};
    };

    class BOrAST : public ExpressionAST {
        public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " | " + right->to_cpp() + ")";
        };

        BOrAST(){};
    };

    class BXorAST : public ExpressionAST {
        public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " ^ " + right->to_cpp() + ")";
        };

        BXorAST(){};
    };

    class LorAST : public ExpressionAST {
        public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " || " + right->to_cpp() + ")";
        };

        LorAST(){};
    };

    class LandAST : public ExpressionAST {
        public:
        ExpressionAST* left;
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(") + left->to_cpp() + " && " + right->to_cpp() + ")";
        };

        LandAST(){};
    };

    class LnotAST : public ExpressionAST {
        public:
        ExpressionAST* right;
        
        virtual std::string to_cpp(){
            return std::string("(!") + right->to_cpp() + ")";
        };

        LnotAST(){};
    };


    class StatementAST : public AST {
        public:
        
        virtual std::string to_cpp(){
            return std::string(";");
        };

        StatementAST(){};
    };

    class DeclareVariableAST : public StatementAST {
        public:
        std::string name="";
        TypeAST* type;
        
        virtual std::string to_cpp(){
            return type->to_cpp() + " " + name + ";";
        };

        DeclareVariableAST(){};
    };

    class InitVariableAST : public StatementAST {
        public:
        std::string name="";
        TypeAST* type;
        ExpressionAST* value;
        
        virtual std::string to_cpp(){
            return type->to_cpp() + " " + name + " = " + value->to_cpp() + ";";
        };

        InitVariableAST(){};
    };

    class SetVariableAST : public StatementAST {
        public:
        std::string name="";
        ExpressionAST* value;
        
        virtual std::string to_cpp(){
            return name + " = " + value->to_cpp() + ";";
        };

        SetVariableAST(){};
    };

    class Block : public StatementAST {
        public:
        
        std::vector<AST*> statements = {};

        virtual std::string to_cpp(){
            std::string s = "";
            for (AST* a : statements) s += std::string("\t") + a->to_cpp() + "\n";
            return s;
        };

        Block(){};
    };

    class ExprStatementAST : public StatementAST {
        public:
        ExpressionAST* expr;
        
        virtual std::string to_cpp(){
            return expr->to_cpp() + ";";
        };

        ExprStatementAST(ExpressionAST* e){expr = e;}
    };

    class FuncDefAST : public StatementAST {
        public:

        Block* contents;
        std::string name = "";
        std::string ret_type = "";
        std::vector<std::string> params = {};
        std::vector<std::string> types = {};

        virtual std::string to_cpp(){
            std::string out = match_cpp_types(ret_type) + " " + name + "(";
            for (int i=0; i<params.size(); i++){
                out += match_cpp_types(types[i]) + " " + params[i] + ",";
            }
            out = out.substr(0,out.size()-1);
            
            out += std::string(")") + "{\n" + contents->to_cpp() + "}\n";
            return out;
        };

        FuncDefAST(){}
    };

    
    extern TypeAST*       parse_one_of_type(std::vector<lexer::Token> tokens, std::vector<fsignal<TypeAST*,       std::vector<lexer::Token>, int, symbol::Namespace*, std::string>> functions, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern AST*           parse_one_of_ast (std::vector<lexer::Token> tokens, std::vector<fsignal<AST*,           std::vector<lexer::Token>, int, symbol::Namespace*>> functions, int local, symbol::Namespace* sr);
    extern ExpressionAST* parse_one_of     (std::vector<lexer::Token> tokens, std::vector<fsignal<ExpressionAST*, std::vector<lexer::Token>, int, symbol::Namespace*, std::string>> functions, int local, symbol::Namespace* sr, std::string expected_type="@unknown");

    extern int splitStack(std::vector<lexer::Token> tokens, std::initializer_list<lexer::Token::TokenType>, int local);
    extern int rsplitStack(std::vector<lexer::Token> tokens, std::initializer_list<lexer::Token::TokenType>, int local);

    extern int errorc;

    extern TypeAST* parse_opt_type(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern TypeAST* parse_type(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern TypeAST* parse_composite_type(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern TypeAST* parse_pointer(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern TypeAST* parse_vec_type(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern TypeAST* parse_single_type(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern TypeAST* parse_multi_type(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    
    extern ExpressionAST* parse_math(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_var_call(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_func_call(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_bool(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_number(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_add(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_cast(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_mul(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_pow(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_clamp(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_bnot(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_band(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_bxor(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_bor(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_lor(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_land(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");
    extern ExpressionAST* parse_lnot(std::vector<lexer::Token>, int local, symbol::Namespace* sr, std::string expected_type="@unknown");

    extern int get_id(std::vector<lexer::Token>, std::string&);

    extern AST* parse_var_declare(std::vector<lexer::Token>, int local, symbol::Namespace* sr);
    extern AST* parse_var_init(std::vector<lexer::Token>, int local, symbol::Namespace* sr);
    extern AST* parse_var_set(std::vector<lexer::Token>, int local, symbol::Namespace* sr);
    extern AST* parse_expr(std::vector<lexer::Token>, int local, symbol::Namespace* sr);
    extern AST* parse_import(std::vector<lexer::Token>, int local, symbol::Namespace* sr);
    extern AST* parse_subblock(std::vector<lexer::Token>, int local, symbol::Namespace* sr);
    extern AST* parse_func_def(std::vector<lexer::Token>, int local, symbol::Namespace* sr);

    extern std::string match_cpp_types(std::string name);
    extern bool type_eq(std::string a, std::string b);

    extern bool is_CamelCase(std::string text);
    extern bool is_pascalCase(std::string text);
    extern bool is_snake_case(std::string text);
    extern bool is_UPPER_CASE(std::string text);

    extern AST* parse(std::vector<lexer::Token>, int local, symbol::Namespace* sr);

    extern symbol::Func* get_best_fn(std::string name, std::vector<parser::ExpressionAST*> params, symbol::SymbolReference* sr, std::string expected_type="");
}
