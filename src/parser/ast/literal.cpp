
#include "literal.hpp"
#include "../symboltable.hpp"
#include "ast.hpp"
#include "../../lexer/lexer.hpp"
#include "../parser.hpp"
#include <string>
#include <regex>
#include <iostream>

IntLiteralAST::IntLiteralAST(int bits, std::string value, bool tsigned, std::vector<lexer::Token> tokens){
    this->bits  = bits;
    this->value = value;
    this->tsigned = tsigned;
    this->tokens  = tokens;
}

std::string IntLiteralAST::emit_ll(int locc){
    return std::string("%") + std::to_string(locc) + " = i" + std::to_string(bits) + " " + value + "\n";
}

std::string IntLiteralAST::emit_cst(){
    return value;
}

AST* IntLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 1 || tokens.size() > 2) return nullptr;
        if (tokens[0].type == lexer::Token::TokenType::INT){
            return new IntLiteralAST(32, tokens[0].value, false, tokens);
        }
        else if (tokens[0].type == lexer::Token::TokenType::HEX){
            return new IntLiteralAST(32, tokens[0].value, false, tokens);
        }
        else if ((tokens[0].type == lexer::Token::TokenType::SUB || tokens[0].type == lexer::Token::TokenType::NEC) && tokens.size() == 2 && tokens[1].type == lexer::Token::TokenType::INT){
            return new IntLiteralAST(32, std::string("-") + tokens[1].value, true, tokens);
        }
    return nullptr;
}

//llvm::Value* IntLiteralAST::codegen(){
//    return llvm::ConstantInt::get(*context, llvm::APInt(bits, (uint64_t) std::stoll(value), std::stoll(value) < 0));
//}

void IntLiteralAST::force_type(std::string type){
    std::regex r("u?int(8|16|32|64|128)");
    bool expected_int = std::regex_match(type, r);
    if (expected_int){
        bool sig = type[0] != 'u';
        int bits = std::stoi(type.substr(3+(!sig),type.size()));

        if(value[0] == '-' && !sig) parser::error("Sign mismatch", tokens[0],tokens[1],std::string("Found a signed value (expected \e[1m") + type + "\e[0m)", 45);
        tsigned = sig;
        this->bits = bits;

    }
    else parser::error("Type mismatch", tokens[0],tokens[tokens.size()-1],std::string("expected a \e[1m") + type + "\e[0m, found int", 17, "Caused by");
}



BoolLiteralAST::BoolLiteralAST(bool value, std::vector<lexer::Token> tokens){
    this->value = value;
    this->tokens = tokens;
}

std::string BoolLiteralAST::emit_ll(int locc){
    return std::string("%") + std::to_string(locc) + " = i1 " + std::to_string(value) + "\n";
}

std::string BoolLiteralAST::emit_cst(){
    return value? "true" : "false";
}
AST* BoolLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() == 1){
        if (tokens[0].value == "true" || tokens[0].value == "false"){
            return new BoolLiteralAST(tokens[0].value == "true", tokens);
        }
    }
    return nullptr;
}
void BoolLiteralAST::force_type(std::string type){
    if (type != "bool") parser::error("Type mismatch", tokens[0] ,std::string("expected a \e[1m") + type + "\e[0m, found bool", 17, "Caused by");
}


FloatLiteralAST::FloatLiteralAST(int bits, std::string value, std::vector<lexer::Token> tokens){
    this->bits  = bits;
    this->value = value;
    this->tokens = tokens;
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

AST* FloatLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() < 1) return nullptr;
    bool sig=false;
    auto t = tokens;
    if (tokens[0].type == lexer::Token::TokenType::SUB || tokens[0].type == lexer::Token::TokenType::NEC){
        sig = true;
        tokens = parser::subvector(tokens, 1,1,tokens.size());
    }
    if (tokens.size() < 2) return nullptr;
    if (tokens.size() > 3) return nullptr;
    if (tokens[0].type == lexer::Token::TokenType::ACCESS && tokens[1].type == lexer::Token::TokenType::INT){
        return new FloatLiteralAST(32, (sig? std::string("-0.") : std::string("0.")) + tokens[1].value + "e00", t);
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
        return new FloatLiteralAST(32, val, t);
    }
    return nullptr;
}

void FloatLiteralAST::force_type(std::string type){
    std::regex r("float(16|32|64|128)");
    bool expected_float = std::regex_match(type, r);
    if (expected_float){
        int bits = std::stoi(type.substr(5,type.size()));
        this->bits = bits;
    }
    else parser::error("Type mismatch", tokens[0], tokens[tokens.size()-1] ,std::string("expected a \e[1m") + type + "\e[0m, found float", 17, "Caused by");
}

CharLiteralAST::CharLiteralAST(std::string value, std::vector<lexer::Token> tokens){
    this->value = value;
    this->tokens = tokens;
}

AST* CharLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() != 1) return nullptr;
    if (tokens[0].type == lexer::Token::TokenType::CHAR){
        if (tokens[0].value.size() == 2){
            parser::error("Empty char", tokens[0], "This char value is empty. This is not supported. Did you mean '\\u0000' ?", 578);
            return new AST();
        }
        std::regex r ("'\\\\u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]'");
        std::regex r2 ("'\\\\(n|a|r|t|f|v|\\\\|'|\"|)'");
        if (std::regex_match(tokens[0].value, r) || std::regex_match(tokens[0].value, r2) || tokens[0].value.size() == 3){
            //std::cout<<"skdskdl"<<std::endl;
            return new CharLiteralAST(tokens[0].value, tokens);
        }
        parser::error("Invalid char", tokens[0], "This char value is not supported. Chars are meant to hold only one character. Did you mean to use \"Double quotes\" ?", 579);
        return new AST();
    }
    return nullptr;
}

std::string CharLiteralAST::get_value(){
    if (this->value == "'\\n'") return "\"\\00\\0A\"";
    if (this->value == "'\\t'") return "\"\\00\\09\"";
    if (this->value == "'\\v'") return "\"\\00\\0B\"";
    if (this->value == "'\\f'") return "\"\\00\\0C\"";
    if (this->value == "'\\r'") return "\"\\00\\0D\"";
    if (this->value == "'\\a'") return "\"\\00\\07\"";
    if (this->value == "'\\\"'") return "\"\\00\\22\"";
    if (this->value == "'\\\\'") return "\"\\00\\5C\"";
    if (this->value == "'\\\''") return "\"\\00\\27\"";

    if (this->value[1] == '\\' && this->value.size() == 8){
        return std::string("\"\\") + this->value.substr(3, 2) + "\\" + this->value.substr(5, 2) + "\"";
    }

    return std::string("\"") + this->value[1] + "\"";
}

void CharLiteralAST::force_type(std::string type){
    if (type != "char"){
        parser::error("Type mismatch", tokens[0], tokens[tokens.size()-1] ,std::string("expected a \e[1m") + type + "\e[0m, found char", 17, "Caused by");
    }
}

StringLiteralAST::StringLiteralAST(std::string value, std::vector<lexer::Token> tokens){
    this->value = value;
    this->tokens = tokens;
}

AST* StringLiteralAST::parse(std::vector<lexer::Token> tokens, int local, symbol::Namespace* sr, std::string expected_type){
    if (tokens.size() != 1) return nullptr;
    if (tokens[0].type == lexer::Token::TokenType::STRING){
        return new StringLiteralAST(tokens[0].value, tokens);
    }
    return nullptr;
}

std::string StringLiteralAST::get_value(){
    /*if (this->value == "'\\n'") return "\"\\00\\0A\"";
    if (this->value == "'\\t'") return "\"\\00\\09\"";
    if (this->value == "'\\v'") return "\"\\00\\0B\"";
    if (this->value == "'\\f'") return "\"\\00\\0C\"";
    if (this->value == "'\\r'") return "\"\\00\\0D\"";
    if (this->value == "'\\a'") return "\"\\00\\07\"";
    if (this->value == "'\\\"'") return "\"\\00\\22\"";
    if (this->value == "'\\\\'") return "\"\\00\\5C\"";
    if (this->value == "'\\\''") return "\"\\00\\27\"";

    if (this->value[1] == '\\' && this->value.size() == 8){
        return std::string("\"\\") + this->value.substr(3, 2) + "\\" + this->value.substr(5, 2) + "\"";
    }

    return std::string("\"") + this->value[1] + "\"";*/
    return "";
}

void StringLiteralAST::force_type(std::string type){
    if (type != "String"){
        parser::error("Type mismatch", tokens[0], tokens[tokens.size()-1] ,std::string("expected a \e[1m") + type + "\e[0m, found String", 17, "Caused by");
    }
}

