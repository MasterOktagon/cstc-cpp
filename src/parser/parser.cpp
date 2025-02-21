#include "parser.hpp"

#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "../lexer/lexer.hpp"
#include <cmath>
#include <regex>
#include "ast/ast.hpp"
#include "symboltable.hpp"


unsigned int parser::errc = 0;
bool parser::one_error = false;

#define DEBUG

void parser::error(std::string name, lexer::Token t, std::string msg, int code, std::string appendix){
    std::cerr << std::string("\e[1;31mERROR: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-2) + "\e[0m [E "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
    std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
    std::string pb = " ";
    
    for (unsigned int i=0; i<std::to_string(t.line).size(); i++) pb += " ";
    pb += " |\t";
    for (unsigned int i=0; i<t.column-2; i++){
        pb += " ";
    }
    pb += "\e[31m^";
    for (size_t i=1; i<t.value.size(); i++){
        pb += "^";
    }
    pb += "\e[0m";

    std::cerr << pb << std::endl << std::endl;
    if (appendix != "") std::cerr << appendix << std::endl;
    errc++;
    if (one_error) std::exit(1);
}

void parser::error(std::string name, lexer::Token t, lexer::Token t2, std::string msg, int code, std::string appendix){
    if (t.line == t2.line){
        std::cerr << std::string("\e[1;31mERROR: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-2) + "\e[0m [E "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
        std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
        std::string pb = " ";
        
        for (size_t i=0; i<std::to_string(t.line).size(); i++) pb += " ";
        pb += " |\t";
        for (size_t i=0; i<t.column-2; i++){
            pb += " ";
        }
        pb += "\e[31m^";
        for (size_t i=1; i<t2.column + t2.value.size() - t.column + 2; i++){
            pb += "^";
        }
        pb += "\e[0m";

        std::cerr << pb << std::endl << std::endl;
        if (appendix != "") std::cerr << appendix << std::endl;
    }
    else {
        std::cerr << std::string("\e[1;31mERROR: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-2) + " - " + std::to_string(t2.line) + ":" + std::to_string(t2.column-1) + "\e[0m [E "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
        std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
        std::string pb = " ";

        for (size_t i=0; i<std::to_string(t.line).size(); i++) pb += " ";
        pb += " |\t";
        for (size_t i=0; i<t.column-2; i++){
            pb += " ";
        }
        pb += "\e[31m^";
        for (size_t i=1; i<t.line_content.size() - (t.column); i++){
            pb += "^";
        }
        pb += "\e[0m";
        for (size_t i=0; i<std::to_string(t.line).size(); i++) pb += " ";
        if(t2.line - t.line > 1){
            pb += std::string("\n ");
            for (size_t i=0; i<std::to_string(t.line).size(); i++) pb += " ";
            pb += std::string(" |\t\t\e[31m( ") + std::to_string(t2.line - t.line-1) + " line(s) hidden)\e[0m\n";
        }
        else pb += "\n";
        pb += std::string(" ") + std::to_string(t2.line) + " |\t" + t2.line_content + "\n ";
        for (size_t i=0; i<std::to_string(t2.line).size(); i++) pb += " ";
        pb += " |\t";
        pb += "\e[31m^";
        for (size_t i=1; i<t2.column+t2.value.size()+2; i++){
            pb += "^";
        }
        pb += "\e[0m";

        std::cerr << pb << std::endl << std::endl;
        if (appendix != "") std::cerr << appendix << std::endl;
    }
    errc++;
    if (one_error) std::exit(1);
}

void parser::warn(std::string name, lexer::Token t, std::string msg, int code){
    std::cerr << std::string("\e[1;33mWARNING: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-1) + "\e[0m [W "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
    std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
    std::string pb = " ";
    
    for (size_t i=0; i<std::to_string(t.line).size(); i++) pb += " ";
    pb += " |\t";
    for (size_t i=0; i<t.column-2; i++){
        pb += " ";
    }
    pb += "\e[33m^";
    for (size_t i=1; i<t.value.size(); i++){
        pb += "^";
    }
    pb += "\e[0m";

    std::cerr << pb << std::endl << std::endl;
}

//template <typename T>
//std::vector<T> parser::subvector(std::vector<T> v, int start, int step, int stop){
//    auto s = v.begin() + start; auto end = v.begin() + stop;
//    step += 1;
//    return std::vector<T>(s, end);
//}

#define INT_OPS(type)                                                            \
    if (type1 == type){                                                          \
        if (op == lexer::Token::TokenType::NOT) return "";                       \
        if (op == lexer::Token::TokenType::NEG) return type;                     \
        if(type2 == "bool"){                                                     \
            if (op == lexer::Token::TokenType::AS) return "bool";                \
        }                                                                        \
        if(std::regex_match(type2, int_regex)){                                  \
            if (op == lexer::Token::TokenType::AS) return type2;                 \
        }                                                                        \
        if(type2 == "char"){                                                     \
            if (op == lexer::Token::TokenType::AS) return "char";                \
        }                                                                        \
        if (type2 == type){                                                      \
            if (op == lexer::Token::TokenType::AND) return type;                 \
            if (op == lexer::Token::TokenType::OR) return type;                  \
            if (op == lexer::Token::TokenType::XOR) return type;                 \
            if (op == lexer::Token::TokenType::ADD) return type;                 \
            if (op == lexer::Token::TokenType::SUB) return type;                 \
            if (op == lexer::Token::TokenType::MUL) return type;                 \
            if (op == lexer::Token::TokenType::DIV) return type;                 \
            if (op == lexer::Token::TokenType::MOD) return type;                 \
            if (op == lexer::Token::TokenType::POW) return type;                 \
        } \
    }


std::string parser::hasOp(std::string type1, std::string type2, lexer::Token::TokenType op){
    std::regex int_regex("u?int(8|16|32|64|128)");
    //std::regex float_regex("u?int(8|16|32|64|128)");
    if (type1 == "bool"){
        if (op == lexer::Token::TokenType::NOT) return "bool";
        if (op == lexer::Token::TokenType::NEG) return "";
        if(type2 == "bool"){
            if (op == lexer::Token::TokenType::LAND) return "bool";
            if (op == lexer::Token::TokenType::LOR) return "bool";
            if (op == lexer::Token::TokenType::NEQ) return "bool";
            if (op == lexer::Token::TokenType::EQ) return "bool";
            if (op == lexer::Token::TokenType::AND) return "bool";
            if (op == lexer::Token::TokenType::OR) return "bool";
            if (op == lexer::Token::TokenType::XOR) return "bool";

        }
        if(std::regex_match(type2, int_regex)){
            if (op == lexer::Token::TokenType::AS) return type2;
        }
    }
    INT_OPS("@int");
    INT_OPS("uint16");
    INT_OPS("uint32");
    INT_OPS("uint64");
    INT_OPS("uint128");

    INT_OPS("int8");
    INT_OPS("int16");
    INT_OPS("int32");
    INT_OPS("int64");
    INT_OPS("int128");

    return "";
}


std::string match_token_clamp(lexer::Token::TokenType t){
    switch (t){
        case lexer::Token::TokenType::PT_OPEN:  return "PARANTHESIS";
        case lexer::Token::TokenType::PT_CLOSE: return "PARANTHESIS";

        case lexer::Token::TokenType::INDEX_OPEN:  return "INDEX";
        case lexer::Token::TokenType::INDEX_CLOSE: return "INDEX";

        case lexer::Token::TokenType::BLOCK_OPEN:  return "BLOCK";
        case lexer::Token::TokenType::BLOCK_CLOSE: return "BLOCK";

        default: return "UNKNOWN";
    }
}

int parser::splitStack(std::vector<lexer::Token> t, std::initializer_list<lexer::Token::TokenType> delimiter, int local, std::initializer_list<lexer::Token::TokenType> prefix_ignore){
    std::stack<lexer::Token> s = {};

    int i;
    for(i=t.size()-1; i>=0 ; i--){
        if (t[i].type == lexer::Token::TokenType::PT_CLOSE || t[i].type == lexer::Token::TokenType::INDEX_CLOSE || t[i].type == lexer::Token::TokenType::BLOCK_CLOSE) {
            s.push(t[i]);
        }

        else if (t[i].type == lexer::Token::TokenType::PT_OPEN){
            if(s.size() == 0) parser::error("Unclosed CLAMP", t[i], "This CLAMP was not closed" , 46);
            if(s.top().type != lexer::Token::TokenType::PT_CLOSE) parser::error("Unopened " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not opened" , 47);
            s.pop();
        }

        else if (t[i].type == lexer::Token::TokenType::INDEX_OPEN){
            if(s.size() == 0) parser::error("Unclosed INDEX", t[i], "This INDEX was not closed" , 46);
            if(s.top().type != lexer::Token::TokenType::INDEX_CLOSE) parser::error("Unopened " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not opened" , 47);
            s.pop();
        }

        else if (t[i].type == lexer::Token::TokenType::BLOCK_OPEN){
            if(s.size() == 0) parser::error("Unclosed BLOCK", t[i], "This BLOCK was not closed" , 46);
            if(s.top().type != lexer::Token::TokenType::BLOCK_CLOSE) parser::error("Unopened " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not opened" , 47);
            s.pop();
        }
        if (s.size() == 0 && std::find(delimiter.begin(), delimiter.end(), t[i].type) != delimiter.end()) return i;
    
    }
    for(int j=0; j<s.size(); j++){
        parser::error("Unopened " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not opened" , 47);
        s.pop();
    }
    return i;
}

int parser::rsplitStack(std::vector<lexer::Token> t, std::initializer_list<lexer::Token::TokenType> delimiter, int local, std::initializer_list<lexer::Token::TokenType> prefix_ignore){
    std::stack<lexer::Token> s = {};

    int i;
    for(i=0; i<t.size(); i++){
        if (t[i].type == lexer::Token::TokenType::PT_OPEN || t[i].type == lexer::Token::TokenType::INDEX_OPEN || t[i].type == lexer::Token::TokenType::BLOCK_OPEN) {
            s.push(t[i]);
        }

        else if (t[i].type == lexer::Token::TokenType::PT_CLOSE){
            if(s.size() == 0) parser::error("Unopened CLAMP", t[i], "This CLAMP was not opened" , 46);
            if(s.top().type != lexer::Token::TokenType::PT_OPEN) parser::error("Unclosed " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not closed" , 47);
            s.pop();
        }

        else if (t[i].type == lexer::Token::TokenType::INDEX_CLOSE){
            if(s.size() == 0) parser::error("Unopened INDEX", t[i], "This INDEX was not opened" , 46);
            if(s.top().type != lexer::Token::TokenType::INDEX_OPEN) parser::error("Unclosed " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not closed" , 47);
            s.pop();
        }

        else if (t[i].type == lexer::Token::TokenType::BLOCK_CLOSE){
            if(s.size() == 0) parser::error("Unopened BLOCK", t[i], "This BLOCK was not opened" , 46);
            if(s.top().type != lexer::Token::TokenType::BLOCK_OPEN) parser::error("Unclosed " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not closed" , 47);
            s.pop();
        }
        if (s.size() == 0 && std::find(delimiter.begin(), delimiter.end(), t[i].type) != delimiter.end()) return i;
    
    }
    for(int j=0; j<s.size(); j++){
        parser::error("Unclosed " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not closed" , 47);
        s.pop();
    }
    return i;
}

AST* parser::parseOneOf(std::vector<lexer::Token> tokens, std::vector<fsignal<AST*, std::vector<lexer::Token>, int, symbol::Namespace*, std::string>> functions, int local, symbol::Namespace* sr, std::string expected_type){
    for (auto fn : functions){
        AST* r = fn(tokens, local, sr, expected_type);
        if (r != nullptr){
            #ifdef DEBUG
                std::cout << "parse_one_of: " << r->emit_cst()  << std::endl;
            #endif
            return r;
            break;
        }
    }
    return nullptr;
}

bool parser::typeEq(std::string a, std::string b){
    if (a == "@unknown" || b == "@unknown") return true; // @unknown should ONLY be used for temporarily unknown types
    if (a == b) return true;
    if (a == "@int"){
        return (b == "int8" || b == "int16" || b == "int32" || b == "int64" || b == "int128" || b == "int256" || b == "int512" || b == "int1024" || 
            b == "uint8" || b == "uint16" || b == "uint32" || b == "uint64" || b == "uint128" || b == "uint256" || b == "uint512" || b == "uint1024" || b == "@uint");
    }
    if (a == "@float"){return b == "float32" || b == "float64";}
    if (b == "@int"){
        return (a == "int8" || a == "int16" || a == "int32" || a == "int64" || a == "int128" || a == "int256" || a == "int512" || a == "int1024" || 
            a == "uint8" || a == "uint16" || a == "uint32" || a == "uint64" || a == "uint128" || a == "uint256" || a == "uint512" || a == "uint1024" || a == "@uint");
    }
    if (a == "@uint"){
        return b == "uint8" || b == "uint16" || b == "uint32" || b == "uint64" || b == "uint128" || b == "uint256" || b == "uint512" || b == "uint1024" || b == "@int";
    }
    if (b == "@uint"){
        return a == "uint8" || a == "uint16" || a == "uint32" || a == "uint64" || a == "uint128" || a == "uint256" || a == "uint512" || a == "uint1024" || a == "@int";
    }
    if (b == "@float"){return a == "float32" || a == "float64";}
    
    return false;
}

bool parser::isAtomic(std::string type){
    if(type == "uint8") return true;
    if(type == "uint16") return true;
    if(type == "uint32") return true;
    if(type == "uint64") return true;
    if(type == "uint128") return true;

    if(type == "int8") return true;
    if(type == "int16") return true;
    if(type == "int32") return true;
    if(type == "int64") return true;
    if(type == "int128") return true;

    if(type == "char") return true;
    if(type == "float16") return true;
    if(type == "float32") return true;
    if(type == "float64") return true;
    if(type == "float128") return true;
    if(type[type.size()-1] == '*') return true;
    if(type[type.size()-1] == '&') return true;
    return false;
}
