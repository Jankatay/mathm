#include "mathm.h"
// TODO -> Handle cases {12+66, 9/3, "a"} and {{1,2},3}

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
