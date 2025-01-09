#include "parser.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <regex>
#include "../lexer/lexer.hpp"
#include "symboltable.hpp"
#include <stack>
#include <algorithm>
#include <initializer_list>

#define DEBUG

int parser::errorc = 0;

std::string parser::match_cpp_types(std::string name){
    if (name == "uint8"){return "uint8_t";}
    if (name == "int8") {return "int8_t";}
    if (name == "uint16"){return "uint16_t";}
    if (name == "int16") {return "int16_t";}
    if (name == "uint32"){return "uint32_t";}
    if (name == "int32") {return "int32_t";}
    if (name == "uint64"){return "uint64_t";}
    if (name == "int64") {return "int64_t";}
    if (name == "float32"){return "float";}
    if (name == "float64") {return "double";}
    if (name == "String") {return "std::string";}
    if (name == "Vector") {return "std::vector";}
    if (name == "List") {return "std::list";}
    if (name == "ptr") {return "std::shared_ptr";}
    if (name == "uptr") {return "std::unique_ptr";}
    if (name == "Map") {return "std::map";}
    if (name == "Tuple") {return "std::tuple";}

    return name;
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

void parser::error(std::string name, lexer::Token t, std::string msg, int code){
        std::cerr << std::string("\e[1;31mERROR: ") + name + "\e[0m at \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-1) + "\e[0m [E "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
        std::cerr << std::endl << " " << std::to_string(t.line) << " |\t" << t.line_content << std::endl;
        std::string pb = " ";
        
        for (int i=0; i<std::to_string(t.line).size(); i++) pb += " ";
        pb += " |\t";
        for (int i=0; i<t.column-2; i++){
            pb += " ";
        }
        pb += "\e[31m^";
        for (int i=1; i<t.value.size(); i++){
            pb += "^";
        }
        pb += "\e[0m";

        std::cerr << pb << std::endl << std::endl;

        std::exit(-1);
    }

void parser::error(std::string name, lexer::Token t, lexer::Token t2, std::string msg, int code){
        std::cerr << std::string("\e[1;31mERROR: ") + name + "\e[0m at + \e[1m" + t.filename + ":" + std::to_string(t.line) + ":" + std::to_string(t.column-1) + "\e[0m [E "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
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

        std::exit(-1);
    }

std::string match_token_clamp(lexer::Token::TokenType t){
    switch (t){
        case lexer::Token::TokenType::CLAMP_OPEN:  return "CLAMP";
        case lexer::Token::TokenType::CLAMP_CLOSE: return "CLAMP";

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
        if (s.size() == 0 && std::find(delimiter.begin(), delimiter.end(), t[i].type) != delimiter.end()) return i;
        else if (t[i].type == lexer::Token::TokenType::CLAMP_CLOSE || t[i].type == lexer::Token::TokenType::INDEX_CLOSE || t[i].type == lexer::Token::TokenType::BLOCK_CLOSE) {
            s.push(t[i]);
        }

        else if (t[i].type == lexer::Token::TokenType::CLAMP_OPEN){
            if(s.size() == 0) parser::error("Unclosed CLAMP", t[i], "This CLAMP was not closed" , 46);
            if(s.top().type != lexer::Token::TokenType::CLAMP_CLOSE) parser::error("Unopened " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not opened" , 47);
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
        if (s.size() == 0 && std::find(delimiter.begin(), delimiter.end(), t[i].type) != delimiter.end()) return i;
        else if (t[i].type == lexer::Token::TokenType::CLAMP_OPEN || t[i].type == lexer::Token::TokenType::INDEX_OPEN || t[i].type == lexer::Token::TokenType::BLOCK_OPEN) {
            s.push(t[i]);
        }

        else if (t[i].type == lexer::Token::TokenType::CLAMP_CLOSE){
            if(s.size() == 0) parser::error("Unopened CLAMP", t[i], "This CLAMP was not opened" , 46);
            if(s.top().type != lexer::Token::TokenType::CLAMP_OPEN) parser::error("Unclosed " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not closed" , 47);
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
    
    }
    for(int j=0; j<s.size(); j++){
        parser::error("Unclosed " + match_token_clamp(s.top().type), s.top(), "This " + match_token_clamp(s.top().type) + " was not closed" , 47);
        s.pop();
    }
    return i;
}

bool parser::type_eq(std::string a, std::string b){
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

template <typename T>
std::vector<T> subvector(std::vector<T> v, int start, int step, int stop){
    auto s = v.begin() + start; auto end = v.begin() + stop;
    step += 1;
    return std::vector<T>(s, end);
}

parser::AST* parser::parse_one_of_ast(std::vector<lexer::Token> tokens, std::vector<fsignal<AST*, std::vector<lexer::Token>, int, symbol::Namespace*>> functions, int local, symbol::Namespace* sr){
    for (auto fn : functions){
        AST* r = fn(tokens, local, sr);
        if (r != nullptr){
            #ifdef DEBUG
                std::cout << "parse_one_of: " << r->to_cpp()  << std::endl;
            #endif
            return r;
            break;
        }
    }
    return nullptr;
}

parser::TypeAST* parser::parse_one_of_type(std::vector<lexer::Token> tokens, std::vector<fsignal<TypeAST*, std::vector<lexer::Token>, int, symbol::Namespace*, std::string>> functions, int local, symbol::Namespace* sr, std::string expected_type){
    for (auto fn : functions){
        TypeAST* r = fn(tokens, local, sr, expected_type);
        if (r != nullptr){
            #ifdef DEBUG
                std::cout << "parse_one_of: " << r->to_cpp()  << std::endl;
            #endif
            return r;
            break;
        }
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_one_of(std::vector<lexer::Token> tokens, std::vector<fsignal<ExpressionAST*, std::vector<lexer::Token>, int, symbol::Namespace*, std::string>> functions, int local, symbol::Namespace* sr, std::string expected_type){
    for (auto fn : functions){
        ExpressionAST* r = fn(tokens, local, sr, expected_type);
        if (r != nullptr){
            #ifdef DEBUG
                std::cout << "parse_one_of: " << r->to_cpp()  << std::endl;
            #endif
            return r;
        }
    }
    return nullptr;
}

parser::TypeAST* parser::parse_type(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    TypeAST * a = parse_one_of_type(t, {parse_single_type, parse_pointer, parse_opt_type, parse_vec_type, parse_composite_type}, local, sr, expected_type);    
    if (a != nullptr){
        return a;
    }
    return nullptr;
};

parser::TypeAST* parser::parse_single_type(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() > 0 && t.size() < 2){
        if (t[0].type == lexer::Token::TokenType::ID){
            #ifdef DEBUG
                std::cout << "parse_single_type: " << t[0].value << std::endl;
            #endif
            return new TypeAST(t[0].value);
        }
    }
    return nullptr;
}

parser::TypeAST* parser::parse_multi_type(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    #ifdef DEBUG
        std::cout << "parse_multi_type" << std::endl;
    #endif
    int l = local; int i;
    for(i=0; i<t.size(); i++){
        if (l == local && t[i].type == lexer::Token::TokenType::COMMA) break;
        else if (t[i].type == lexer::Token::TokenType::LESS) l++;
        else if (t[i].type == lexer::Token::TokenType::GREATER) l--;
        else if (t[i].type == lexer::Token::TokenType::BITSHIFTR) l-=2;
        else if (t[i].type == lexer::Token::TokenType::ID) {}
        else if (t[i].type == lexer::Token::TokenType::QM) {}
        else if (t[i].type == lexer::Token::TokenType::MUL) {}
        else if (t[i].type == lexer::Token::TokenType::INDEX_OPEN) {}
        else if (t[i].type == lexer::Token::TokenType::INDEX_CLOSE) {}
        else if (t[i].type == lexer::Token::TokenType::COMMA) {}
        else if (t[i].type == lexer::Token::TokenType::INT) {}
        else parser::error("Unexpected Symbol", t[i], std::string("Unexpected symbol ('") + t[i].value + "') in Type declaration", 5);
    }
    if (i != t.size()){
        TypeAST* t0 = parse_type(subvector(t, 0, 1, i), local, sr);
        if(t0 == nullptr) parser::error("Missing Type", t[i], "Expected type before comma", 230);
        MultiTypeAST* a = new MultiTypeAST();
        a->c = t0;
        
        a->t = parse_one_of_type(subvector(t, i+1, 1, t.size()), {parse_type, parse_multi_type} , local, sr);
        if(a->t == nullptr) parser::error("Missing Type", t[i], "Expected type after comma", 231);
        return a;
    }
    return nullptr;
}

parser::TypeAST* parser::parse_composite_type(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() >= 4){  
        if (t[t.size() - 1].type == lexer::Token::TokenType::GREATER && t[1].type == lexer::Token::TokenType::LESS && t[0].type == lexer::Token::TokenType::ID){
            if (t[0].value == "rptr"){
                PointerTypeAST* a = new PointerTypeAST("*");
                a->t = parse_type(subvector(t, 2, 1, t.size()-1), local, sr);
                if (a->t != nullptr){
                    return a;
                }
                parser::error("Missing Type", t[1], "Expected 1 type in rptr", 232);
            }
            #ifdef DEBUG
                std::cout << "parse_composite_type" << std::endl;
            #endif
            ComposeTypeAST* a = new ComposeTypeAST(t[0].value);
            a->t = parse_one_of_type(subvector(t, 2, 1, t.size()-1), {parse_type, parse_multi_type} , local, sr);
            if (a->t != nullptr){
                return a;
            }
        }
    }
    return nullptr;
};

parser::TypeAST* parser::parse_pointer(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() > 1){
        if (t[t.size() - 1].type == lexer::Token::TokenType::MUL){
            #ifdef DEBUG
                std::cout << "parse_pointer" << std::endl;
            #endif
            ComposeTypeAST* a = new ComposeTypeAST("ptr");
            a->t = parse_type(subvector(t, 0, 1, t.size()-1), local, sr);
            if (a->t != nullptr){
                return a;
            }
        }
    }
    return nullptr;
}

parser::TypeAST* parser::parse_opt_type(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() > 1){
        if (t[t.size()-1].type == lexer::Token::TokenType::QM){
            #ifdef DEBUG
                std::cout << "parse_opt_type" << std::endl;
            #endif
            OptionalTypeAST* a = new OptionalTypeAST("Optional");
            a->t = parse_type(subvector(t, 0, 0, t.size()-1), local, sr);
            if (a->t != nullptr){
                return a;
            }
        }
    }
    return nullptr;
}

parser::TypeAST* parser::parse_vec_type(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() > 2){
        if (t[t.size()-2].type == lexer::Token::TokenType::INDEX_OPEN && t[t.size()-1].type == lexer::Token::TokenType::INDEX_CLOSE){
            #ifdef DEBUG
                std::cout << "parse_vec_type" << std::endl;
            #endif
            ComposeTypeAST* a = new ComposeTypeAST("Vector");
            a->t = parse_type(subvector(t, 0, 1, t.size()-2), local, sr);
            if (a->t != nullptr){
                return a;
            }
        }
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_math(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    ExpressionAST * a = parse_one_of(t, {
        parse_number,
        parse_var_call,
        parse_func_call,
        parse_bool,
        parse_lor,
        parse_land,
        parse_bor,
        parse_bxor,
        parse_band,
        parse_add,
        parse_mul,
        parse_pow,
        parse_lnot,
        parse_bnot,
        parse_cast,
        parse_clamp
    }, local, sr, expected_type);    
    if (a != nullptr){
        return a;
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_number(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 1){
        #ifdef DEBUG
            std::cout << "Literal?" << std::endl;
        #endif
        if (t[0].type == lexer::Token::TokenType::INT){
            return new IntLiteralAST(t[0].value, "@int");
        }
        if (t[0].type == lexer::Token::TokenType::HEX){
            return new IntLiteralAST(t[0].value, "@int");
        }
        if (t[0].type == lexer::Token::TokenType::BINARY){
            return new IntLiteralAST(t[0].value, "@int");
        }
        if (t[0].type == lexer::Token::TokenType::FLOAT){
            return new FloatLiteralAST(t[0].value, "@float");
        }
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_var_call(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() > 0){
        std::string name = "";

        #ifdef DEBUG
            std::cout << "Name: " << name << std::endl;
        #endif

        int i = 0;
        i = parser::get_id(t,name);

        if(i > 0 && i == t.size()){
            #ifdef DEBUG
                std::cout << "VARCALL: " << name << std::endl;
            #endif
            if(sr->find(name) == "") parser::error("Undefined Variable", t[0], t[t.size()-1], "No variable of this name was found in this context", 786);

            parser::VarCallAST* va = new parser::VarCallAST(name, sr->find(name));
            return va;
        }
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_bool(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 1){
        if (t[0].type == lexer::Token::TokenType::BOOL){
            return new BoolLiteralAST(t[0].value, "bool");
        }
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_cast(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    if(t[0].type == lexer::Token::TokenType::AS) parser::error("Missing Expression", t[0], "Expected expression left of 'as'", 167);
    if(t[t.size()-1].type == lexer::Token::TokenType::AS) parser::error("Missing Type", t[t.size()-1], "Expected type right of 'as'", 312);

    int i = parser::splitStack(t, {lexer::Token::TokenType::AS}, local);

    #ifdef DEBUG
        std::cout << "parse_cast : " << i <<"\n";
    #endif
    //i = 1;
    if (i != t.size() && i > 0){
        
        TypeCastAST* a = new TypeCastAST();
        
        
        a->type_ast = parse_type(subvector(t, i+1, 1, t.size()), local, sr);
        if(a->type_ast == nullptr) parser::error("Missing Type", t[i+1], t[t.size()-1], "Expected type right of 'as'", 312);

        a->updateType();

        ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, a->type_ast->get_type());
        if(t0 == nullptr) parser::error("Missing Expression", t[0], t[i], "Expected expression left of 'as'", 167);
        a->expr = t0;
        
        return a;
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_add(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    int i = splitStack(t, {lexer::Token::TokenType::ADD, lexer::Token::TokenType::SUB}, local);
    
    if (i != t.size() && i > 0){
        #ifdef DEBUG
            std::cout << "parse ADD/SUB : " << i << "/" << t.size() << std::endl;
        #endif
        if(t[i].type == lexer::Token::TokenType::ADD){
            ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, expected_type);
            if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '+'", 167);
            AddAST* a = new AddAST();
            a->left = t0;
            
            a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr, a->left->type);
            if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '+'", 168);
            
            if(!type_eq(a->left->type, a->right->type)) parser::error("Unmatching Types", t[i],
                "You may only add values of the same kind (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
                
            a->type = a->left->type[0] == '@' ? a->right->type : a->left->type;
            
            return a;
        }
        if(t[i].type == lexer::Token::TokenType::SUB){
            ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, expected_type);
            if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '-'", 167);
            SubAST* a = new SubAST();
            a->left = t0;
            
            a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr, a->left->type);
            if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '-'", 168);
            
            if(!type_eq(a->left->type, a->right->type)) parser::error("Unmatching Types", t[i],
                "You may only subtract values of the same kind (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
            a->type = a->left->type[0] == '@' ? a->right->type : a->left->type;
            
            return a;
        }
    }
    
    return nullptr;
}

parser::ExpressionAST* parser::parse_mul(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    int i = splitStack(t, {lexer::Token::TokenType::MUL, lexer::Token::TokenType::DIV, lexer::Token::TokenType::MOD}, local);

    if (i != t.size() && i > 0){
        #ifdef DEBUG
            std::cout << "parse MUL/DIV : " << i << "/" << t.size() << std::endl;
        #endif
        if(t[i].type == lexer::Token::TokenType::MUL){
            ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, expected_type);
            if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '*'", 187);
            MulAST* a = new MulAST();
            a->left = t0;
            
            a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr, a->left->type);
            if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '*'", 188);
            
            if(!type_eq(a->left->type, a->right->type)) parser::error("Unmatching Types", t[i],
                "You may only multiply values of the same kind (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
                
            a->type = a->left->type[0] == '@' ? a->right->type : a->left->type;
            
            return a;
        }
        if(t[i].type == lexer::Token::TokenType::DIV){
            ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, expected_type);
            if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '/'", 197);
            DivAST* a = new DivAST();
            a->left = t0;
            
            a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr, a->left->type);
            if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '/'", 198);
            
            if(!type_eq(a->left->type, a->right->type)) parser::error("Unmatching Types", t[i],
                "You may only divide values of the same kind (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
            a->type = a->left->type[0] == '@' ? a->right->type : a->left->type;
            
            return a;
        }
        if(t[i].type == lexer::Token::TokenType::MOD){
            ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, expected_type);
            if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '%'", 317);
            ModAST* a = new ModAST();
            a->left = t0;
            
            a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr, a->left->type);
            if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '%'", 318);
            
            if(!(type_eq(a->right->type, "@uint") && (type_eq(a->left->type, "@int") || type_eq(a->left->type, "@float")))) parser::error("Unmatching Types", t[i],
                "Modulo takes one number and one unsigned integer as input (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
            a->type = a->right->type;
            
            return a;
        }
    }
    
    return nullptr;
}

parser::ExpressionAST* parser::parse_pow(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    int i = parser::splitStack(t, {lexer::Token::TokenType::POW}, local);
    
    if (i != t.size() && i > 0){
        #ifdef DEBUG
            std::cout << "parse POW : " << i << "/" << t.size() << std::endl;
        #endif
        ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, expected_type);
        if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '**'", 197);
        PowAST* a = new PowAST();
        a->left = t0;
        
        a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr, a->left->type);
        if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '**'", 198);
        
        if(!type_eq(a->left->type, a->right->type)) parser::error("Unmatching Types", t[i],
            "You may only power values of the same kind (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
        a->type = a->left->type[0] == '@' ? a->right->type : a->left->type;
        
        return a;
    }
    
    return nullptr;
}

parser::ExpressionAST* parser::parse_clamp(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t[0].type == lexer::Token::TokenType::CLAMP_OPEN){
        if (t[t.size()-1].type == lexer::Token::TokenType::CLAMP_CLOSE){
            #ifdef DEBUG
                std::cout << "parse CLAMP" << std::endl;
            #endif
            ClampAST* a = new ClampAST();
            if (subvector(t, 1,0,t.size()-1).size() == 0) parser::error("Missing Expression", t[0], "This clamp has no expression.", 576);
            a->expr = parse_math(subvector(t, 1,0,t.size()-1), local, sr, expected_type);
            if (a->expr == nullptr) return nullptr;
            a->type = a->expr->type;
            #ifdef DEBUG
                std::cout << "end CLAMP" << std::endl;
            #endif

            return a;
        }
        parser::error("Unclosed Clamp", t[0], "This clamp is unclosed.", 15);
    }
    if (t[t.size()-1].type == lexer::Token::TokenType::CLAMP_CLOSE){
        parser::error("Unopened Clamp", t[0], "This clamp is unopened.", 16);
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_bnot(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t[0].type == lexer::Token::TokenType::BNOT){
        #ifdef DEBUG
            std::cout << "parse BNOT" << std::endl;
        #endif
        BNotAST* a = new BNotAST();
        if (subvector(t, 1,0,t.size()).size() == 0) parser::error("Missing Expression", t[0], "Expected expression right of '~'", 218);
        a->expr = parse_math(subvector(t, 1,0,t.size()), local, sr, expected_type);
        if (a->expr == nullptr) parser::error("Missing Expression", t[0], "Expected expression right of '~'", 218);
        a->type = a->expr->type;

        return a;
    }
    return nullptr;
}

parser::ExpressionAST* parser::parse_band(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    int i = parser::splitStack(t, {lexer::Token::TokenType::BAND}, local);
    
    if (i != t.size() && i > 0){
        #ifdef DEBUG
            std::cout << "parse BAND : " << i << "/" << t.size() << std::endl;
        #endif
        ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, expected_type);
        if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '&'", 327);
        BAndAST* a = new BAndAST();
        a->left = t0;
        
        a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr, a->left->type);
        if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '&'", 328);
        
        if(!type_eq(a->left->type, a->right->type)) parser::error("Mismatching Types", t[i],
            "You may only AND values of the same kind (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
        a->type = a->left->type[0] == '@' ? a->right->type : a->left->type;
        
        return a;
    }
    
    return nullptr;
}

parser::ExpressionAST* parser::parse_bor(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    int i = parser::splitStack(t, {lexer::Token::TokenType::BOR}, local);
    
    if (i != t.size() && i > 0){
        #ifdef DEBUG
            std::cout << "parse BOR : " << i << "/" << t.size() << std::endl;
        #endif
        ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr, expected_type);
        if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '|'", 327);
        BOrAST* a = new BOrAST();
        a->left = t0;
        
        a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr, a->left->type);
        if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '|'", 328);
        
        if(!type_eq(a->left->type, a->right->type)) parser::error("Mismatching Types", t[i],
            "You may only OR values of the same kind (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
        a->type = a->left->type[0] == '@' ? a->right->type : a->left->type;
        
        return a;
    }
    
    return nullptr;
}

parser::ExpressionAST* parser::parse_bxor(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    int i = parser::splitStack(t, {lexer::Token::TokenType::BXOR}, local);
    
    if (i != t.size() && i > 0){
        #ifdef DEBUG
            std::cout << "parse BXOR : " << i << "/" << t.size() << std::endl;
        #endif
        ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr);
        if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '^'", 327);
        BXorAST* a = new BXorAST();
        a->left = t0;
        
        a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr);
        if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '^'", 328);
        
        if(!type_eq(a->left->type, a->right->type)) parser::error("Mismatching Types", t[i],
            "You may only XOR values of the same kind (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
        a->type = a->left->type[0] == '@' ? a->right->type : a->left->type;
        
        return a;
    }
    
    return nullptr;
}

parser::ExpressionAST* parser::parse_lor(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    int i = parser::splitStack(t, {lexer::Token::TokenType::OR}, local);
    
    if (i != t.size() && i > 0){
        #ifdef DEBUG
            std::cout << "parse LOR : " << i << "/" << t.size() << std::endl;
        #endif
        ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr);
        if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '||'", 327);
        LorAST* a = new LorAST();
        a->left = t0;
        
        a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr);
        if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '||'", 328);
        
        if(!type_eq(a->left->type, "bool") || !type_eq(a->right->type, "bool")) parser::error("Mismatching Types", t[i],
            "You may only do OR on two bools (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
        a->type = "bool";
        
        return a;
    }
    
    return nullptr;
}

parser::ExpressionAST* parser::parse_land(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t.size() == 0){return nullptr;}
    int i = parser::splitStack(t, {lexer::Token::TokenType::AND}, local);
    
    if (i != t.size() && i > 0){
        #ifdef DEBUG
            std::cout << "parse LAND : " << i << "/" << t.size() << std::endl;
        #endif
        ExpressionAST* t0 = parse_math(subvector(t, 0, 1, i), local, sr);
        if(t0 == nullptr) parser::error("Missing Expression", t[i], "Expected expression left of '&&'", 327);
        LandAST* a = new LandAST();
        a->left = t0;
        
        a->right = parse_math(subvector(t, i+1, 1, t.size()), local, sr);
        if(a->right == nullptr) parser::error("Missing Expression", t[i], "Expected expression right of '&&'", 328);
        
        if(!type_eq(a->left->type, "bool") || !type_eq(a->right->type, "bool")) parser::error("Mismatching Types", t[i],
            "You may only do AND on two bools (found '" + a->left->type + "' and '" + a->right->type + "')", 11);
            
        a->type = "bool";
        return a;
    }
    
    return nullptr;
}

parser::ExpressionAST* parser::parse_lnot(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if (t[0].type == lexer::Token::TokenType::NOT){
        #ifdef DEBUG
            std::cout << "parse LNOT" << std::endl;
        #endif
        BNotAST* a = new BNotAST();
        if (subvector(t, 1,0,t.size()).size() == 0) parser::error("Missing Expression", t[0], "Expected expression right of '!'", 218);
        a->expr = parse_math(subvector(t, 1,0,t.size()), local, sr);
        if (a->expr == nullptr) parser::error("Missing Expression", t[0], "Expected expression right of '!'", 218);
        if (!type_eq(a->expr->type, "bool")) parser::error("Mismatching Types", t[0], "NOT can only only be used on a bool (found " + a->expr->type + ")", 218);
        a->type = a->expr->type;

        return a;
    }
    return nullptr;
}

bool parser::is_pascalCase(std::string text){
    const std::regex rx("\\_?[a-z][a-zA-Z0-9]*");
    std::cmatch m;
    return std::regex_match(text.c_str(), m, rx);
}

bool parser::is_snake_case(std::string text){
    const std::regex rx("[a-z\\_][a-z0-9\\_]*");
    std::cmatch m;
    return std::regex_match(text.c_str(), m, rx);
}

bool parser::is_CamelCase(std::string text){
    const std::regex rx("\\_?[A-Z][a-zA-Z0-9]*");
    std::cmatch m;
    return std::regex_match(text.c_str(), m, rx);
}

bool parser::is_UPPER_CASE(std::string text){
    const std::regex rx("[A-Z\\_][A-Z0-9\\_]*");
    std::cmatch m;
    return std::regex_match(text.c_str(), m, rx);
}

parser::AST* parser::parse_var_declare(std::vector<lexer::Token> t, int local, symbol::Namespace* sr){
    if(t.size() < 3) return nullptr;
    if(t[t.size()-2].type == lexer::Token::TokenType::ID){
        if(t[t.size()-1].type == lexer::Token::TokenType::END_CMD){
            parser::TypeAST* ta = parser::parse_type(subvector(t, 0,1,t.size()-2), local, sr);
            if (ta == nullptr) return nullptr;

            parser::DeclareVariableAST* a = new parser::DeclareVariableAST();
            a->name = t[t.size()-2].value;
            a->type = ta;

            if(!parser::is_snake_case(a->name)){
                parser::warn("Wrong Casing", t[t.size()-2], std::string("Name '") + a->name + "' should be snake_case", 34);
            }

            if(sr->find(a->name) != "") parser::error("Variable redefinition", t[t.size()-2], "A Variable with the name '" + a->name + "' of type " + sr->find(a->name) + " is already known in this kontext. ", 2380);
                sr->add(a->name, new symbol::Var(a->name,a->type->get_type()));

            return a;
        }
    }

    return nullptr;
}

parser::AST* parser::parse_var_init(std::vector<lexer::Token> t, int local, symbol::Namespace* sr){
    if(t.size() < 5) return nullptr;
    int i, l = local;
    for(i=0; i<=t.size() ; i++){
        if (l == local && t[i].type == lexer::Token::TokenType::SET) break;
    }
    if(i < 1) {parser::error("Type Expected", t[0], "Expected a type or name before '='", 5498); return new parser::StatementAST;}
    if(i <= t.size()-1 && i > 1){
        if(t[i-1].type == lexer::Token::TokenType::ID){
            if (t[t.size()-1].type == lexer::Token::TokenType::END_CMD){
                parser::TypeAST* ta = parser::parse_type(subvector(t, 0,1,i-1), local, sr);
                if (ta == nullptr) return nullptr;

                parser::ExpressionAST* ea = parser::parse_math(subvector(t, i+1,1,t.size()-1), local, sr);
                if (ea == nullptr)  {parser::error("Expression expected", t[i], "An expression is required after '='", 5497); return new parser::StatementAST;}

                parser::InitVariableAST* a = new parser::InitVariableAST();
                a->name = t[i-1].value;
                a->type = ta;
                a->value = ea;

                if(!type_eq(ea->type, ta->get_type())) parser::error("Mismatching types", t[0], t[i-2], "You are trying to set a variable of type " + a->type->get_type() + " to a value of type " + ea->type, 9340);

                if(!parser::is_snake_case(a->name)){
                    parser::warn("Wrong Casing", t[i-1], std::string("Name '") + a->name + "' should be snake_case", 34);
                }

                if(sr->find(a->name) != "") parser::error("Variable redefinition", t[i-1], "A Variable with the name '" + a->name + "' of type " + sr->find(a->name) + " is already known in this kontext. ", 2380);
                sr->add(a->name, new symbol::Var(a->name,a->type->get_type()));

                return a;
            }
            else {parser::error("Missing Semicolon", t[t.size()-1], "A Semicolon is missing after this command", 5499); return new parser::StatementAST;}
        }
        else {parser::error("Name Expected", t[i], "Expected a name before '='", 5499); return new parser::StatementAST;}
    }

    return nullptr;
}

parser::AST* parser::parse_var_set(std::vector<lexer::Token> t, int local, symbol::Namespace* sr){
    if (t.size() < 3) return nullptr;
    if (t[0].type == lexer::Token::TokenType::ID){
        if (t[1].type == lexer::Token::TokenType::SET){
            if(t[t.size()-1].type != lexer::Token::TokenType::END_CMD) {parser::error("Missing Semicolon", t[t.size()-1], "A Semicolon is missing after this command", 5499); return new parser::StatementAST;}
            if(sr->find(t[0].value) == "") parser::error("Undefined Variable", t[0], "No variable of this name was not found in this context", 786);

            parser::ExpressionAST* a = parser::parse_math(subvector(t, 2,1,t.size()-1), local, sr);
            if (a == nullptr) parser::error("Expression expected", t[1], "Expression was expected right of '='.",790);
            if (!type_eq(a->type,sr->find(t[0].value))) parser::error("Mismatching types", t[2], t[t.size()-1], "You are trying to set a variable of type " + sr->find(t[0].value) + " to a value of type " + a->type,790);

            parser::SetVariableAST* va = new parser::SetVariableAST();
            va->name  = t[0].value;
            va->value = a;
            return va;
        }
    }

    return nullptr;
}

parser::ExpressionAST* parser::parse_func_call(std::vector<lexer::Token> t, int local, symbol::Namespace* sr, std::string expected_type){
    if(t.size() < 3) return nullptr;
    std::string name = "";
    int x = parser::get_id(t,name);
    if(x > 0){
        #ifdef DEBUG
            std::cout << "Func Name: " << name << std::endl;
        #endif
        auto t2 = t; t = subvector(t, x,1,t.size());
        if(t[0].type == lexer::Token::TokenType::CLAMP_OPEN){
            if(t[t.size()-1].type == lexer::Token::TokenType::CLAMP_CLOSE){
                if(t.size()==2) return new parser::FuncCallAST(name, std::vector<parser::ExpressionAST*>({}));

                std::vector<parser::ExpressionAST*> params = {};
                int i = 0; int last;
                do {
                    last = i+1;
                    i = parser::splitStack(subvector(t, last,1,t.size()-1), {lexer::Token::TokenType::COMMA}, local);
                    if(i < t.size()-1){
                        parser::ExpressionAST* a = parser::parse_math(subvector(t, last,1,i), local, sr);
                        if(a == nullptr) parser::error("Expression expected", t[last], t[i], "A valid expression was expected as function parameter", 1212);
                        params.push_back(a);
                    }
                } while(i < t.size()-1);
                parser::ExpressionAST* a = parser::parse_math(subvector(t, last,1,t.size()-1), local, sr);
                if(a == nullptr) parser::error("Expression expected", t[last], t[i], "A valid expression was expected as function parameter", 1212);
                params.push_back(a);

                
                symbol::Func* fn = parser::get_best_fn(name, params, sr, expected_type);
                if(fn == nullptr) parser::error("Unknown Function", t[0], "A function named '" + name + "' is not known in this kontext", 6765);

                for(int j=0; j<params.size(); j++){
                    if(!type_eq(params[j]->type,fn->params[j])) parser::error("Mismatching types", t2[1], "The type returned ('" + params[j]->type + "') does not match the expected type of param " + std::to_string(j) + " ('" + fn->params[j] + "')", 4505);
                }

                auto r = new parser::FuncCallAST(name, params);
                r->type = fn->type;

                return r;
            }
            else parser::error("Unclosed Clamp", t[1], "This clamp (function call) was never closed ", 4505);
        }
    }

    return nullptr;
}

parser::AST* parser::parse_expr(std::vector<lexer::Token> t, int local, symbol::Namespace* sr){
    if(t.size() == 0) return nullptr;
    if(t[t.size()-1].type == lexer::Token::TokenType::END_CMD){
        if(t.size() > 1){
            parser::ExpressionAST* a = parse_math(subvector(t, 0,1,t.size()-1), local, sr, "@unknown");
            if(a == nullptr) parser::error("Expected statement or expression", t[0], t[t.size()-1], "", 3);
            return new parser::ExprStatementAST(a);
        }
        return new parser::StatementAST();
    }
    return nullptr;
}

parser::AST* parser::parse_import(std::vector<lexer::Token> t, int local, symbol::Namespace* sr){
    if(t.size() == 0) return nullptr;
    if(t[0].type == lexer::Token::TokenType::IMPORT){
        if(t[t.size()-1].type == lexer::Token::TokenType::END_CMD){
            return new parser::StatementAST();
        }
        else parser::error("Expected semicolon", t[t.size()-1], "Expected an END_CMD (;) token", 4505);
        return new parser::StatementAST();
    }
    return nullptr;
}

int parser::get_id(std::vector<lexer::Token> t, std::string& s){
    int i=0;
    std::string out = "";
    if(t[0].type != lexer::Token::TokenType::ID) return -1;
    lexer::Token::TokenType last = lexer::Token::SUBNS;
    for (i=0; i<t.size(); i++){
        //std::cout << i << " " << t[i].value << std::endl;
        if     (t[i].type == lexer::Token::TokenType::ID && last == lexer::Token::SUBNS) {out += t[i].value; last = t[i].type;}
        else if(t[i].type == lexer::Token::TokenType::SUBNS && last == lexer::Token::ID) {out += t[i].value; last = t[i].type;}
        else break;
    }
    s = out;
    return i;
}

parser::AST* parser::parse(std::vector<lexer::Token> t, int local, symbol::Namespace* sr){

    std::vector<lexer::Token> buffer = {};
    
    int i = 0;
    Block* block = new parser::Block;
    while (i < t.size()){
        buffer.push_back(t[i]);
        if (t[i].type == lexer::Token::TokenType::END_CMD && (i == 0 || t[0].type != lexer::Token::TokenType::BLOCK_OPEN) || t[i].type == lexer::Token::TokenType::BLOCK_CLOSE){

            AST* a = parse_one_of_ast(buffer, {
                parser::parse_subblock,
                parse_import,
                parse_var_declare,
                parse_var_set,
                parse_var_init,
                parse_expr,
            }, local, sr);

            block->statements.push_back(a);

            buffer = {};
        }

        i++;
    }

    return block;
}

parser::AST* parser::parse_subblock(std::vector<lexer::Token> t, int local, symbol::Namespace* sr){

    if (t.size() == 0) return nullptr;
    if(t[0].type == lexer::Token::TokenType::BLOCK_OPEN){
        if(t[t.size()-1].type == lexer::Token::TokenType::BLOCK_CLOSE){
            #ifdef DEBUG
                std::cout << "SUBBLOCK START" << std::endl;
            #endif
            t = subvector(t, 1,1,t.size()-1);
            std::vector<lexer::Token> buffer = {};
            symbol::SubBlock* sb = new symbol::SubBlock(sr);
            
            int i = 0;
            Block* block = new parser::Block;
            while (i < t.size()){
                buffer.push_back(t[i]);
                if (t[i].type == lexer::Token::TokenType::END_CMD && (i == 0 || t[0].type != lexer::Token::TokenType::BLOCK_OPEN) || t[i].type == lexer::Token::TokenType::BLOCK_CLOSE){

                    AST* a = parse_one_of_ast(buffer, {
                        parser::parse_subblock,
                        parse_import,
                        parse_var_declare,
                        parse_var_set,
                        parse_var_init,
                        parse_expr
                    }, local, sb);

                    block->statements.push_back(a);

                    buffer = {};
                }

                i++;
            }
            #ifdef DEBUG
                std::cout << "SUBBLOCK END" << std::endl;
            #endif
            return block;
        }
    }
    return nullptr;
}

symbol::Func* parser::get_best_fn(std::string name, std::vector<parser::ExpressionAST*> params, symbol::SymbolReference* sr, std::string expected_type){

    symbol::Func* best_match = nullptr;
    int best_score = -512;
    for(symbol::Func* fn : sr->find_fn(name)){
        int score = -abs(fn->params.size() - params.size());
        if(!type_eq(fn->type,expected_type)) score -= 2;
        #ifdef DEBUG
            std::string p = "(";
        #endif
        for(int i=0; i<std::min(fn->params.size(), params.size()); i++){
            score -= int(!type_eq(params[i]->type, fn->params[i]));
            #ifdef DEBUG
                p += fn->params[i];
            #endif
        }
        if(score > best_score){
            best_score = score;
            best_match = fn;
        }
        #ifdef DEBUG
            std::cout << fn->type << " " << name << p << ")" << " : " << score << std::endl;
        #endif
    }

    if(best_match == nullptr){
        // TODO: error here
    }
    return best_match;
}






