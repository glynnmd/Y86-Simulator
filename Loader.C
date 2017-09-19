/**
 * Names:
 * Team:
*/
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>

using namespace std;

#include "Loader.h"
#include "Memory.h"

//first column in file is assumed to be 0
#define ADDRBEGIN 2   //starting column of 3 digit hex address 
#define ADDREND 4     //ending column of 3 digit hext address
#define DATABEGIN 7   //starting column of data bytes
#define COMMENT 28    //location of the '|' character 

/**
 * Loader constructor
 * Opens the .yo file named in the command line arguments, reads the contents of the file
 * line by line and loads the program into memory.  If no file is given or the file doesn't
 * exist or the file doesn't end with a .yo extension or the .yo file contains errors then
 * loaded is set to false.  Otherwise loaded is set to true.
 *
 * @param argc is the number of command line arguments passed to the main; should
 *        be 2
 * @param argv[0] is the name of the executable
 *        argv[1] is the name of the .yo file
 */
Loader::Loader(int argc, char * argv[])
{
   loaded = false;

   //start by writing a method that opens the file (checks whether it ends with a .yo and
   //whether the file successfully opens; if not, return without loading)
   
   //next write a simple loop that reads the file line by line and prints it out
   
   //next, add a method that will write the data in the line to memory (call that from within
   //your loop)

   //Finally, add code to check for errors in the input line.
   //When your code finds an error, you need to print an error message and return.
   //Since your output has to be identical to your instructor's, use this cout to print the
   //error message.  Change the variable names if you use different ones.
   //  cout << "Error on line " << dec << lineNumber
   //       << ": " << line << endl;

   //if the end of the function is reached, then the input file was error free and
   //all of the lines were loaded into memory. Uncomment line below when ready:
   //
   //loaded = true;   // file was error free; program loaded into memory
  
}

/**
 * isLoaded
 * returns the value of the loaded data member; loaded is set by the constructor
 *
 * @return value of loaded (true or false)
 */
bool Loader::isLoaded()
{
   return loaded;
}


//You'll need to add more helper methods to this file.  Don't put all of your code in the
//Loader constructor.  When you add a method here, add the prototype to Loader.h in the private
//section.
