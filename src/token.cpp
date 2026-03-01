#pragma once


// include
#include "gmp/gmp.h"
#include "gmp/gmpxx.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>
#include <deque>
#include <map>

using namespace std;

// possible tokens
// ---------------------------------------------------------------------------
enum TOK_T {
  NIL,
  // symbols
  OP_PAREN, CL_PAREN, 
  OP_CURL, CL_CURL,
  COMMA,

  // operators
  RSHIFT, LSHIFT, 
  ADD, SUB, MUL, DIV,     

  // values
  NUM, STR, ASM, ARR // arr is special
};


// simple tokens with constant single-char mappings 
// ---------------------------------------------------------------------------
static const map<char, TOK_T> tokenMap = {
  {'(', OP_PAREN}, {')', CL_PAREN},
  {'{', OP_CURL}, {'}', CL_CURL},
  {'+', ADD}, {'-', SUB},
  {'*', MUL}, {'/', DIV},
  {',', COMMA},
};


// tokens with extra info attached
// ---------------------------------------------------------------------------
struct Token {
  // values are either numeric or string 
  Token(TOK_T token) : type(token) {}
  Token(string str) : type(STR), str(str) {}
  Token(mpf_class value) : type(NUM), num(value) {}
  Token(mpf_t value) : type(NUM) {
    num = mpf_class(value);
  }

  // safely get number or string data 
  string text() { assert(type == STR); return str; }
  mpf_class& number() { assert(type == NUM); return num; }
  vector<Token>& list() { return arr; }

  // the actual token is public
  TOK_T type;

  private:
  // Classes with unions in them is slightly weird in C++ 
  // I would rather have this tradeoff 
  string str = "";
  mpf_class num = mpf_class();
  vector<Token> arr;
};


// check for and parse a token started and ended by some literal "lit" 
// returns the empty string on failure, offset first index with start
// ---------------------------------------------------------------------------
Token tokText(string& str, ssize_t& len, size_t start = 0, char lit = '"') {
  // sanitize
  if(str.empty() || (str[start] != lit)) return Token("");
  len = -1;

  // find the delimiter position
  string left = str.substr(start+1);
  ssize_t delim = left.find(lit);
  if(delim < 0) return Token(NIL);

  // return the substring
  len = delim + 2; // +2 for quotations
  return left.substr(0, delim); 
}


// slightly more complicated than above since  numbers can be hex, oct, bin...
// returns the length of the number as a string on success, 0 on error
// also sets buf to the result but only canonicalizes on success
// ---------------------------------------------------------------------------
Token tokNumeral(string& str, ssize_t& len, size_t start) {
  // sanitize
  len = -1;
  Token res = Token(NIL);
  if(str.length() < 1) return Token(NIL);

  // try parsing a number, don't include the sign
  string cpy = str.substr(start);
  int end = cpy.find_first_not_of("0123456789abcdefABCDEFbeox.");
  if(end == 0) return Token(NIL);
  if(end < 0) end = cpy.length();
  cpy.erase(end);
  len = end;

  // try integers first and float second.
  mpz_class itmp; mpf_class ftmp;
  if(itmp.set_str(cpy, 0) >= 0) return Token(itmp);
  if(ftmp.set_str(cpy, 0) >= 0) return Token(ftmp);

  // otherwise error
  len = -1;
  return Token(NIL);
}


// token function to split some string into bunch of tokens
// sets status = 0 and returns on error
// ---------------------------------------------------------------------------
deque<Token> tokenize(string equation, bool &status) {
  // init
  deque<Token> res = deque<Token>();
  string buf = "";
  status = false;
  bool prevOptr = true; // previous was an operator or not

  // for each character in the equation
  for(size_t i = 0; i < equation.length(); i++) {
    if(isspace(equation[i])) continue;
    auto mapvalue = tokenMap.find(equation[i]);

    // trivial list, it's a minus as a sign
    if(prevOptr && (mapvalue->second == SUB)) {
      res.push_back(Token(-1));
      res.push_back(MUL);
      continue;
    }

    prevOptr = false;
    // Check the trivial list last
    if(mapvalue != tokenMap.end()) {
      Token token = Token(mapvalue->second);
      res.push_back(token);
      prevOptr = true;
      continue;
    }

    // parse for string first, if invalid parse for asm code
    ssize_t llen = -1;
    Token buf = tokText(equation, llen, i);
    if(llen <= 0) buf = tokText(equation, llen, i, '`');

    // if either of them worked we are good
    if(llen > 0) {
      res.push_back(buf);
      i += llen; // +1 to skip delimiter at the end
      continue;
    }

    // if a number is coming in 
    Token val = tokNumeral(equation, llen, i);
    if(llen > 0) {
      res.push_back(val);
      i += llen-1; // -1 because iterator handles that already
      continue;
    }

    // tokens with 2 characters
    string optr = equation.substr(i, 2);
    if((optr == ">>") || (optr == "<<")) {
      res.push_back((optr == "<<") ? LSHIFT : RSHIFT);
      i++; // since we took two letters for this token
      continue;
    }
  
    // invalid 
    else return res;
  }

  // success
  status = true;
  return res;
}


// Ensure parser works
// ---------------------------------------------------------------------------
#ifdef TEST_PARSER
int main() {
  bool status = false;
  deque<Token> tokens = tokenize("(0b1) + \"hello, world\" * 5 ", status);
  while(!tokens.empty()) {
    Token token = tokens.front();
    tokens.pop_front();
    if(token.type == NUM) cout << token.number() << endl;
    else if(token.type == STR) cout << token.text() << endl;
    else cout << token.type << endl;
  }
  if(!status) return 1;
  return 0;
}
#endif


