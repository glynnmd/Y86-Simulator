
class Loader
{
   private:
      bool loaded;   //set to true if a file is successfully loaded into memory
      ifstream inf;  //input file handle
   public:
      Loader(int argc, char * argv[]);
      bool isLoaded();
      bool check(char * argv[]);
      void adder(int argc, char * argv[]);
};
