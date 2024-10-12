// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <iostream>
#include <string>

inline void output_error1(std::string const& cmd, std::string& tname) {
    std::cout << "Error during " << cmd << ": Cannot create already existing table " << tname << "\n";
}

inline void output_error2(std::string const& cmd, std::string& tname) {
    std::cout << "Error during " << cmd << ": " << tname << " does not name a table in the database\n";
}

inline void output_error3(std::string const& cmd, std::string& colname, std::string& tname) {
    std::cout << "Error during " << cmd << ": " << colname << " does not name a column in " << tname << "\n";
}

inline void output_error4() {
    std::cout << "Error: unrecognized command\n";
}