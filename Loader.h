
class Loader
{
   private:
      bool loaded;   //set to true if a file is successfully loaded into memory
      ifstream inf;  //input file handle
   public:
      Loader(int argc, char * argv[]);
      bool isLoaded();
      bool check(int argc, char * argv[]);
      void loadLine(std::string line);
      int32_t convert(std::string s, int start, int end);
      bool hasErrors(std::string line);
      bool hasAddress(std::string line);
      bool hasData(std::string line);
      bool errorData(std::string line, int32_t & numDBytes);
      bool addrError(std::string line);
      bool checkSpaces(std::string line, int32_t start, int32_t end);
      bool goodFile(char * filename);
};
