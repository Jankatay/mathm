#pragma once


// include
#include <string>
#include "shunt.cpp"


// Convert result of Ast to a base-string, if Ast is array, uses {...} syntax
// Sets status false on error, doesn't run at all if status=false
// ---------------------------------------------------------------------------
static mp_exp_t IGNORE;
string getbase(const Ast& val, int base, bool& status) {
  if(!status) return "";

  // init optimisticly
  status = true;  
  Token tok = val.data;

  // base case, flat values
  if(tok.type == NUM) return tok.number().get_str(IGNORE, base);
  if(tok.type == STR) return tok.text();

  // sanitize before recursion, no early-return since status of false skips anyway
  if(tok.type != ARR) status = false;

  // write all the values in a c-like array
  string arrstring = "{";
  for(auto& elem : val.children) arrstring += getbase(val, base, status);
  arrstring += "}";

  // finish 
  return arrstring;
}
