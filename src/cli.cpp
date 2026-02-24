#pragma once 


// include 
#include "convert.cpp"
#include "shunt.cpp"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>


// Calculate a string and return the resolved Abstract Syntax Tree
// Sets status false on error
// -----------------------------------------------------------------
Ast solve(string str, bool& status);


// print the general usage and exit the program with EXIT_SUCCESS
// -----------------------------------------------------------------
const static void printUsage();


// parse commandline 
// returns the equation args concatonated 
// sets mode 0 on cli-mode and 1 on interactive
// -----------------------------------------------------------------
const static string handleOptions(int argc, char* argv[], int& mode);


// both calculate for the interactive and cli modes
// both return EXIT_FAILURE on error and EXIT_SUCCESS on success
// -----------------------------------------------------------------
const static int calcInteractive();
const static int calcCommandline(string equation);


// Get equation from commandline and print results
int main(int argc, char* argv[]) {
  // Init 
  int mode = 0;
  string equation = "";
  if(argc <= 1) mode = 2;
  else equation = handleOptions(argc, argv, mode);

  // pass to the respective mode
  if(mode == 1) return calcCommandline(equation);
  else if(mode == 2) return calcInteractive();
  else return EXIT_FAILURE;
  
  // invalid mode
  return EXIT_SUCCESS;
}


// Calculate a string and return the resolved Abstract Syntax Tree
// Sets status false on error
// ---------------------------------------------------------------
Ast solve(string str, bool& status) {
  // Sanitize  
  if(str.empty()) return Ast(NIL);

  // Tokenize
  deque<Token> tokens = tokenize(str, status);
  if(!status) return Ast(NIL);

  // Resolve
  vector<Ast> calculations = shunt(tokens, status);
  if(calculations.size() != 1) return Ast(NIL);
  if(!status) return Ast(NIL);
  Ast res = calculations.back();

  // Return
  status = res.resolve();
  return res;
}


// print the general usage and exit the program with EXIT_SUCCESS
constexpr const static char* usage =  
"Usage: ./main [Option] <equations> \n" 
"Options :                          \n"
"--help/-h -> Print this message    \n"
"-i        -> Interactive mode      \n";
// ---------------------------------------------------------------
const static void printUsage() {
  fprintf(stderr, "%s\n", usage);
  exit(EXIT_SUCCESS);
}


// parse commandline 
// returns the equation args concatonated 
// sets mode 0 on invalid, 1 on interacitve, and 2 on cli-mode
// -----------------------------------------------------------------
const static string handleOptions(int argc, char* argv[], int& mode) {
  // init 
  string equation = "";
  mode = 1; // assume commandline
  if(!argv || argc <= 1) mode = 0;

  // while parsing commandline options
  for(int i = 1; (i < argc) && (mode > 1); i++) {
    // get the arg
    char* arg = argv[i];
    int len = strlen(argv[i]);

    // usage
    if(strncmp(arg, "help", len) == 0) printUsage();
    else if(strncmp(arg, "--help", len) == 0) printUsage();
    else if(strncmp(arg, "h", len) == 0) printUsage();
    else if(strncmp(arg, "-h", len) == 0) printUsage();

    // interactive
    if(strncmp(arg, "-i", len) == 0) mode = 2;
    else if(strncmp(arg, "--interactive", len) == 0) mode = 2;

    // default
    else equation.append(argv[i], len);
  }

  return equation;
}


// Prompt user and calculate from input
// Returns exit status like EXIT_SUCCESS
// -----------------------------------------------------------------
const static int calcInteractive() {
  // init 
  cout << "mathm> ";
  string buf;

  // while getting delimited input 
  while(getline(cin, buf)) {
    // calculate and print
    int res = calcCommandline(buf);
    if(res == EXIT_FAILURE) return res;

    // prompt next
    cout << "mathm> ";
  }

  // success
  return EXIT_SUCCESS;
}


// Calculate from a string, useful for cli usage
// Prints output in decimal, hex, oct, and char
// Numeral arrays are printed as {...}, char ones printed as string
// Returns EXIT_SUCCESS on success and EXIT_FAILURE on failure
// -----------------------------------------------------------------
const static int calcCommandline(string equation) {
  // init
  bool status = true;
  if(equation.empty()) return EXIT_FAILURE;

  // calculate
  Ast res = solve(equation, status);
  if(!status) return EXIT_FAILURE;

  // print
  bool s = true;
  cout << "Dec -> " << getbase(res, 10, s) << endl;
  cout << "Hex -> " << getbase(res, 16, s) << endl;
  cout << "Oct -> " << getbase(res, 8, s) << endl;
  cout << "Bin -> " << getbase(res, 1, s) << endl;

  return s? EXIT_SUCCESS : EXIT_FAILURE;
}
