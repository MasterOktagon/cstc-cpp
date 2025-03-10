#pragma once
#include <string>
#include <vector>

#define PRETTY_SIZE 120

namespace lexer {
/*
Holds functions adding to the tokenizing features
*/

    class Token {
        /*
        represents a token
        */

        public:
        enum TokenType {
            /*
            Enum of all available Token types
            */

                //  SPECIAL   //
            NONE     ,        // undefined token
            ID       ,        // a name
            EF       ,        // File end
            END_CMD  ,        // ;

                //  LITERALS  //
            INT      ,        // int literal
            HEX      ,        // int literal in hexadecimal
            BINARY   ,        // int literal in binary
            BOOL     ,        // bool literal
            STRING   ,        // String literal
            CHAR     ,        // char literal
            FLOAT    ,        // float literal
            NULV     ,        // "null"

                //    MATH    //
            SET      ,        // =
            ADD      ,        // +
            SUB      ,        // -
            MUL      ,        // *
            DIV      ,        // /
            MOD      ,        // %
            POW      ,        // **
            INC      ,        // ++
            DEC      ,        // --
            NEC      ,        // - <int>

            NEG      ,        // ~
            AND      ,        // &
            OR       ,        // |
            XOR      ,        // ^
            SHL      ,        // <<
            SHR      ,        // >>
            LSHR     ,        // !>

                //  LOGICAL   //
            LAND     ,        // &&
            LOR      ,        // ||
            NOT      ,        // !

                // COMPARISON //
            EQ       ,        // ==
            NEQ      ,        // !=
            LESS     ,        // <
            GREATER  ,        // >
            GEQ      ,        // >=
            LEQ      ,        // <=

                //    FLOW    //
            QM       ,        // ?
            IN       ,        // :
            UNPACK   ,        // <-
            ADDR     ,        // #
            LIFETIME ,        // @
            SUBNS    ,        // ::
            ACCESS   ,        // .
            COMMA    ,        // ,
            DOTDOT   ,        // ..

                // PARANTHESIS //
            PT_OPEN   ,       // (
            PT_CLOSE  ,       // )
            BLOCK_OPEN ,      // {
            BLOCK_CLOSE,      // }
            INDEX_OPEN ,      // [
            INDEX_CLOSE,      // ]

                //  KEYWORDS  //
            IF        ,         
            ELSE      ,         
            FOR       ,   
            WHILE     ,        
            DO        ,    
            THROW     ,      
            CATCH     ,
            TRY       ,
            BREAK     ,
            CONTINUE  ,
            NOIMPL    ,
            RETURN    ,
            AS        ,
            OPERATOR  ,

            IMPORT    ,
            IMPLEMENTS,
            CLASS     ,
            ENUM      ,
            STRUCT    ,
            VIRTUAL   ,
            INTERFACE ,
            FRIEND    ,
            PUBIC     ,
            PROTECTED ,
            PRIVATE   ,
            GUARDED   ,
            STATIC    ,
            PUBLIC    ,
            FINAL     ,
            CONST     ,
            NAMESPACE ,
            NEW
        };

        
            TokenType type;
            std::string value;

            unsigned int column = 0;
            unsigned int line = 0;

            std::string line_content = "";
            std::string filename = "";

            Token(TokenType type, std::string value, std::string lc, unsigned int line, unsigned int column, std::string filename);
            Token(){};

    };

    extern std::vector<Token> tokenize(std::string text, std::string filename);
    extern void error(std::string name, lexer::Token t, std::string msg, int code);
    extern void warn(std::string name, lexer::Token t, std::string msg, int code);
    extern void warn(std::string name, int l, int c, std::string lc, std::string file, std::string msg, int code);
}

