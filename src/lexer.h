#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

typedef struct token_t* Token;

typedef enum {
  TOKEN_ERROR,
  
  TOKEN_INTEGER,
  TOKEN_IDENTIFIER,
  TOKEN_KEYWORD,
  TOKEN_STRING,
  
  TOKEN_MODULO,

  TOKEN_SEMICOLON,
  TOKEN_COLON,
  TOKEN_COMMA,
  TOKEN_EQUAL,
  TOKEN_EQUAL_EQUAL,
  TOKEN_GT,
  TOKEN_LT,
  TOKEN_GT_EQUAL,
  TOKEN_LT_EQUAL,
  TOKEN_INITIALZE, 

  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_DIVIDE,
  TOKEN_MULT,

  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACKET,
  TOKEN_RIGHT_BRACKET,
  

  TOKEN_QUOTATION,
  TOKEN_PRINT,
  TOKEN_FUN,
  TOKEN_LET,
  TOKEN_RETURN,
  TOKEN_FOR,
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_RANGE,
} TokenType;


Token makeToken(std::string literal, TokenType type);
TokenType getTokenType(Token token);
std::string getTokenLiteral(Token token);

#ifdef DEBUG_FLAG
void printToken(Token token);
#endif


class Lexer {
  std::string buffer;
  size_t cursor;
  size_t capacity;
  std::vector<Token> tokens;
  public:
  Lexer(const std::string filename);
  ~Lexer(); 
  void init();
  constexpr inline char peek();
  std::vector<Token>& tokenize();
  void advance();
  constexpr inline char peekNext();
  constexpr inline char peekTwo();

  void consume_current_token();
  Token peek_token();
  Token peek_next_token(); 
  #ifdef DEBUG_FLAG
  void print();
  #endif

};

#endif
