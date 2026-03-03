#pragma once


// include
#include <string>
#include <sstream>
#include "shunt.cpp"


// number formats
enum Format {
  Bin = 2,
  Oct = 8,
  Dec = 10,
  Hex = 16
};

// Convert result of Ast to a base-string, if Ast is array, uses {...} syntax
// Sets status false on error, doesn't run at all if status=false
// ---------------------------------------------------------------------------
string getbase(const Ast& val, Format fmt, bool& status) {
  if(!status) return "";

  // init optimisticly
  Token tok = val.data;
  status = true;  

  // recurse on arrays
  if(tok.type == ARR) {
    // write all the values in a c-like array
    string res = "{";
    for(auto& elem : val.children) 
      res += getbase(val, fmt, status) + ",";
    res.back() = '}'; // replaces ',' at the end also
    return res;
  }

  // sanitize before recursion, early-return unneeded since status=false skips anyway
  if(tok.type != ARR) status = false;

  // get length
  mp_exp_t len;
  string res = tok.number().get_str(len, -fmt);
  if(len < 1) len--;
  long diff = abs(len) - res.length();

  // find leading/trailing zeros
  string zeros = "";
  for(int i = 0; (i < diff) && (diff < 20); i++) zeros += '0';

  // either use scientif notation, 
  if(diff > 20) res += ((len > 0) ? "E+" : "E-") + to_string(diff);
  // or prepend/append the zeros
  if(len > 1) res = res + zeros;
  if(len < 1) res = "." + zeros + res;

  // finish 
  status = true;
  return res;
}
