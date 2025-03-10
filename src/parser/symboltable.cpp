
#include "symboltable.hpp"
#include <llvm-14/llvm/IR/LLVMContext.h>
#include <string>
#include <map>
#include <iostream>

//llvm::LLVMContext* context;

//void setup_codegen(){
//    context = new llvm::LLVMContext();
//}

symbol::SymbolReference::~SymbolReference(){}

void symbol::Namespace::add(std::string k, SymbolReference* sr){
    if(table.count(k) > 0) {table[k].push_back(sr);}
    else table[k] = {sr};
}

symbol::Namespace::~Namespace(){
    for (std::pair<std::string,std::vector<SymbolReference*>> sr : table){
        for(SymbolReference* s : sr.second){
            delete s;
        }
    }
}

std::string symbol::Var::find (std::string name){
    //std::cout << "FOUND: " <<  loc << std::endl;
    return type;
}

std::string symbol::Func::find (std::string name){
    //std::cout << "FOUND: " <<  loc << std::endl;
    std::string d = std::string("Signal<") + type;
    for( std::string p : params){
        d += "," + p;
    }
    d += ">";
    return d;
}

std::vector<symbol::Func*> symbol::Func::find_fn (std::string name){
    if (name == "") return {this};
    return {};
}


std::string symbol::Namespace::find (std::string name){
    //std::cout << "FIND: " <<  name << " in " << loc << std::endl;
    if(name == "") return "";
    size_t pos = name.find("::");
    if (pos != std::string::npos){
        if (table.count(name.substr(0, pos)) > 0) {
            return table[name.substr(0, pos)][0]->find(name.substr(pos+2, name.size()));
        }
    }
    else if (table.count(name) > 0){
        return table[name][0]->find("");
    }
    return "";
}

std::vector<symbol::Func*> symbol::Namespace::find_fn (std::string name){
    //std::cout << "FIND: " <<  name << std::endl;
    size_t pos = name.find("::");
    if (pos != std::string::npos){
            if (table.count(name.substr(0, pos)) > 0){
            std::vector<Func*> out = {};
            for (SymbolReference* sr : table[name.substr(pos+2, name.size())]){
                auto f = sr->find_fn("");
                out.insert(out.end(), f.begin(), f.end());
            }
            return out;
        }
    }
    else if (table.count(name) > 0){
        std::vector<Func*> out = {};
        for (SymbolReference* sr : table[name]){
            auto f = sr->find_fn("");
            out.insert(out.end(), f.begin(), f.end());
        }
        return out;
    }
    return {};
}

std::string symbol::Func::get_sig(){
    std::string out = type + " " + loc + "(";
    for (std::string p : params){
        out += p + ",";
    }
    return out + ")";
}



std::string symbol::SubBlock::find (std::string name){
    //std::cout << "FIND: " <<  name << " in " << loc << std::endl;
    if(name == "") return "";
    size_t pos = name.find("::");
    if (pos != std::string::npos){
        if (table.count(name.substr(0, pos)) > 0) {
            return table[name.substr(0, pos)][0]->find(name.substr(pos+2, name.size()));
        }
    }
    else if (table.count(name) > 0){
        return table[name][0]->find("");
    }
    return parent->find(name);
}

std::vector<symbol::Func*> symbol::SubBlock::find_fn (std::string name){
    //std::cout << "FIND: " <<  name << std::endl;
    size_t pos = name.find("::");
    if (pos != std::string::npos){
            if (table.count(name.substr(0, pos)) > 0){
            std::vector<Func*> out = {};
            for (SymbolReference* sr : table[name.substr(pos+2, name.size())]){
                auto f = sr->find_fn("");
                out.insert(out.end(), f.begin(), f.end());
            }
            return out;
        }
    }
    else if (table.count(name) > 0){
        std::vector<Func*> out = {};
        for (SymbolReference* sr : table[name]){
            auto f = sr->find_fn("");
            out.insert(out.end(), f.begin(), f.end());
        }
        return out;
    }
    return parent->find_fn(name);
}


