// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <iostream>
#include "SillyQL.h"

using namespace std;

int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);
    cin >> std::boolalpha;
    cout << std::boolalpha;

    SillyQL sql;
    sql.set_options(argc, argv);
    sql.shell();
}