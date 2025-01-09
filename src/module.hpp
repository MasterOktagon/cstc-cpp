
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include "lexer/lexer.hpp"
#include "parser/symboltable.hpp"
#include "parser/parser.hpp"

class Module : public symbol::Namespace {

    std::vector<lexer::Token> tokens;

    void addLangFn();

    public:

        static std::map<std::string, Module*> known_modules;
        static std::map<std::string, Module*> unknown_modules;
        static std::list<Module*> load_order;

        parser::AST* root;

        std::vector<std::string> dependencies = {};

        bool lexed = false;
        bool parsed = false;
        
        std::string cst_filename = "";
        std::string hst_filename = "";
        std::string module_name = "";
        std::string folder = "";

        void tokenize();
        void preproc();
        void parse();

        bool is_main_file();
        bool is_header();
        bool is_known();

        std::string operator [] (std::string name);

        Module(std::string modulename, bool direct_filepath = false);
        virtual ~Module();

};

extern bool is_less(Module* a, Module* b);





