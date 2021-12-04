#include <iomanip>
#include <iostream>
using namespace std;

int main( int argc, char* argv[] )
  {
  cout << "Total number of arguments are : " << argc-1 << "\n";
  cout << "The name used to start the program: " << argv[ 0 ]
       << "\nArguments are:\n";
  for (int n = 1; n < argc; n++)
    cout << setw( 2 ) << n << ": " << argv[ n ] << '\n';
  return 0;
  }