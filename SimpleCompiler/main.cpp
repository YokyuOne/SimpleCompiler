#include <iostream>
#include <string>
#include "Symbol.h"
#include "simpleMachine.h"
#include "SimpleLanguage.h"
using namespace std;

int main()
{
    /*string hello = "hello";
    char hi = 'L';
    int loser = 10;

    Symbol dank (hello, hi, loser);
    cout << dank.getLocation() << endl;
*/

 SimpleLanguage test("TestFile.txt");
 test.compile();
 test.run();
    return 0;
}

