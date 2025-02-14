
#include "literal.hpp"
#include "../symboltable.hpp"
#include "ast.hpp"
#include "../../lexer/lexer.hpp"
#include "../parser.hpp"
#include <string>
#include <regex>

IntLiteralAST::IntLiteralAST(int bits, std::string value, bool tsigned){
    this->bits  = bits;
    this->value = value;
    this->tsigned = tsigned;
}

std::string IntLiteralAST::emit_ll(int locc){
    return std::string("%") + std::to_string(locc) + " = i" + std::to_string(bits) + " " + value + "\n";
}

std::string IntLiteralAST::emit_cst(){
    return value;
}

IntLiteralAST* IntLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 1) return nullptr;
    std::regex r("u?int(8|16|32|64|128)");
    bool expected_int = std::regex_match(expected_type, r);
    if (expected_int){
        bool sig = expected_type[0] != 'u';
        //std:: cout << expected_type.substr(3+(!sig),expected_type.size()) << std::endl;
        int bits = std::stoi(expected_type.substr(3+(!sig),expected_type.size()));
        if (tokens[0].type == lexer::Token::TokenType::INT){
            return new IntLiteralAST(bits, tokens[0].value, sig);
        }
        else if (tokens[0].type == lexer::Token::TokenType::HEX){
            return new IntLiteralAST(bits, tokens[0].value, sig);
        }
        else if (tokens[0].type == lexer::Token::TokenType::SUB && tokens.size() == 2 && tokens[1].type == lexer::Token::TokenType::INT){
            if (sig) {
                return new IntLiteralAST(bits, std::string("-") + tokens[1].value, sig);
            }
            parser::error("Sign mismatch", tokens[0],tokens[1],std::string("Found a signed value (expected ") + expected_type + ")", 1234);
        }
        //else parser::error("Type mismatch", tokens[0],tokens[tokens.size()-1],std::string("expected a ") + expected_type, 1233);
    }

    return nullptr;
}



BoolLiteralAST::BoolLiteralAST(bool value){
    this->value = value;
}

std::string BoolLiteralAST::emit_ll(int locc){
    return std::string("%") + std::to_string(locc) + " = i1 " + std::to_string(value) + "\n";
}

std::string BoolLiteralAST::emit_cst(){
    return value? "true" : "false";
}
BoolLiteralAST* BoolLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() == 1){
        if (tokens[0].value == "true" || tokens[0].value == "false"){
            return new BoolLiteralAST(tokens[0].value == "true");
        }
    }
    return nullptr;
}


FloatLiteralAST::FloatLiteralAST(int bits, std::string value){
    this->bits  = bits;
    this->value = value;
}

std::string FloatLiteralAST::emit_ll(int locc){
    return std::string("%") + std::to_string(locc) + " = " + get_ll_type() + " " + value + "\n";
}

std::string FloatLiteralAST::emit_cst(){
    return value;
}

std::string FloatLiteralAST::get_ll_type(){
    if (bits < 32)       return "half";
    else if (bits < 64)  return "float";
    else if (bits < 128) return "double";
    else                 return "fp128";
}

FloatLiteralAST* FloatLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 1) return nullptr;
    std::regex r("float(16|32|64|128)");
    bool expected_float = std::regex_match(expected_type, r);
    if (expected_float){
        bool sig=false;
        int bits = std::stoi(expected_type.substr(5,expected_type.size()));
        if (tokens[0].type == lexer::Token::TokenType::SUB){
            sig = true;
            tokens = parser::subvector(tokens, 1,1,tokens.size());
        }
        if (tokens.size() < 2) return nullptr;
        if (tokens.size() > 3) return nullptr;
        if (tokens[0].type == lexer::Token::TokenType::ACCESS && tokens[1].type == lexer::Token::TokenType::INT){
            return new FloatLiteralAST(bits, (sig? std::string("-0.") : std::string("0.")) + tokens[1].value + "e00");
        }
        else if (tokens[0].type == lexer::Token::TokenType::INT && tokens[1].type == lexer::Token::TokenType::ACCESS){
            std::string val = (sig? std::string("-") : std::string("")) + tokens[0].value + ".";
            if (tokens.size() == 3){
                if (tokens[2].type == lexer::Token::TokenType::INT){
                    val += tokens[2].value;
                }
                else return nullptr;
            }
            val += "0e00";
            return new FloatLiteralAST(bits, val);
        }
    }
    return nullptr;
}