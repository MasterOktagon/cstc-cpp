
#include <cstdio>
#include "lexer.hpp"
#include <regex>
#include <iostream>

lexer::LexerError::LexerError(std::string message){
    msg = message;
}

const char* lexer::LexerError::what(){
    return msg.c_str();
}

lexer::Token::Token(TokenType type, std::string value, std::string lc, int line, int column, std::string filename){
    this->value = value;
    this->line = line;
    this->column = column;
    this->type = type;
    this->line_content = lc;
    this->filename = filename;
}

bool in(char c, std::vector<char> options){
    for (char o : options){
        if (o == c){return true;}
    }
    return false;
}

bool in(std::string c, std::vector<std::string> options){
    for (std::string o : options){
        if (o == c){return true;}
    }
    return false;
}

lexer::Token::TokenType get_mark(std::string c){
    lexer::Token::TokenType type = lexer::Token::TokenType::NONE;
    if (c == "=="){ type = lexer::Token::TokenType::EQUALS;}
    else if (c == "++"){ type = lexer::Token::TokenType::INCREASE; }
    else if (c == "--"){ type = lexer::Token::TokenType::DECREASE; }
    else if (c == "**"){ type = lexer::Token::TokenType::POW; }
    else if (c == "<<"){ type = lexer::Token::TokenType::BITSHIFTL; }
    else if (c == ">>"){ type = lexer::Token::TokenType::BITSHIFTR; }
    
    else if (c == "+="){ type = lexer::Token::TokenType::ADDSET; }
    else if (c == "-="){ type = lexer::Token::TokenType::SUBSET; }
    else if (c == "*="){ type = lexer::Token::TokenType::MULSET; }
    else if (c == "/="){ type = lexer::Token::TokenType::DIVSET; }
    else if (c == "%="){ type = lexer::Token::TokenType::MODSET; }
    else if (c == "&="){ type = lexer::Token::TokenType::BANDSET; }
    else if (c == "|="){ type = lexer::Token::TokenType::BORSET; }
    else if (c == "^="){ type = lexer::Token::TokenType::BXORSET; }
    else if (c == "~="){ type = lexer::Token::TokenType::BNOTSET; }

    else if (c == "||"){ type = lexer::Token::TokenType::OR; }
    else if (c == "&&"){ type = lexer::Token::TokenType::AND; }
    
    else if (c == "!="){ type = lexer::Token::TokenType::NEQ; }
    else if (c == "<="){ type = lexer::Token::TokenType::LEQ; }
    else if (c == ">="){ type = lexer::Token::TokenType::GEQ; }
    
    else if (c == "::"){ type = lexer::Token::TokenType::SUBNS; }
    
    else if (c == "//"){ type = lexer::Token::TokenType::COMMENT; }
    else if (c == "/*"){ type = lexer::Token::TokenType::ML_COMMENT_OPEN; }
    else if (c == "*/"){ type = lexer::Token::TokenType::ML_COMMENT_CLOSE; }
    return type;
}

lexer::Token::TokenType get_mark(char c){
    lexer::Token::TokenType type;
    switch (c){
        default: type = lexer::Token::TokenType::NONE; break;
        case ';': type = lexer::Token::TokenType::END_CMD; break;
        case '[': type = lexer::Token::TokenType::INDEX_OPEN; break;
        case ']': type = lexer::Token::TokenType::INDEX_CLOSE; break;
        case '(': type = lexer::Token::TokenType::CLAMP_OPEN; break;
        case ')': type = lexer::Token::TokenType::CLAMP_CLOSE; break;
        case '{': type = lexer::Token::TokenType::BLOCK_OPEN; break;
        case '}': type = lexer::Token::TokenType::BLOCK_CLOSE; break;
        
        case '=': type = lexer::Token::TokenType::SET; break;
        case '+': type = lexer::Token::TokenType::ADD; break;
        case '-': type = lexer::Token::TokenType::SUB; break;
        case '*': type = lexer::Token::TokenType::MUL; break;
        case '/': type = lexer::Token::TokenType::DIV; break;
        case '%': type = lexer::Token::TokenType::MOD; break;
        
        case '.': type = lexer::Token::TokenType::DOT; break;
        case ':': type = lexer::Token::TokenType::IN; break;
        
        case '>': type = lexer::Token::TokenType::GREATER; break;
        case '<': type = lexer::Token::TokenType::LESS; break;
        case '!': type = lexer::Token::TokenType::NOT; break;
        case '&': type = lexer::Token::TokenType::BAND; break;
        case '|': type = lexer::Token::TokenType::BOR; break;
        case '^': type = lexer::Token::TokenType::BXOR; break;
        case '~': type = lexer::Token::TokenType::BNOT; break;
        case '?': type = lexer::Token::TokenType::QM; break;
        case ',': type = lexer::Token::TokenType::COMMA; break;
        case '#': type = lexer::Token::TokenType::ADDR; break;
        
    }
    return type;
}

lexer::Token::TokenType get_token(std::string c){
    lexer::Token::TokenType type = lexer::Token::TokenType::ID;
    std::cmatch m;

    if (c == "true" || c == "false") return lexer::Token::TokenType::BOOL;

    const std::regex int_regex("[0-9]+");
    const std::regex float_regex("([0-9]+\\.([0-9])*|\\.([0-9])+|[0-9]+f)");
    const std::regex hex_regex("0x[0-9a-fA-F]+");
    const std::regex binary_regex("0b[0-1]+");
    
    if (regex_match(c.c_str(), m, int_regex)){return lexer::Token::TokenType::INT;}
    if (regex_match(c.c_str(), m, float_regex)){return lexer::Token::TokenType::FLOAT;}
    if (regex_match(c.c_str(), m, hex_regex)){return lexer::Token::TokenType::HEX;}
    if (regex_match(c.c_str(), m, binary_regex)){return lexer::Token::TokenType::BINARY;}
    
    if (c[0] == '\'' && c[c.size()-1] == '\''){return lexer::Token::TokenType::CHAR;}
    if (c[0] == '\"' && c[c.size()-1] == '\"'){return lexer::Token::TokenType::STRING;}
    
    if (c == "as"){ type = lexer::Token::TokenType::AS;}
    else if (c == "if"){ type = lexer::Token::TokenType::IF; }
    else if (c == "else"){ type = lexer::Token::TokenType::ELSE; }
    else if (c == "for"){ type = lexer::Token::TokenType::FOR; }
    else if (c == "while"){ type = lexer::Token::TokenType::WHILE; }
    else if (c == "return"){ type = lexer::Token::TokenType::RETURN; }
    else if (c == "continue"){ type = lexer::Token::TokenType::CONTINUE; }
    else if (c == "break"){ type = lexer::Token::TokenType::BREAK; }
    else if (c == "namespace"){ type = lexer::Token::TokenType::NAMESPACE; }
    else if (c == "module"){ type = lexer::Token::TokenType::MODULE; }
    else if (c == "import"){ type = lexer::Token::TokenType::IMPORT; }
    else if (c == "noimpl"){ type = lexer::Token::TokenType::NOIMPL; }
    else if (c == "implements"){ type = lexer::Token::TokenType::IMPLEMENTS; }
    else if (c == "class"){ type = lexer::Token::TokenType::CLASS; }
    else if (c == "struct"){ type = lexer::Token::TokenType::STRUCT; }
    else if (c == "enum"){ type = lexer::Token::TokenType::ENUM; }
    else if (c == "final"){ type = lexer::Token::TokenType::FINAL; }
    else if (c == "C"){ type = lexer::Token::TokenType::C; }
    else if (c == "goto"){ type = lexer::Token::TokenType::GOTO; }
    else if (c == "public"){ type = lexer::Token::TokenType::PUBLIC; }
    else if (c == "private"){ type = lexer::Token::TokenType::PRIVATE; }
    else if (c == "protected"){ type = lexer::Token::TokenType::PROTECTED; }
    else if (c == "readonly"){ type = lexer::Token::TokenType::READONLY; }
    else if (c == "const"){ type = lexer::Token::TokenType::CONST; }
    else if (c == "static"){ type = lexer::Token::TokenType::STATIC; }
    else if (c == "throw"){ type = lexer::Token::TokenType::THROW; }
    else if (c == "catch"){ type = lexer::Token::TokenType::CATCH; }
    else if (c == "try"){ type = lexer::Token::TokenType::TRY; }
    else if (c == "new"){ type = lexer::Token::TokenType::NEW; }
    else if (c == "delete"){ type = lexer::Token::TokenType::DELETE; }
    
    return type;
}

std::string getline_from_str(std::string text, int line){
    int current_line = 1;
    std::string s = "";
    for (char c : text){
        if (c == '\n'){
            current_line++;
        }
        else if (line == current_line){
            s += c;
        }
    }
    return s;
}

std::vector<lexer::Token> lexer::tokenize(std::string text, std::string filename){
    std::vector<Token> tokens = {};
    
    unsigned long i = 0, c = 0, l = 1;
    std::string buffer = "";
    bool in_string = false;
    bool in_char = false;
    bool in_inline_comment = false;
    int ml_comment_level = 0;
    //bool in_multiline_comment = false;
    
    while ((size_t) i < text.size()){
        c++;
        if (text[i] == '\n'){c = 1; l++;}
        if (in_inline_comment && text[i] != '\n'){
            i++;
            continue;
        }
        Token::TokenType single_type = get_mark(text[i]);
        Token::TokenType dual_type = (size_t) i < (text.size() - 1) ?get_mark(std::string("") + text[i] + text[i+1]) : Token::TokenType::NONE;
        
        if (dual_type == lexer::Token::TokenType::ML_COMMENT_OPEN){
            ml_comment_level += 1;
            i++;
            continue;
        }
        else if (dual_type == lexer::Token::TokenType::ML_COMMENT_CLOSE){
            ml_comment_level -= 1;
            i+=2;
            continue;
        }

        if(ml_comment_level <= 0){
            if (text[i] == '\"' && text[i-1] != '\\' && !in_char){
                in_string = !in_string;
            }
            if (text[i] == '\'' && text[i-1] != '\\' && !in_string){
                in_char = !in_char;
            }
            if (in_string || in_char){
                buffer += text[i];
            }
            else if (dual_type == Token::TokenType::COMMENT){
                in_inline_comment = true;
            }
            else if (text[i] == ' ' || text[i] == '\t' || text[i] == '\n' || i+1 >= text.size()){
                Token::TokenType type = get_token(buffer);
                if (buffer != ""){
                    tokens.push_back(Token(type, buffer, getline_from_str(text, l-bool(text[i]=='\n')), l, c-buffer.size(), filename));
                    buffer = "";
                }
                in_inline_comment = false;
            }
            else if (dual_type != Token::TokenType::NONE){
                Token::TokenType type = get_token(buffer);
                if (buffer != ""){
                    tokens.push_back(Token(type, buffer, getline_from_str(text, l), l, c-buffer.size(), filename));
                    buffer = "";
                }
                tokens.push_back(Token(dual_type, std::string("") + text[i] + text[i+1], getline_from_str(text, l), l, c, filename));
                i++;
            }
            else if (single_type != Token::TokenType::NONE){
                Token::TokenType type = get_token(buffer);
                if (buffer != ""){
                    tokens.push_back(Token(type, buffer, getline_from_str(text, l), l, c-buffer.size(), filename));
                    buffer = "";
                }
                tokens.push_back(Token(single_type, std::string("") + text[i], getline_from_str(text, l), l, c, filename));
            }
            else{
                buffer += text[i];
            }
        }
        
        i++;
    }

    tokens.push_back(Token(Token::TokenType::EF, "", getline_from_str(text, l), l, c, filename));
    
    return tokens;
}











