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
 int lineNumber = 1;
  //int lastAddress = -1;
   //start by writing a method that opens the file (checks whether it ends with a .yo and
   //whether the file successfully opens; if not, return without loading)

   //next write a simple loop that reads the file line by line and prints it out
   //
    if(check(argc,argv))
    {
     	std::ifstream infile;
     	infile.open(argv[1]);
     	std::string line;
     	
    	while (getline(infile, line))
     	{
     		if (hasErrors(line)) 
			{
   				std::cout << "Error on line " << std::dec << lineNumber
     	        	<< ": " << line << std::endl;
   				return;
			}
     		if (hasAddress(line) && hasData(line)) 
     		{
     			loadLine(line);
      			lineNumber++;
      		}
      	}
    }      

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
   loaded = true;   // file was error free; program loaded into memory


}

bool Loader::hasErrors(std::string line)
{
   //checking for errors in a particular order can significantly 
   //simplify your code
   //1) line is at least 29 characters long and contains a '|' in column 28
   //   if not, return true
    if (line.length() < 29 || line[28] != '|')
    {
        return true;
    }
   //2) check whether line has an address.  If it doesn't,
   //   return result of checkSpaces (line must be all spaces up
   //   to the | character)
   //   Hint: use hasAddress and checkSpaces
   
    if (!Loader::hasAddress(line))
    {
        return !Loader::checkSpaces(line, 0, COMMENT - 1);      
    }
   //3) return true if the address is invalid
   //   Hint: use errorAddress 
   
    if(Loader::addrError(line))
    {
        return true;
    }
   //4) check whether the line has data. If it doesn't
   //   return result of checkSpaces (line must be all spaces from
   //   after the address up to the | character)
   //   Hint: use hasData and checkSpaces
    if(!Loader::hasData(line))
    {
        return Loader::checkSpaces(line, ADDREND + 1, COMMENT - 1);
    }
   //5) if you get past 4), line has an address and data. Check to
   //   make sure the data is valid using errorData
   //   Hint: use errorData
        int32_t count = 0;
        for (int i = DATABEGIN; i != ' '; i++)
        {
            count++;
        }
			count = count / 2;   
    int32_t arg = count;
    if (Loader::errorData(line, arg))
        {
            return true;
        }
   //6) if you get past 5), line has a valid address and valid data.
   //   Make sure that the address on this line is > the last address
   //   stored to (lastAddress is a private data member)
   //   Hint: use convert to convert address to a number and compare
   //   to lastAddress

    //int32_t current = Loader::convert(line, ADDRBEGIN, 3);    
    //if (current <= lastAddress)
   // {
    //    return true;
    //}
   //7) Make sure that the last address of the data to be stored
   //   by this line doesn't exceed the memory size
   //   Hint: use numDBytes as set by errorData, MEMSIZE in Memory.h,
   //         and addr returned by convert
 if (Loader::convert(line, ADDRBEGIN, 3) + arg > MEMSIZE)    
    {
        return true;
    }   
   // if control reaches here, no errors found
   return false;
}


bool Loader::check(int argc, char * argv[])
{
    if (argc == 2)
    {
      std::string fn = argv[1];
      if(fn.substr(fn.find_last_of(".") + 1) == "yo")
      {
          std::ifstream infile(argv[1]);
          if (!infile)
          {
              std::cerr << "Error!.\n";
              return 0;
            }
            else
            {
              return 1;
            }
          }
          else
          {
            return 0;
          }
    }
    else
    {
        return 0;
    }
}

void Loader::loadLine(std::string line)
{
   if(line[0] == '0')
   {
        if(line[DATABEGIN] != ' ')
        {
            int32_t addr = Loader::convert(line, ADDRBEGIN, ADDREND);
           // printf("%s\n%x:\n", line.c_str(), addr);
            bool yee = false;
            //char check = line[DATABEGIN];
            for(int i = DATABEGIN; line[i] != ' '; i = i + 2)
            {
                uint8_t bytenums = convert(line, i, i + 1);
                //printf("%x- ", bytenums);
                Memory::getInstance()->putByte(bytenums, addr, yee);
                //check = line[i+2];
                addr++;
            }
            //printf("\n");
        }
   }
   //lastAddress = address - 1;
  //Memory::putByte(line,address,false); 
}

int32_t Loader::convert(std::string line, int32_t start, int32_t end)
{
  //basically what im trying to do is say
  //string = char at a position of the line,
  //then you add the next char to that string
  //and so on. stoul turns string into int. 
  std::string result = line.substr(start, end - start + 1);
  /*for(int i = start; i <= end; i++)
  {
    result += swag.c_str()[i];
  }*/
  return stoul(result.c_str(), NULL, 16);
}

bool Loader::hasAddress(std::string line)
{
	if(line[0] == '0')
	{
		return true;
	}
	return false;
}

bool Loader::hasData(std::string line)
{
	if(line[DATABEGIN] != ' ')
	{
		return true;
	}
	return false;
}

bool Loader::errorData(std::string line, int32_t & numDBytes)
{
   //Hint: use isxdigit and checkSpaces
    bool ans = false;
    int i = DATABEGIN;
    int count = 0;

    do
    {
        if(!isxdigit(line[i]))
        {
            return true;
        }
        i++;
        count++;
    }
    while(line[i] != ' ');
	
    if (count % 2 != 0)
    {
        return true;
    }
    
    numDBytes = count / 2;
    ans = !Loader::checkSpaces(line, i, COMMENT - 1);

    return ans;
}


bool Loader::addrError(std::string line)
{
	for(int i = ADDRBEGIN; i <= ADDREND; i++)
	{
		if(!isxdigit(line[i]))
		{
			return true;
		}
	}

	if(line[1] != 'x')
	{
		return true;
	}

	if(line[5] != ':')
	{
		return true;
	}

	if(line[6] != ' ')
	{
		return true;
	}
	return false;
}

bool Loader::checkSpaces(std::string line, int32_t start, int32_t end)
{
	for(int i = start; i <= end; i++)
	{
		if(line[i] != ' ')
		{
			return false;
		}
	}
	return true;
}

bool Loader::goodFile(char * filename)
{
   //Hint: use strlen and strcmp
   int len = strlen(filename);
   const char *lastThree = &filename[len-3];
   char * str2 = ".yo";
	
   if (len >= 4 && !strcmp(str2, lastThree))
   {
	   return true; 
   }
   return false;
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
