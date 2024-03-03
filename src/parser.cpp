
#include "parser.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>

Parser::Parser(std::string source) {
  Lexer     lexer(source);
  std::vector<Token> tokenss = lexer.tokenize();
  for(int i = 0; i < tokenss.size(); i++) {
    tokens.push_back(makeToken(getTokenLiteral(tokenss[i]), getTokenType(tokenss[i])));
  }
  current = 0; 
}


static void consumeToken(std::vector<Token>& tokens) {
  #ifdef DEBUG_FLAG
    printToken(tokens[0]);
  #endif
  tokens.erase(tokens.begin());
}

bool Parser::match(TokenType type) {
  Token current = peek_current();
  advance();
  if(getTokenType(current) == type) return true;
  return false;
}


typedef enum {
   Expr_Number,
   Expr_BinOp,
   Expr_FunCall,
} ExprType;

typedef enum {
  BINOP_PLUS,
  BINOP_MULT,
} BinopType;


class Expr {
  public:
  Expr(){};
  Expr(ExprType type): expr_type(type) {};
  Expr(int number, ExprType type): number(number), expr_type(type) {};
  ~Expr(); 
  int value() {
    return number;
  };
  ExprType getType() {
    return expr_type;
  };
  private:
    ExprType expr_type;
    int number;
};

class ExprBinary: public Expr {
  public:
    ExprBinary(Expr* lhs,Expr* rhs,BinopType binop):Expr(Expr_BinOp) ,lhs(lhs), rhs(rhs), binop(binop) {};
    ~ExprBinary() {
      delete lhs;
      delete rhs;
    };
    Expr* getLhs() {
      return lhs;
    };
    Expr* getRhs() {
      return rhs;
    };
    BinopType getBinopType() {
      return binop;
    };
  private:
    Expr*     lhs;
    Expr*     rhs;
    BinopType binop;
};

class ExprFunCall: public Expr {
  public:
    ExprFunCall(){};
    ~ExprFunCall() {
      for(auto expr: args) {
        delete expr;
      }
    };
  private:
    std::vector<Expr*> args;
    std::string        name;
};



// parses numbers and funcalls
Expr* parse_primary(Parser& parser) {
  Token t = parser.peek_current();
  parser.advance();
  
  if(getTokenType(t) == TOKEN_INTEGER) {
    
    int num = std::stoi(getTokenLiteral(t));
    Expr* number = new Expr((int)num, Expr_Number);
       
    return number;
  } else {
    std::cout << "unexpected token" << std::endl;
  }
  return NULL;
}

Expr* parse_expr_mult(Parser& parser) 
{
  Expr* lhs = parse_primary(parser);
  if(!lhs) return NULL; 
  if(getTokenType(parser.peek_current()) == TOKEN_MULT) {
    parser.advance();
    Expr* rhs = parse_primary(parser);
    if(!rhs) return NULL;
    Expr* plus = new ExprBinary(lhs, rhs, BINOP_MULT);  
    if(!plus) return NULL;
    return plus;
  }
  return lhs;

}

Expr* parse_expr_plus(Parser& parser) {
  Expr* lhs = parse_expr_mult(parser);
  if(!lhs) {
    std::cout << "im at expr_plus lhs" << std::endl;
    return NULL; }
  if(getTokenType(parser.peek_current()) == TOKEN_PLUS) {
    parser.advance();
    Expr* rhs = parse_expr_mult(parser);
    if(!rhs) {
      std::cout << "im at expr_plus rhs" << std::endl;
      return NULL;
    }
    Expr* plus = new ExprBinary(lhs, rhs, BINOP_PLUS); 
    
    if(!plus) {
      std::cout << "im at expr_plus plus" << std::endl;
      return NULL;
    }
    
    return plus;
  }
  return lhs;
}

Expr* parse_expression(Parser& parser) {
 return parse_expr_plus(parser); 
}

void compile_expr(FILE* out,Expr* expr, size_t* stack_size){
  ExprType type = expr->getType();
 
    if(type == Expr_Number) {
      std::cout << expr->value();
      fprintf(out, "  %%s%zu =w copy %d\n", *stack_size, expr->value()); 
      *stack_size += 1;
    } else if(type ==
    Expr_BinOp) {  
      ExprBinary* tmp = (ExprBinary*)expr;
      compile_expr(out,tmp->getLhs(), stack_size);
      compile_expr(out,tmp->getRhs(), stack_size);
      switch(tmp->getBinopType()) {
        case BINOP_PLUS:
          fprintf(out, "  %%s%zu =w add %%s%zu, %%s%zu\n", *stack_size - 2, *stack_size - 2 , *stack_size - 1); 
          *stack_size -= 1;
          break;
        case BINOP_MULT:
          fprintf(out, "  %%s%zu =w mul %%s%zu, %%s%zu\n", *stack_size - 2, *stack_size - 2 , *stack_size - 1); 
          *stack_size -= 1;

          break;
      }; 
    } else {
      std::cout << "error" <<std::endl;
    }
     
}

Token Parser::peek_current() {
  if(current >= tokens.size()) return NULL;
  return tokens[current];
}

Token Parser::peek_next_token() {
  if(current + 1 >= tokens.size()) return NULL;

  return tokens[current + 1];
}

void Parser::advance() {
  if(current >= tokens.size()) return;
  current++;
}

void Parser::init(){
  FILE* out = std::fopen("./main.ssa", "wb");
  if(!out) return;
  
  fprintf(out , "export function w $main() {\n");
  fprintf(out , "@start\n");
  
  size_t stack_size = 0;
  while(current < tokens.size()) { 
    Expr* root =  parse_expression(*this);  
    if(!root) {
      std::cout << "root is null" << std::endl;
      return;
    }
    compile_expr(out, root, &stack_size);
  }
  fprintf(out, "  call $printf(l $fmt, ... ,w %%s%zu)\n", stack_size - 1);
  fprintf(out, "  ret 0\n");
  fprintf(out, "}\n");
  fprintf(out, "data $fmt = { b \"%%d\\n\", b 0}");

  std::fclose(out); 
} 


