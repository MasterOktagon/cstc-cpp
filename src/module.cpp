
#include "module.hpp"
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include "lexer/lexer.hpp"
#include "parser/ast/ast.hpp"
//#include "parser/parser.hpp"
#include "parser/ast/literal.hpp"
#include "parser/ast/base_math.hpp"
#include "parser/symboltable.hpp"

#define DEBUG
#define STD_PATH "/home/oskar/Programmierprojekte/C*/cstc-cpp/stdlib/"

std::map<std::string, Module*> Module::unknown_modules = {};
std::map<std::string, Module*> Module::known_modules = {};
std::list<Module*> Module::load_order = {};

Module::Module (std::string modulename, bool direct_filepath) : symbol::Namespace() {
    dependencies = {};
    module_name = modulename;

    if (!direct_filepath){
        size_t pos = modulename.find("::");
        while (pos != std::string::npos){
            modulename.replace(pos,2,"/");
        
            pos = modulename.find("::");
        }

        pos = modulename.find("std/");
        if(pos != std::string::npos){
            modulename = STD_PATH + modulename.substr(5, modulename.size()-5);
            module_name = module_name.substr(pos+1, module_name.size()-pos-1);
        }
    }
    else{
        size_t pos = modulename.rfind(".");
        modulename = modulename.substr(0, pos);
        pos = module_name.find("/");
        while (pos != std::string::npos){
            module_name.replace(pos,1,"::");
        
            pos = module_name.find("/");
        }
        pos = module_name.rfind(".");
        module_name = module_name.substr(0, pos);
    }

    loc = module_name;
    
    size_t pos = modulename.rfind("/");
    if (pos != std::string::npos) folder = modulename.substr(0, pos) + "::";
    else folder = "";

    #ifdef DEBUG
        std::cout << "Searching Module: " << modulename << std::endl;
    #endif

    if (std::filesystem::exists(std::filesystem::u8path(modulename + ".hst"))) {
        hst_filename = modulename + ".hst";
        if (!std::filesystem::exists(std::filesystem::u8path(modulename + ".cst"))){
            std::cerr << "\e[1;33mWARNING: \e[0mImplementation file for module \e[1m" << module_name << "\e[0m not found!" << std::endl;
        }
        #ifdef DEBUG
            std::cout << "Module " << module_name << " added from HST" << std::endl;
        #endif
    }
    else {
        if (std::filesystem::exists(std::filesystem::u8path(modulename + ".cst"))) {
        cst_filename = modulename + ".cst";
        #ifdef DEBUG
            std::cout << "Module " << module_name << " added from CST" << std::endl;
        #endif
        }
        else {
            if (Module::unknown_modules.count(module_name) == 0){
                Module::unknown_modules[module_name] = this;
                std::cerr << "\e[1;31mERROR:\e[0m Module not found: \e[1m" << module_name << "\e[0m" << std::endl;
                std::cerr << "(Maybe you forgot to add it to foreign sources?)" << std::endl;
            }
            return;
        }
    }

    Module::known_modules[module_name] = this;
    //Module::load_order.push_back(this);

    tokenize();
    preproc();
    addLangFn();

}

void Module::tokenize(){
    #ifdef DEBUG
        std::cout << "tokenizing " << module_name << std::endl;
    #endif
    if(is_known()){
        std::string filename = is_header() ? hst_filename : cst_filename;
        std::ifstream file(filename, std::ifstream::in);
        if(file.is_open()){
            std::string data;
            std::string line;
            while (getline(file, line)) {
                data += line + "\n";
            }
            #ifdef DEBUG
                std::cout << data << std::endl;
            #endif
            tokens = lexer::tokenize(data, cst_filename);
            #ifdef DEBUG
                for (lexer::Token t : tokens){
                    std::cout << t.type << "\t \"" << t.value << "\"\t at l:" << t.line << ":" << t.column << std::endl;
                }
            #endif

            file.close();
        }
        else{
            std::cerr << "\e[1;31mERROR:\e[0m File could not be opened: \e[1m" << filename << std::endl;
            std::exit(-500);
        }
    }
    #ifdef DEBUG
        else {
            std::cout << "\tunknown " << module_name << std::endl;
        }
    #endif

}

Module::~Module(){symbol::Namespace::~Namespace();}

void Module::preproc(){
    #ifdef DEBUG
        std::cout << "preprocessing " << module_name << std::endl;
    #endif
    std::string d;
    std::vector<lexer::Token> token_buffer = {};
    for (lexer::Token t : tokens){
        token_buffer.push_back(t);

        if(t.type == lexer::Token::TokenType::END_CMD || t.type == lexer::Token::TokenType::EF){
            // try to import module
            if(token_buffer[0].type == lexer::Token::TokenType::IMPORT){
                if(token_buffer[token_buffer.size()-1].type == lexer::Token::TokenType::END_CMD){
                    std::string new_module_name = "";
                    lexer::Token::TokenType last = lexer::Token::TokenType::SUBNS;

                    for(size_t i=1; i<token_buffer.size()-1; i++){
                        if(last == lexer::Token::TokenType::SUBNS){
                            if (token_buffer[i].type == lexer::Token::TokenType::ID){
                                new_module_name += token_buffer[i].value;
                            }
                            else { 
                                std::cerr << "\e[1;31mERROR:\e[0m encountered mismatching symbol: " << token_buffer[i].value << std::endl;
                                goto skip_import;
                            }
                        }
                        else if(last == lexer::Token::TokenType::ID){
                            if (token_buffer[i].type == lexer::Token::TokenType::SUBNS){
                                new_module_name += token_buffer[i].value;
                            }
                            else { 
                                std::cerr << "\e[1;31mERROR:\e[0m encountered mismatching symbol: " << token_buffer[i].value << std::endl;
                                goto skip_import;
                            }
                        }

                        last = token_buffer[i].type;
                    }

                    #ifdef DEBUG
                        std::cout << "import: " << new_module_name << std::endl;
                    #endif
                    if(Module::known_modules.count(new_module_name) + Module::unknown_modules.count(new_module_name) == 0){
                        d = folder + new_module_name;
                        dependencies.push_back(d);
                        Module* m = new Module(d, false);
                        if (m->is_known()){
                            Module::load_order.push_back(m);
                        }
                        //std::cout << new_module_name <<std::endl;
                        add(new_module_name, m);
                    }
                }
                else {
                    std::cerr << "\e[1;31mERROR:\e[0m Missing ';' [END_CMD] at the end of the line." << std::endl;
                    goto skip_import;
                }

            }
            skip_import: token_buffer = {};
        }
    }
    #ifdef DEBUG
        std::cout << "preprocessing completed for " << module_name << std::endl;
    #endif
}

bool Module::is_main_file(){
    return cst_filename != "";
}


bool Module::is_header(){
    return hst_filename != "";
}

bool Module::is_known(){
    return known_modules.find(module_name) != known_modules.end();
}

bool is_less(Module* a, Module* b){
    for (std::string mod : b->dependencies){
        if (mod == a->module_name) return true;
    }
    return false;
}

void Module::parse(){
    tokens.pop_back();
    root = math::parse(tokens, 0, this, "int32");

    if(root != nullptr){
        std::cout << root->emit_ll() << std::endl;
    }
    
}

void Module::addLangFn(){
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("int8")})));
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("int16")})));
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("int32")})));
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("int64")})));

    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("uint8")})));
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("uint16")})));
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("uint32")})));
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("uint64")})));

    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("float32")})));
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("float64")})));

    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("String")})));
    add("print", new symbol::Func(std::string("print"), std::string("void"), std::vector({std::string("Repr")})));
    
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("int8")})));
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("int16")})));
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("int32")})));
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("int64")})));

    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("uint8")})));
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("uint16")})));
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("uint32")})));
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("uint64")})));

    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("float32")})));
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("float64")})));

    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("String")})));
    add("println", new symbol::Func(std::string("println"), std::string("void"), std::vector({std::string("Repr")})));

    add("abs", new symbol::Func(std::string("abs"), std::string("int8"), std::vector({std::string("int8")})));
    add("abs", new symbol::Func(std::string("abs"), std::string("int16"), std::vector({std::string("int16")})));
    add("abs", new symbol::Func(std::string("abs"), std::string("int32"), std::vector({std::string("int32")})));
    add("abs", new symbol::Func(std::string("abs"), std::string("int64"), std::vector({std::string("int64")})));

    add("abs", new symbol::Func(std::string("abs"), std::string("float32"), std::vector({std::string("float32")})));
    add("abs", new symbol::Func(std::string("abs"), std::string("float64"), std::vector({std::string("float64")})));

    add("round", new symbol::Func(std::string("round"), std::string("int64"), std::vector({std::string("float32")})));
    add("round", new symbol::Func(std::string("round"), std::string("int64"), std::vector({std::string("float64")})));
    add("round", new symbol::Func(std::string("round"), std::string("float32"), std::vector({std::string("float32"), std::string("uint8")})));
    add("round", new symbol::Func(std::string("round"), std::string("float64"), std::vector({std::string("float64"), std::string("uint8")})));

    add("ceil", new symbol::Func(std::string("ceil"), std::string("int64"), std::vector({std::string("float32")})));
    add("ceil", new symbol::Func(std::string("ceil"), std::string("int64"), std::vector({std::string("float64")})));

    add("exit", new symbol::Func(std::string("exit"), std::string("void"), std::vector({std::string("uint8")})));
}
