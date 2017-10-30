#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include "Loader.h"
#include "Memory.h"

using namespace std; 

#define ADDRBEGIN 2
#define ADDREND 4
#define DATABEGIN 7
#define COMMENT 28

/* 
 * Loader
 * opens up the file named in argv[0] and loads the
 * contents into memory. If the file is able to be loaded,
 * then loaded is set to true.
 */ 
Loader::Loader(int argc, char * argv[])
{
   std::ifstream inf;
   int lineNumber = 1;
   lastAddress = -1;
   loaded = false;

   if (argc < 2 || !checker(argv[1]))
   { 
      return;
   }   

   inf.open(argv[1]);
   if (!inf.is_open())
   {
      return;
   } 
  
   std::string line;
   while (getline(inf, line))
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
      }
      lineNumber++;
   }
   loaded = true;
}

bool Loader::hasAddress(std::string line)
{
  if (line[0] == '0')
  {
    return true;
  }
  return false;
}

bool Loader::hasData(std::string line)
{	
	return !Loader::checkSpaces(line, DATABEGIN, COMMENT - 1);
}

void Loader::loadLine(std::string line)
{
  int32_t addr = Loader::convert(line, ADDRBEGIN, 3);
	bool error = false;

  for (int i = DATABEGIN; line[i] != ' '; i += 2)
  {
	  uint8_t byte = convert(line, i, 2);
	  Memory::getInstance()->putByte(byte, addr, error);
	  addr++;
  }	  
    lastAddress = addr - 1;

}

int32_t Loader::convert(std::string line, int32_t start, int32_t len)
{
   std::string result = line.substr(start, len);  
   return stoul(result.c_str(), NULL, 16); 
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

bool Loader::errorAddr(std::string line)
{
    for (int i = 2; i < 5; i++)
    {
        if (!isxdigit(line[i]))
        {
            return true;
        }
    }
	if (line[1] != 'x')
    {
        return true;
    }
    if (line[5] != ':')
    {
        return true;
    }
	if (line[6] != ' ')
    {
        return true;
    }
    return false;
}

bool Loader::checkSpaces(std::string line, int32_t start, int32_t end)
{
    for (int i = start; i <= end; i++)
    {
        if (line[i] != ' ')
        {
            return false; 
        }
    }
    return true;
}

/*
 * isLoaded
 * getter for the private loaded data member
 */
bool Loader::isLoaded()
{
   return loaded;  
}

bool Loader::checker(char * argv)
{
    std::string fn = argv;
    if(fn.substr(fn.find_last_of(".") + 1) == "yo")
    {
      return true;
    }
    return false;

}
