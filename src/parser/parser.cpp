#include "parser.hpp"

#include <iostream>
#include <string>
#include <vector>
#include "../lexer/lexer.hpp"
#include <cmath>
#include <regex>

unsigned int parser::errc = 0;

void parser::error(std::string name, lexer::Token t, std::string msg, int code){
    std::cerr << std::string("\e[1;31mERROR: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-1) + "\e[0m [E "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
    std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
    std::string pb = " ";
    
    for (unsigned int i=0; i<std::to_string(t.line).size(); i++) pb += " ";
    pb += " |\t";
    for (unsigned int i=0; i<t.column-2; i++){
        pb += " ";
    }
    pb += "\e[31m^";
    for (int i=1; i<t.value.size(); i++){
        pb += "^";
    }
    pb += "\e[0m";

    std::cerr << pb << std::endl << std::endl;
    errc++;
}

void parser::error(std::string name, lexer::Token t, lexer::Token t2, std::string msg, int code){
    if (t.line == t2.line){
        std::cerr << std::string("\e[1;31mERROR: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-1) + "\e[0m [E "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
        std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
        std::string pb = " ";
        
        for (int i=0; i<std::to_string(t.line).size(); i++) pb += " ";
        pb += " |\t";
        for (int i=0; i<t.column-2; i++){
            pb += " ";
        }
        pb += "\e[31m^";
        for (int i=1; i<t2.column + t2.value.size() - t.column; i++){
            pb += "^";
        }
        pb += "\e[0m";

        std::cerr << pb << std::endl << std::endl;
    }
    else {
        std::cerr << std::string("\e[1;31mERROR: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-1) + " - " + std::to_string(t2.line) + ":" + std::to_string(t2.column-1) + "\e[0m [E "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
        std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
        std::string pb = " ";

        for (int i=0; i<std::to_string(t.line).size(); i++) pb += " ";
        pb += " |\t";
        for (unsigned int i=0; i<t.column-2; i++){
            pb += " ";
        }
        pb += "\e[31m^";
        for (int i=1; i<t.line_content.size() - (t.column-2); i++){
            pb += "^";
        }
        pb += "\e[0m";
        for (int i=0; i<std::to_string(t.line).size(); i++) pb += " ";
        if(abs(t2.line - t.line) > 1){
            pb += std::string("\n ");
            for (int i=0; i<std::to_string(t.line).size(); i++) pb += " ";
            pb += std::string(" |\t\t\e[31m( ") + std::to_string(abs(t2.line - t.line)-1) + " line(s) hidden)\e[0m\n";
        }
        else pb += "\n";
        pb += std::string(" ") + std::to_string(t2.line) + " |\t" + t2.line_content + "\n ";
        for (int i=0; i<std::to_string(t2.line).size(); i++) pb += " ";
        pb += " |\t";
        pb += "\e[31m^";
        for (int i=1; i<t2.column+t2.value.size(); i++){
            pb += "^";
        }
        pb += "\e[0m";

        std::cerr << pb << std::endl << std::endl;
    }
    errc++;
}

void parser::warn(std::string name, lexer::Token t, std::string msg, int code){
    std::cerr << std::string("\e[1;33mWARNING: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-1) + "\e[0m [W "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
    std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
    std::string pb = " ";
    
    for (int i=0; i<std::to_string(t.line).size(); i++) pb += " ";
    pb += " |\t";
    for (int i=0; i<t.column-2; i++){
        pb += " ";
    }
    pb += "\e[33m^";
    for (int i=1; i<t.value.size(); i++){
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
    if (type1 == type){                                                     \
        if (op == lexer::Token::TokenType::NOT) return false;                    \
        if (op == lexer::Token::TokenType::NEG) return true;                     \
        if(regex_match(type2, int_regex) || type2 == "bool"){                    \
            if (op == lexer::Token::TokenType::AS) return true;                  \
        }                                                                        \
        if (type2 == type){                                                      \
            if (op == lexer::Token::TokenType::AND) return true;                 \
            if (op == lexer::Token::TokenType::OR) return true;                  \
            if (op == lexer::Token::TokenType::XOR) return true;                 \
            if (op == lexer::Token::TokenType::ADD) return true;                 \
            if (op == lexer::Token::TokenType::SUB) return true;                 \
            if (op == lexer::Token::TokenType::MUL) return true;                 \
            if (op == lexer::Token::TokenType::DIV) return true;                 \
            if (op == lexer::Token::TokenType::MOD) return true;                 \
            if (op == lexer::Token::TokenType::POW) return true;                 \
        } \
    }


bool hasOp(std::string type1, std::string type2, lexer::Token::TokenType op){
    std::regex int_regex("u?int(8|16|32|64|128)");
    //std::regex float_regex("u?int(8|16|32|64|128)");
    if (type1 == "bool"){
        if (op == lexer::Token::TokenType::NOT) return true;
        if (op == lexer::Token::TokenType::NEG) return false;
        if(type2 == "bool"){
            if (op == lexer::Token::TokenType::LAND) return true;
            if (op == lexer::Token::TokenType::LOR) return true;
            if (op == lexer::Token::TokenType::NEQ) return true;
            if (op == lexer::Token::TokenType::EQ) return true;
            if (op == lexer::Token::TokenType::AND) return true;
            if (op == lexer::Token::TokenType::OR) return true;
            if (op == lexer::Token::TokenType::XOR) return true;

        }
        if(regex_match(type2, int_regex)){
            if (op == lexer::Token::TokenType::AS) return true;
        }
    }
    INT_OPS("uint8");
    INT_OPS("uint16");
    INT_OPS("uint32");
    INT_OPS("uint64");
    INT_OPS("uint128");

    INT_OPS("int8");
    INT_OPS("int16");
    INT_OPS("int32");
    INT_OPS("int64");
    INT_OPS("int128");

    return false;
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

int parser::splitStack(std::vector<lexer::Token> t, std::initializer_list<lexer::Token::TokenType> delimiter, int local){
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

int parser::rsplitStack(std::vector<lexer::Token> t, std::initializer_list<lexer::Token::TokenType> delimiter, int local){
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
