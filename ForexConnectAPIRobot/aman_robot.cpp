#include <iostream>
#include <string>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <algorithm>
using namespace std;
using namespace boost;

int parse_params(string params)
{
    string params = "token, test   string";

    char_separator<char> sep(", ");
    tokenizer< char_separator<char> > tokens(params, sep);
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
         cin >> c;
         if (c.compare("login")==0){login(c);}
   }
}
int main(){
run_program();
}
