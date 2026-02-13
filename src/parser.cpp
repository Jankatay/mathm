#ifndef _PARSER_CPP
#define _PARSER_CPP

#include <vector>
#include <string>
using namespace std;


// possible tokens
// ---------------------------------------------------------------------------
enum Token {
  ERR,
  // symbols
  OP_PAREN, CL_PAREN, 
  OP_CURL, CL_CURL,    

  // operators
  ADD, SUB, MUL, DIV,     
  RSHIFT, LSHIFT,

  // numeric
  DEC, HEX, OCT, BIN,

  // patterns
  EQU, FACTOR, TERM,
  ARR, STR, VAL
};


// tokens with extra info attached
// ---------------------------------------------------------------------------
class Lex {
  // numeric or string
  union { double num; string str; } info;
  Token token;        // the actual token 
};


// lexer function to split some string into bunch of lex
// ---------------------------------------------------------------------------
vector<Lex> lexer(string equation) {
  //TODO:
  return vector<Lex>();
}


// Ensure parser works
// ---------------------------------------------------------------------------
#ifdef TEST_PARSER
int main() {
  return 0;
}
#endif


#endif
