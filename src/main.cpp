
#include <iostream>
#include <map>
#include <string>
#include <filesystem>

#include "module.hpp"
#include "parser/parser.hpp"

//#define DEBUG

int main(int argc, const char** argv){

    if (argc < 2){
        std::cerr << "\e[1;31mERROR:\e[0m No File specified" << std::endl;
        return 1;
    }

    std::map<std::string, std::string> flags = {};
    bool main_file_loaded = false;
    std::string main_file = "";

    for(int i=1; i<argc; i++){
        if (argv[i][0] != '-'){
            if (!main_file_loaded){
                if (std::filesystem::exists(std::filesystem::u8path(argv[i]))) {
                    #ifdef DEBUG
                        std::cout << "Main file found: " << argv[i] << std::endl;
                    #endif
                    main_file = argv[i];
                    main_file_loaded = true;
                }
                else{
                    std::cerr << "\e[1;31mERROR:\e[0m File not found: " << argv[i] << std::endl;
                    return 3;
                }
            }
            else {
                std::cerr << "\e[1;31mERROR:\e[0m You can only pass one main file!" << std::endl;
                return 4;
            }
        }
        else {
            std::string arg = argv[i];
            size_t split_pos = arg.find("=");
            if (split_pos != std::string::npos){
                flags[arg.substr(0,split_pos)] = arg.substr(split_pos+1, arg.size()-split_pos-1);
            }
            else {
                flags[arg] = "";
            }
        }
    }

    #ifdef DEBUG
        std::cout << "FLAGS: " << std::endl;
        for (std::pair<std::string, std::string> el : flags){
            std::cout << "\t" << el.first << " = " << el.second << std::endl;
        }
    #endif

    if(!main_file_loaded){
        std::cerr << "\e[1;31mERROR:\e[0m No main file specified!" << std::endl;
        return 1;
    }
    if (flags.count("--one-error") > 0){
        parser::one_error = true;
    }

    Module prog_root = Module(main_file, true);
    Module::load_order.push_back(&prog_root);

    if(Module::load_order.size() < 1) std::exit(-1);
    Module::load_order.sort(is_less);

    std::cout << Module::load_order.size() << " Module(s) loaded succesfully" << std::endl;

    if (flags.count("--list-modules") > 0 || flags.count("-l")){
        std::cout << "\e[1;36mINFO:\e[0m Modules loaded:" << std::endl;
        for (Module* mod : Module::load_order){
            std::string s = mod->module_name;
            while (s.size() < 80) s += " ";
            std::cout << "\t\e[1m" << s << "\e[0;36m" << (mod->is_header()? " [from Header]\e[0m\t @ " : "\e[0m\t\t @ ") << (mod->is_header() ? mod->hst_filename : mod->cst_filename) << std::endl;
        } 
    }

    for(Module* m : Module::load_order){
        std::cout << m->module_name << std::endl;
        m->parse();
    }
    
    if (parser::errc > 0 || parser::warnc > 0){
        std::cout << "\n";
        std::cout << parser::errc << " error" << (parser::errc == 1 ? ", " : "s, ") << parser::warnc << " warning" << (parser::warnc == 1 ? "" : "s") << " generated\n";
        if (parser::warnc > 0 && flags.count("--punish") > 0){
            std::cout << "Treating warnings as errors (--punish)\n\e[1;31mCompilation aborted\e[0m\n";
            std::exit(2);
        }
        else if(parser::errc > 0){
            std::cout << "\e[1;31mCompilation aborted\e[0m\n";
            std::exit(2);
        }
        goto compile;
    }
    else{
        compile:
        std::cout << "Complete!" << std::endl;
    }
    std::exit(0);
}



