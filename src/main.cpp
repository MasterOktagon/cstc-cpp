
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
        return -1;
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
                    return -2;
                }
            }
            else {
                std::cerr << "\e[1;31mERROR:\e[0m You can only pass one main file!" << std::endl;
                return -10;
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
        return 43;
    }
    Module prog_root = Module(main_file, true);
    Module::load_order.push_back(&prog_root);

    if(Module::load_order.size() < 1) std::exit(-67);
    Module::load_order.sort(is_less);

    std::cout << Module::load_order.size() << " Module(s) loaded succesfully" << std::endl;

    if (flags.count("--list-modules") > 0){
        std::cout << "\e[1;36mINFO:\e[0m Modules loaded:" << std::endl;
        for (Module* mod : Module::load_order){
            std::cout << "\t\e[1m" << mod->module_name << "\e[0m" << (mod->is_header()? " [from Header] " : "") << std::endl;
        } 
    }

    for(Module* m : Module::load_order){
        std::cout << m->module_name << std::endl;
        m->parse();
    }
    
    if (parser::errc > 0){
        
    }
    else{
        std::cout << "Complete!" << std::endl;
    }
    return 0;
}



