#pragma once


// include
#include "token.cpp"
#include <stack>
#include <queue>

// hash value for shunting yard
typedef pair<TOK_T, int> thash;

// node in the abstract syntax tree
struct Ast {
  Token data;
  vector<Ast> children;
  Ast(Token tok) : data(tok), children() {}
  Ast(TOK_T tok) : Ast(Token(tok)) {}
  void print(size_t depth=0); // for debugging
  // calculate contents
  bool resolve(); 
};


// calculate an operator for two given Asts
// ---------------------------------------------------------------------------
bool calculate(Ast& left, TOK_T optr, Ast& right) {
  TOK_T ltype = left.data.type;
  TOK_T rtype = right.data.type;

  // handle arrays
  if(ltype == ARR) for(auto& elem : left.children) calculate(elem, optr, right); 
  else if(rtype == ARR) {
    Ast res = Token(ARR);
    for(auto& elem : right.children) {
      Ast tmp = left;
      calculate(tmp, optr, elem);
      res.children.push_back(tmp);
    }
    left = res;
  }
  if((ltype == ARR) || (rtype == ARR)) return true; // handled

  // calculate 
  assert((ltype == NUM) && (rtype == NUM));
  switch(optr) {
    case ADD: left.data.number() += right.data.number(); break;
    case SUB: left.data.number() -= right.data.number(); break;
    case MUL: left.data.number() *= right.data.number(); break;
    case DIV: left.data.number() /= right.data.number(); break;
    case LSHIFT: left.data.number() <<= right.data.number().get_d(); break;
    case RSHIFT: left.data.number() >>= right.data.number().get_d(); break;
    default: return false; // failure
  }

  // left.data.number() += right.data.number();
  return true;
};

// weights for the shunting yard 
const map<TOK_T, int> shuntMap = {
  // flat values are 0
  {NUM, 0}, {ARR, 0},

  // values that `can` resolve to flat are negative 
  {STR, -1}, {ASM, -1},

  // symbols are listed by weight
  {OP_PAREN, 2},
  {OP_CURL, 1},
  {LSHIFT, 3}, {RSHIFT, 3},
  {MUL, 4}, {DIV, 4},
  {ADD, 5}, {SUB, 5},
  {CL_PAREN, 31},
  {COMMA, 32}, {CL_CURL, 32}
};


// push value to output for shunting yard
// returns false on invalid 
// ---------------------------------------------------------------------------
bool pushOutput(vector<Ast>& output, Ast optr) {
  // math operators
  if(output.size() < 2) return false;
  bool invalid = (optr.data.type != ADD) && (optr.data.type != SUB);
  invalid &= (optr.data.type != MUL) && (optr.data.type != DIV);
  invalid &= (optr.data.type != RSHIFT) && (optr.data.type != LSHIFT);
  if(invalid) return false;

  // get two children for the operator
  Ast left = output.back(); output.pop_back();
  Ast right = output.back(); output.pop_back();
  optr.children.push_back(right);
  optr.children.push_back(left);
  output.push_back(optr);
  return true;
}


// push value to optr for shunting yard
// returns false on invalid 
// ---------------------------------------------------------------------------
bool pushOptr(stack<thash>& optr, vector<Ast>& output, TOK_T tok, int weight) {
  // push everything from stack to output queue depending on weight
  while(!optr.empty()) {
    // reach down to eq
    auto top = optr.top();
    // "or-equal" for the parentheses handling
    if(top.second > weight) break; 
    if((top.second >= weight) && (weight > 30)) break; 
    optr.pop();
    // lower weight ones should "overflow" into the output
    pushOutput(output, top.first);
  }

  // special parantheses tokens modify themselves once established
  if(tok == OP_PAREN) weight = shuntMap.find(CL_PAREN)->second;
  if(tok == OP_CURL) {
    weight = shuntMap.find(CL_CURL)->second;
    output.push_back(ARR);
  }

  // no operators yet
  if(optr.empty()) {
    optr.push({tok, weight});
    return true;
  }

  // same priority/weight, handle special cases 
  TOK_T elem = optr.top().first;
  if((elem == OP_PAREN) && (tok == CL_PAREN)) optr.pop();
  if((elem == OP_CURL) && (tok == CL_CURL)) optr.pop();

  // on comma and }, manage array
  if((tok == COMMA) || (tok == CL_CURL)) {
    // safely get the values 
    if(output.size() < 1) return false;
    Ast child = output.back(); output.pop_back();
    // empty set
    if(child.data.type == ARR) {
      output.push_back(child);
      return true;
    }
    // fill list and link otherwise
    if(output.size() < 1) return false;
    Ast parent = output.back(); output.pop_back();
    if(parent.data.type != ARR) return false;
    parent.children.push_back(child);
    output.push_back(parent);
    return true;
  }

  // finally put yourself into the optr list
  if((tok != CL_PAREN)) optr.push({tok, weight});
  return true;
}


// shunting yard algorithm
// status is set true on success, left alone otherwise
// ---------------------------------------------------------------------------
vector<Ast> shunt(deque<Token> data, bool& status) { 
  // init head node and operator stack
  stack<thash> optr{};
  vector<Ast> output;
  status = true;   // optimist

  // while getting tokens 
  for(Token token : data) {
    // get the weight and sanitize
    auto shash = shuntMap.find(token.type);
    if(shash == shuntMap.end()) return output;
    int weight = shash->second;

    // get a flat value if can and directly send to output
    //if(weight < 0) token.resolve();
    if(weight <= 0) {
      output.push_back(Ast(token));
      continue;
    } 

    // otherwise try operator
    TOK_T tok = shash->first;
    if(pushOptr(optr, output, tok, weight)) continue;

    // invalid
    status = false;
    break;
  }

  // push everything left over
  while(!optr.empty()) {
    status = pushOutput(output, optr.top().first);
    if(!status) break;
    optr.pop();
  }

  // success  
  return output;
}


// for debugging
// ---------------------------------------------------------------------------
void Ast::print(size_t depth) {
  for(size_t i = 0; i < depth; i++) printf("\t");
  switch(data.type) {
  case MUL: printf("MUL\n"); break;
  case ADD: printf("ADD\n"); break;
  case SUB: printf("SUB\n"); break;
  case DIV: printf("DIV\n"); break;
  case ARR: printf("ARR\n"); break;
  case NUM: cout << data.number() << endl; break;
  default: printf("NIL\n");
  }
  for(auto child : children) child.print(depth+1);
}


// claculate contents
// return false on error
// ---------------------------------------------------------------------------
bool Ast::resolve() {
  // base cases, numbers
  if((data.type == ARR) && (children.size() == 1)) {
    children[0].resolve();
    *this = children[0];
    return true;
  }
  if(data.type == NUM) return true;

  // string, split into array of bytes
  if(data.type == STR) {
    for(char c : data.text()) {
      // init number 
      mpf_t tmp;
      mpf_init(tmp);
      // set value to the ASCII and push
      mpf_set_d(tmp, c);
      children.push_back(Token(tmp));
    }
    data.type = ARR;
    return true;
  }

  // TODO: asm using libcapstone

  // first resolve the children
  for(Ast& child : children)  {
    if(!child.resolve()) return false;
  }
  if(data.type == ARR) return true;

  // node is an operator
  if(children.size() != 2) return false;
  Ast right = children.back(); children.pop_back();
  Ast left = children.back(); children.pop_back();
  if(!calculate(left, data.type, right)) return false;
  *this = left;

  // success
  return true;
}


#ifdef TEST_SHUNT
int main() {
  bool status = false;
  deque<Token> tokens = tokenize("{1, {2, 3}, 3}", status);
  if(!status) return 1;
  
  vector<Ast> ast = shunt(tokens, status);
  if(!status) return 1;

  for(auto& elem : ast) {
    if(!elem.resolve()) return 1;
    elem.print();
  }

  return 0;
}
#endif
