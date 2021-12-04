#include <iostream>
#include <string>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/assign/list_inserter.hpp>
using namespace std;
using namespace boost;

int parse_params()
{
    string text = "token, test   string";

    char_separator<char> sep(", ");
    tokenizer< char_separator<char> > tokens(text, sep);
    BOOST_FOREACH (const string& t, tokens) {
        cout << t << "." << endl;
    }
}

int login(string c){
   //if (sizeof(argv[] < 4){
   cout << "usage : login username password accounttype";
   //}
}


int run_program(){
   int i = 0;
   string c;
   while (c.compare("quit")!=0){
         cout << ">> : " ;
         getline(cin,c);
         if (c.compare("login")==0){login(c);}
         else if (c.compare("parse")==0){};
         
   }
}






int main(){
//run_program();

   int first[5]={1,2,3,4,5};   
   cout << "Length of array = " << (sizeof(first)/sizeof(*first)) << endl;
   parse_params();

    return 0;
}
