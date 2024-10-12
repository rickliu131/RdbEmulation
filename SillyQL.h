// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <iostream>
#include <string>
#include <getopt.h>
#include "Database.h"

// for recognizing commands
#define CREATE_C 'C'
#define QUIT_C 'Q'
#define COMMENT_C '#'
#define REMOVE_C 'R'
#define INSERT_C 'I'
#define PRINT_C 'P'
#define DELETE_C 'D'
#define JOIN_C 'J'
#define GENERATE_C 'G'

// hashmap, table_name: table_content

class SillyQL {
    private:
        bool option_help = false;
        bool option_quiet = false;
        Database db;

    public:
        void set_options(int argc, char* argv[]) {
            int choice;
            int index = 0;

            option long_options[] = {
                { "help", no_argument, nullptr, 'h' },
                { "quiet", no_argument, nullptr, 'q' },
                { nullptr, 0, nullptr, '\0' },
            };
    
            while ((choice = getopt_long(argc, argv, "hq", long_options, &index)) != -1) {
                switch (choice) {
                    case 'h':
                        this->option_help = true;
                        // print
                        exit(0);
                        break;
                    case 'q':
                        this->option_quiet = true;
                        break;
                    default:
                        break;
                }
            }
        }

        void shell() {
            std::string cmd;
            do {
                if (std::cin.fail()) {
                    std::cout << "Error: Reading from cin has failed\n";
                    exit(1);
                }

                std::cout << "% ";
                std::cin >> cmd;
                
                // process cmd
                std::string trash;
                // std::cout << "cmd[0]: " << cmd[0] << "\n";
                switch (cmd[0]) {
                    // db commands
                    case QUIT_C:
                        std::cout << "Thanks for being silly!\n";
                        break;
                    case COMMENT_C:
                        std::getline(std::cin, trash);
                        break;
                    case CREATE_C:
                        if (!this->db.create()) {
                            continue;
                        }
                        break;
                    case REMOVE_C:
                        if (!this->db.remove()) {
                            continue;
                        }
                        break;
                    // table commands
                    case INSERT_C:
                        if (!this->db.insert()) {
                            continue;
                        }
                        break;
                    case PRINT_C:
                        if (!this->db.print(this->option_quiet)) {
                            continue;
                        }
                        break;
                    case DELETE_C:
                        if (!this->db.delete_()) {
                            continue;
                        }
                        break;
                    case JOIN_C:
                        if (!this->db.join(this->option_quiet)) {
                            continue;
                        }
                        break;
                    case GENERATE_C:
                        if (!this->db.generate()) {
                            continue;
                        }
                        break;
                    // invalid commands
                    default:
                        output_error4();
                        std::getline(std::cin, trash);
                        break;
                }
            } while (cmd[0] != QUIT_C);
        }
};