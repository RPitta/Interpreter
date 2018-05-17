#include "projlex.h"
#include <iostream>
#include <string>
#include <cctype>
using namespace std;

string toString(const Token& tok) {
     
    if (tok == SET) {
        return "SET";
    }
    else if (tok == PRINT) {
        return "PRINT";
    }
    else if (tok == VAR) {
        return "VAR";
    }
    else if (tok == REPEAT) {
        return "REPEAT";
    }
    else if (tok == IDENT) {
        return "IDENT";
    }
    else if (tok == ICONST) {
        return "ICONST";
    }
    else if (tok == SCONST) {
        return "SCONST";
    }
    else if (tok == PLUS) {
        return "PLUS";
    }
    else if (tok == MINUS) {
        return "MINUS";
    }
    else if (tok == STAR) {
        return "STAR";
    }
    else if (tok == COLON) {
        return "COLON";
    }
    else if (tok == RSQ) {
        return "RSQ";
    }
    else if (tok == LSQ) {
        return "LSQ";
    }
    else if (tok == LPAREN) {
        return "LPAREN";
    }
    else if (tok == RPAREN) {
        return "RPAREN";
    }
    else if (tok == SC) {
        return "SC";
    }
    else if (tok == ERR) {
        return "ERR";
    }
    return "DONE";
}

ostream& operator<<(ostream& out, const Token& tok) {
    
    if (tok == SCONST || tok == ICONST || tok == IDENT) {
        out << toString(tok) << "(" + tok.GetLexeme() + ")\n";
    }
    else {
        out << toString(tok) << '\n';
    }
    return out;
}
    
Token getNextToken (istream *in, int *linenumber) {     

    char ch;
    string lexeme = "";
    
    while ( in->get(ch) ) {
        
        while (isspace(ch) && !in->eof()) {
            if (ch == '\n') {
                (*linenumber)++;
            }
            in->get(ch);
        }
     
        if (isalpha(ch)) {
            while (isalpha(ch) || isdigit(ch)) {
                lexeme += ch;
                in->get(ch);
            }
            if (lexeme == "var") {
                in->putback(ch);
                return Token(VAR, lexeme, *linenumber);
            }
            else if (lexeme == "print") {
                in->putback(ch);
                return Token(PRINT, lexeme, *linenumber);
            }
            else if (lexeme == "set") { 
                in->putback(ch);
                return Token(SET, lexeme, *linenumber);
            }
            else if (lexeme == "repeat") {
                in->putback(ch);
                return Token(REPEAT, lexeme, *linenumber);
            }
            in->putback(ch);
            return Token(IDENT, lexeme, *linenumber);
        }
     
        switch(ch) {
            
            case '+':
                return Token(PLUS, "+", *linenumber);
          
            case '-':
                in->get(ch);
                if (isdigit(ch)) {
                    while (isdigit(ch)) {
                        lexeme += ch;
                        in->get(ch);
                    }
                    if (isalpha(ch)) {
                        lexeme += ch;
                        return Token(ERR, lexeme, *linenumber);
                    }
                    in->putback(ch);
                    lexeme = "-" + lexeme;
                    return Token(ICONST, lexeme, *linenumber);
                }
                else {
                    return Token(MINUS, "-", *linenumber);
                }
          
            case '*':
                return Token(STAR, "*", *linenumber);
          
            case ':':
                return Token(COLON, ":", *linenumber);
          
            case '[':
                return Token(LSQ, "[", *linenumber);
            
            case ']':
                return Token(RSQ, "]", *linenumber);
          
            case '(':
                return Token(LPAREN, "(", *linenumber);
          
            case ')':
                return Token(RPAREN, ")", *linenumber);
          
            case ';':
                return Token(SC, ";", *linenumber);
          
        }
          
        if (isdigit(ch)) {
            while (isdigit(ch)) {
                lexeme += ch;
                in->get(ch);
            }
            if (isalpha(ch)) {
                lexeme += ch;
                return Token(ERR, lexeme, *linenumber);
            }
            in->putback(ch);
            return Token(ICONST, lexeme, *linenumber);
        }   
        
        if (ch == '#') {
            while (ch != '\n') {
                in->get(ch);
            }
            in->putback(ch);
        }
     
        if (ch == '"') {
            do {
                if (ch != '"') { 
                    lexeme += ch;  
                }
                in->get(ch);
            }while ((ch != '\n') && (ch != '"'));
            if (ch == '\n') {
                lexeme += ch;
                lexeme = '"' + lexeme;
                (*linenumber)++;
                return Token(ERR, lexeme, *linenumber);
            }
            return Token(SCONST, lexeme, *linenumber);
        }
    }
    
    return Token(DONE, lexeme, *linenumber);
}
