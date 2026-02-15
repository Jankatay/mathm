%{
  #include <math.h>
  #include <stdio.h>
  #include "parser.cpp"
  int yylex(void);
  void yyerror(char const*);
%}


// Token conversions for bison
// Most of these are implicit funny enough, 
// I wasted time on parser.cpp
// I guess that's why we do personal projects
%define api.value.type {Token}
%token OP_PAREN
%token CL_PAREN
%token OP_CURL
%token CL_CURL
%token COMMA
%token STR
%token ASM
%token NUM
%token NIL
// left-associative
%left ADD
%left SUB
%left MUL
%left DIV
// right-associative
%right EXP
%right RSHIFT
%right LSHIFT

// for the LSP more than anything
extern Token yylval;
extern int YYEOF;


// backus-naur form grammar
// I am following the info-page of bison as tutorial
%%
// reuslt can be an equation or list of them
result: equations 
| OP_CURL equations CL_CURL
;

// list of equations
// string and asm-code is converted to bytes too
equations: equation
| equations COMMA equation
| STR         {}
| ASM         {}
;

equation: value 
| equation ADD equation
| equation SUB equation
| equation MUL equation
| equation DIV equation
| '(' equation ')'
;

value: NUM
| NUM RSHIFT NUM
| NUM LSHIFT NUM
| value EXP value
;
%%


// init tokens based on string input
// returns false on token error
// ---------------------------------------------------------------------------
deque<Token> tokens;
bool yyinit(string str) {
  bool status = 0;
  tokens = tokenize(str, status);
  return status;
}

// call the bison lexer 
// the lexer in parser.cpp is more so a tokenizer
// ---------------------------------------------------------------------------
int yylex(void) {
  // EOF if no tokens
  if(tokens.empty()) return YYEOF;
  
  // otherwise return the token as is
  Token res = tokens.front();
  tokens.pop_front();
  
  // copy get contents to yylval
  yylval = res;

  // return the token type
  return res.get();
}
