
#include "lexer.hpp"
#include <string>
#include <iostream>
#include <regex>

lexer::Token::Token(TokenType type, std::string value, std::string lc, unsigned int line, unsigned int column, std::string filename){
    this->value = value;
    this->line = line;
    this->column = column;
    this->type = type;
    this->line_content = lc;
    this->filename = filename;
}

void lexer::error(std::string name, lexer::Token t, std::string msg, int code){
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
}

void lexer::warn(std::string name, lexer::Token t, std::string msg, int code){
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

void lexer::warn(std::string name, int l, int c, std::string lc, std::string file, std::string msg, int code){
    std::cerr << std::string("\e[1;33mWARNING: ") + name + "\e[0m at \e[1m" + file + ":" + std::to_string(l) + ":" + std::to_string(c-1) + "\e[0m [W "+ std::to_string(code) +"]" + "\n" + msg << std::endl;
    std::cerr << std::endl << " " << std::to_string(l) << " |\t" << lc << std::endl;
    std::string pb = " ";
    
    for (int i=0; i<std::to_string(l).size(); i++) pb += " ";
    pb += " |\t";
    for (int i=0; i<c-2; i++){
        pb += " ";
    }
    pb += "\e[33m^";
    for (int i=1; i<1; i++){
        pb += "^";
    }
    pb += "\e[0m";

    std::cerr << pb << std::endl << std::endl;
}

lexer::Token::TokenType getDoubleToken(std::string s){
    /*
        Get a double delimiting token
    */

    if (s == "++") return lexer::Token::TokenType::INC;
    if (s == "--") return lexer::Token::TokenType::DEC;
    if (s == "**") return lexer::Token::TokenType::POW;

    if (s == "<<") return lexer::Token::TokenType::SHL;
    if (s == ">>") return lexer::Token::TokenType::SHR;
    if (s == "!>") return lexer::Token::TokenType::LSHR;

    if (s == "&&") return lexer::Token::TokenType::LAND;
    if (s == "||") return lexer::Token::TokenType::LOR;

    if (s == "==") return lexer::Token::TokenType::EQ;
    if (s == "!=") return lexer::Token::TokenType::NEQ;
    if (s == ">=") return lexer::Token::TokenType::GEQ;
    if (s == "<=") return lexer::Token::TokenType::LEQ;

    if (s == "<-") return lexer::Token::TokenType::UNPACK;
    if (s == "::") return lexer::Token::TokenType::SUBNS;

    return lexer::Token::TokenType::NONE;
}

lexer::Token::TokenType getSingleToken(char c){
    /*
        Get a single delimiting token
    */

    switch (c) {
        default:   return lexer::Token::TokenType::NONE;
        case ';':  return lexer::Token::TokenType::END_CMD;

        case '=':  return lexer::Token::TokenType::SET;
        case '+':  return lexer::Token::TokenType::ADD;
        case '-':  return lexer::Token::TokenType::SUB;
        case '*':  return lexer::Token::TokenType::MUL;
        case '/':  return lexer::Token::TokenType::DIV;
        case '%':  return lexer::Token::TokenType::MOD;
        case '~':  return lexer::Token::TokenType::NEG;
        case '&':  return lexer::Token::TokenType::AND;
        case '|':  return lexer::Token::TokenType::OR;
        case '^':  return lexer::Token::TokenType::XOR;
        case '!':  return lexer::Token::TokenType::NOT;

        case '<':  return lexer::Token::TokenType::LESS;
        case '>':  return lexer::Token::TokenType::GREATER;

        case '?':  return lexer::Token::TokenType::QM;
        case ':':  return lexer::Token::TokenType::IN;
        case '#':  return lexer::Token::TokenType::ADDR;
        case '@':  return lexer::Token::TokenType::LIFETIME;
        case '.':  return lexer::Token::TokenType::ACCESS;
        case ',':  return lexer::Token::TokenType::COMMA;

        case '(':  return lexer::Token::TokenType::PT_OPEN;
        case ')':  return lexer::Token::TokenType::PT_CLOSE;
        case '{':  return lexer::Token::TokenType::BLOCK_OPEN;
        case '}':  return lexer::Token::TokenType::BLOCK_CLOSE;
        case '[':  return lexer::Token::TokenType::INDEX_OPEN;
        case ']':  return lexer::Token::TokenType::INDEX_CLOSE;
    }
}

lexer::Token::TokenType matchType(std::string c){

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
    else if (c == "import"){ type = lexer::Token::TokenType::IMPORT; }
    else if (c == "noimpl"){ type = lexer::Token::TokenType::NOIMPL; }
    else if (c == "implements"){ type = lexer::Token::TokenType::IMPLEMENTS; }
    else if (c == "class"){ type = lexer::Token::TokenType::CLASS; }
    else if (c == "struct"){ type = lexer::Token::TokenType::STRUCT; }
    else if (c == "enum"){ type = lexer::Token::TokenType::ENUM; }
    else if (c == "final"){ type = lexer::Token::TokenType::FINAL; }
    else if (c == "do"){ type = lexer::Token::TokenType::DO; }
    else if (c == "public"){ type = lexer::Token::TokenType::PUBLIC; }
    else if (c == "private"){ type = lexer::Token::TokenType::PRIVATE; }
    else if (c == "guarded"){ type = lexer::Token::TokenType::GUARDED; }
    else if (c == "protected"){ type = lexer::Token::TokenType::PROTECTED; }
    else if (c == "const"){ type = lexer::Token::TokenType::CONST; }
    else if (c == "static"){ type = lexer::Token::TokenType::STATIC; }
    else if (c == "throw"){ type = lexer::Token::TokenType::THROW; }
    else if (c == "catch"){ type = lexer::Token::TokenType::CATCH; }
    else if (c == "try"){ type = lexer::Token::TokenType::TRY; }
    else if (c == "new"){ type = lexer::Token::TokenType::NEW; }
    //else if (c == "delete"){ type = lexer::Token::TokenType::DELETE; }
    
    return type;
}

std::string getline_from_str(std::string text, unsigned int line){
    unsigned int current_line = 1;
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
    
    unsigned i = 0, c = 0, l = 1;
    std::string buffer = "";
    char next = ' ';
    bool in_string = false;
    bool in_char = false;
    bool in_inline_comment = false;
    int ml_comment_level = 0;
    int last_linetoolong = 0;
    
    while ((size_t) i < text.size()){
        c++;
        if(text[i] == '\n'){c=1; l++;}
        if (c > PRETTY_SIZE && text[i] != ' ' && text[i] != '\t' && text[i] != '\n' && last_linetoolong != l){
            warn("Line too long", l, 101, getline_from_str(text, l-bool(text[i]=='\n')), filename, "It will become hard to read if you do long lines", 109);
            last_linetoolong = l;
        }
        if(text[i] == '\t') c += 3;
        //if (text[i] == '\n'){c = 1; l++;}
        if (in_inline_comment && text[i] != '\n'){
            i++;
            continue;
        }
        next = i+1 < text.size()? text[i+1] : ' ';

        if (ml_comment_level < 1 && next == '/' && text[i] == '/'){
            in_inline_comment = true;
            Token::TokenType type = matchType(buffer);
            if (buffer != ""){
                tokens.push_back(Token(type, buffer, getline_from_str(text, l-bool(text[i]=='\n')), l, c-buffer.size()+1, filename));
                buffer = "";
            }
            continue;
        }
        else if (!in_inline_comment && next == '*' && text[i] == '/'){
            ml_comment_level++;
            Token::TokenType type = matchType(buffer);
            if (buffer != ""){
                tokens.push_back(Token(type, buffer, getline_from_str(text, l-bool(text[i]=='\n')), l, c-buffer.size(), filename));
                buffer = "";
            }
        }
        else if (!in_inline_comment && next == '/' && text[i] == '*'){
            ml_comment_level--;
            if (ml_comment_level < 0){
                error("Unopened multiline comment", Token(Token::TokenType::NONE, "*/", getline_from_str(text, l-bool(text[i]=='\n')), l, c, filename), "This */ multiline closing was never opened", 2350);
                ml_comment_level = 0;
            }
            c+=2;
            i+=2;
            continue;
        }

        Token::TokenType single_type = getSingleToken(text[i]);
        Token::TokenType dual_type = next != ' ' ? getDoubleToken(std::string("") + text[i] + next) : Token::TokenType::NONE;

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
            else if (text[i] == ' ' || text[i] == '\t' || text[i] == '\n' || i+1 >= text.size()){
                Token::TokenType type = matchType(buffer);
                if (buffer != ""){
                    tokens.push_back(Token(type, buffer, getline_from_str(text, l-bool(text[i]=='\n')), l, c-buffer.size(), filename));
                    buffer = "";
                }
                in_inline_comment = false;
            }
            else if (dual_type != Token::TokenType::NONE){
                Token::TokenType type = matchType(buffer);
                if (buffer != ""){
                    tokens.push_back(Token(type, buffer, getline_from_str(text, l), l, c-buffer.size(), filename));
                    buffer = "";
                }
                tokens.push_back(Token(dual_type, std::string("") + text[i] + text[i+1], getline_from_str(text, l), l, c, filename));
                i++;
            }
            else if (single_type != Token::TokenType::NONE){
                Token::TokenType type = matchType(buffer);
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

    tokens.push_back(Token(Token::TokenType::EF, "", "",l,c,filename));
    for (Token t : tokens){
        //if (t.type == Token::TokenType::CHAR && t.value.size() > 1)
    }

    #ifdef DEBUG
        std::cout << "Tokenizing finished!" << std::endl;
    #endif
    return tokens;
}


