#include "Search.h"
#include <iostream>
using namespace std;


int main(){
    string pattern("\r\n\r\n");
    string target("bcabcab\r\n\r\nc");
    Search s(pattern);
    size_t pos = 0;
    cout << s.match(target, pos) << endl;
    cout << "pos:" << pos<<endl;
}