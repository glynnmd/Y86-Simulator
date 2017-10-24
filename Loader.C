#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include "Loader.h"
#include "Memory.h"

#define ADDRBEGIN 2
#define ADDREND 4
#define DATABEGIN 7
#define COMMENT 28
 
Loader::Loader(int argc, char * argv[])
{
   std::ifstream inf;  //input file stream for reading from file
   int lineNumber = 1;
   lastAddress = -1;
   loaded = false;

   //if no file name given or filename badly formed, return without loading
   if (argc < 2 || !goodFile(argv[1])) return;  
   inf.open(argv[1]);

   //if file can't be opened, return without loading
   if (!inf.is_open()) return;  
  
   std::string line;
   while (getline(inf, line))
   {
      if (hasErrors(line))
      {
         std::cout << "Error on line " << std::dec << lineNumber 
              << ": " << line << std::endl;
         return;
      }
      if (hasAddress(line) && hasData(line)) loadLine(line);
      lineNumber++;
   }
   loaded = true;
}


bool Loader::hasAddress(std::string line)
{
	bool ans = false;
	
	if (line[0] == '0')
	{
		ans = true;
	}
	return ans;
}


bool Loader::hasData(std::string line)
{
	
	return !Loader::checkSpaces(line, DATABEGIN, COMMENT - 1);	
}


void Loader::loadLine(std::string line)
{

   int32_t address = Loader::convert(line, 2, 3);
	bool error = false;

  for (int i = DATABEGIN; line[i] != ' '; i += 2)
  {
	  uint8_t byte = convert(line, i, 2);
	  Memory::getInstance()->putByte(byte, address, error);
	  address++;
  }	  
    lastAddress = address - 1;

}


int32_t Loader::convert(std::string line, int32_t start, int32_t len)
{
   std::string linePart = line.substr(start, len);   
   int32_t ans = strtol(linePart.c_str(), NULL, 16); 
   return ans;  
}


bool Loader::hasErrors(std::string line)
{
    if (line.length() < 29 || line[28] != '|')
    {
        return true;
    }
  
   
    if (!Loader::hasAddress(line))
    {
        return !Loader::checkSpaces(line, 0, COMMENT - 1);      
    }

    if(Loader::errorAddr(line))
    {
        return true;
    }

    if(!Loader::hasData(line))
    {

        return Loader::checkSpaces(line, ADDREND + 1, COMMENT - 1);
    }
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

    int32_t current = Loader::convert(line, ADDRBEGIN, 3);    
    if (current <= lastAddress)
    {
        return true;
    }
 if (Loader::convert(line, ADDRBEGIN, 3) + arg > MEMSIZE) 
    {
        return true;
    }
     
   return false;
}

/*
 * errorData
 * Called when the line contains data. It returns true if the data
 * in the line is valid. Valid data consists of characters in the range
 * '0' .. '9','a' ... 'f', and 'A' .. 'F' (valid hex digits). 
 * The data digits start at index DATABEGIN.
 * The hex digits come in pairs, thus there must be an even number of them.
 * In addition, the characters after the last hex digit up to the
 * '|' character at index COMMENT must be spaces. 
 * If these conditions are met, errorData returns false, else errorData
 * returns true.
 *
 * @param line - input line from the .yo file
 * @return numDBytes is set to the number of data bytes on the line
 */
bool Loader::errorData(std::string line, int32_t & numDBytes)
{
    
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

/*
 * errorAddr
 * This function is called when the line contains an address in order
 * to check whether the address is properly formed.  An address must be of
 * this format: 0xHHH: where HHH are valid hex digits.
 * 
 * @param line - input line from a .yo input file
 * @return true if the address is not properly formed and false otherwise
 */
bool Loader::errorAddr(std::string line)
{
   //Hint: use isxdigit
   bool ans = false;
    for (int i = 2; i < 5; i++)
    {
        if (!isxdigit(line[i]))
        {
            ans = true;
        }
    }
	if (line[1] != 'x')
    {
        ans = true;
    }
    if (line[5] != ':')
    {
        ans = true;
    }
	if (line[6] != ' ')
    {
        ans = true;
    }
    return ans;
}

/* 
 * checkSpaces
 * This function checks that characters in the line starting at 
 * index start and ending at index end are all spaces.
 * This can be used to check for errors
 *
 * @param line - string containing a line from a .yo file
 * @param start - starting index
 * @param end - ending index
 * @return true, if the characters in index from start to end are spaces
 *         false, otherwise
 */
bool Loader::checkSpaces(std::string line, int32_t start, int32_t end)
{
    bool ans = true;
    for (int i = start; i <= end; i++)
    {
        if (line[i] != ' ')
        {
            ans = false;
        }
    }

    return ans;
}

bool Loader::goodFile(char * filename)
{
   bool answer = false;
   int len = strlen(filename);
   const char *lastThree = &filename[len-3];
   char * stringIwant = ".yo";
   if (len >= 4 && !strcmp(stringIwant, lastThree))
   {
	   answer = true;   
   }
   return answer;
}


/*
 * isLoaded
 * getter for the private loaded data member
 */
bool Loader::isLoaded()
{
   return loaded;  
}

//
