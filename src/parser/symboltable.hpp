#pragma once

#include <llvm-14/llvm/IR/LLVMContext.h>
#include <string>
#include <vector>
#include <map>
#include "../lexer/lexer.hpp"

template<typename T, typename S>
using MultiMap = std::map<T, std::vector<S>>;

//extern llvm::LLVMContext* context;
//extern void setup_codegen();

namespace symbol {

    class Func;
    class CompType;
    class Operator_fn;

    class SymbolReference{

        public:
        virtual ~SymbolReference();

        private:
        friend class Namespace;
        friend class Module;

        public:
        std::string loc = "";
        virtual std::string find (std::string name) = 0;
        virtual std::vector<Func*> find_fn(std::string name) = 0;
        virtual SymbolReference* find_symbol(std::string name) = 0;
        virtual llvm::LLVMContext* get_context(){return nullptr;}

        SymbolReference(){}
        
    };

    class Namespace : public SymbolReference{

        public:
        virtual ~Namespace();

        protected:

        MultiMap<std::string, SymbolReference*> table = {};

        public:

        virtual std::string find (std::string name);
        virtual std::vector<Func*> find_fn (std::string name);
        virtual SymbolReference* find_symbol(std::string name);
        void add(std::string, SymbolReference* sr);
        //virtual llvm::LLVMContext* get_context(){return context;}

        Namespace(std::string l){loc = l;}
        Namespace(){table = {};}
        
    };

    class Var : public SymbolReference{
        std::string type = "";
        bool on_stack;

        public:
        bool used = false;
        lexer::Token declared;
        std::string find (std::string name);
        std::vector<Func*> find_fn (std::string name){return {};}

        Var(std::string l, std::string t, lexer::Token tok, bool s=true){loc = l; type = t; on_stack = s; declared = tok;}
        SymbolReference* find_symbol(std::string name);
        ~Var(){};

    };

    class Func : public SymbolReference{

        public:

        std::string type = "";
        std::vector<std::string> params = {};

        std::string find (std::string name);
        std::vector<Func*> find_fn (std::string name);

        Func(std::string l, std::string t, std::vector<std::string> p){loc = l; type = t; params = p;}
        SymbolReference* find_symbol(std::string name);
        ~Func(){};

        std::string get_sig();
    };

    class Operator_fn : public Func {};

    class SubBlock : public Namespace {
        SymbolReference* parent;

        public:
        SubBlock(SymbolReference* p){parent = p;}

        virtual std::string find (std::string name);
        virtual std::vector<Func*> find_fn (std::string name);
    };

    class CompType : public Namespace {
        std::vector<CompType*> super = {};

        public:

        bool is_final = false;
        bool is_interface = false;

        static std::vector<std::string> promised_types;

        CompType();

        bool has_super(std::string name);
        bool has_op(std::string op);

        virtual std::string find (std::string name);
        virtual std::vector<Func*> find_fn (std::string name);
    };
}





