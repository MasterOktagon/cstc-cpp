#pragma once

#include <exception>
#include <string>
#include <vector>

namespace lexer {
    class LexerError : public std::exception {
        std::string msg = "";
        public:
            LexerError(std::string msg);
            const char* what();
    };


    class Token {
        public:
            enum TokenType {
                NONE,
                ID,
                END_CMD,
                BLOCK_OPEN,
                BLOCK_CLOSE,
                CLAMP_OPEN,
                CLAMP_CLOSE,
                INDEX_OPEN,
                INDEX_CLOSE,
                
                SET,
                ADD,
                SUB,
                MUL,
                DIV,
                MOD,
                
                BAND,
                BOR,
                BXOR,
                BNOT,
                
                INCREASE,
                DECREASE,
                POW,
                BITSHIFTL,
                BITSHIFTR,
                
                ADDSET,
                SUBSET,
                MULSET,
                DIVSET,
                MODSET,
                BANDSET,
                BORSET,
                BXORSET,
                BNOTSET,
                
                IN,
                DOT,
                SUBNS,
                QM,
                COMMA,
                
                EQUALS,
                GREATER,
                LESS,
                GEQ,
                LEQ,
                NEQ,
                
                NOT,
                OR,
                AND,
                ADDR,
                
                AS,
                IF,
                ELSE,
                FOR,
                WHILE,
                RETURN,
                CONTINUE,
                BREAK,
                NAMESPACE,
                MODULE,
                NOIMPL,
                CLASS,
                STRUCT,
                ENUM,
                FINAL,
                C,
                GOTO,
                PUBLIC,
                PRIVATE,
                PROTECTED,
                FRIEND,
                READONLY,
                CONST,
                STATIC,
                IMPLEMENTS,
                IMPORT,
                THROW,
                CATCH,
                TRY,
                NEW,
                DELETE,
                
                STRING,
                CHAR,
                INT,
                FLOAT,
                HEX,
                BINARY,
                BOOL,
                
                COMMENT,
                ML_COMMENT_OPEN,
                ML_COMMENT_CLOSE,
                EF
                
            };
            
            TokenType type;
            std::string value;
            int line, column;
            std::string line_content;
            std::string filename;
            
            Token(TokenType type, std::string value, std::string lc, int line, int column, std::string filename);
        
    };
    
    extern std::vector<Token> tokenize(std::string text, std::string filename);

}









