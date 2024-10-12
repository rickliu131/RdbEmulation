// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <iostream>
#include <unordered_map>
#include "Table.h"

// for type recognition
#define BOOL_C 'b'
#define INT_C 'i'
#define DOUBLE_C 'd'
#define STRING_C 's'

#define TABLE1_C '1'
#define TABLE2_C '2'

class Database {
    public:
        std::unordered_map<std::string, Table> tables;

        bool create() {
            std::string table_name;
            std::string temp;
            std::cin >> table_name;
            if (this->tables.find(table_name) != this->tables.end()) {
                // table already exists, error
                // std::cout << "Error during CREATE: Cannot create already existing table " << table_name << "\n";
                output_error1(CREATE, table_name);
                std::getline(std::cin, temp);
                return 0; 
            }
            // no error...
            Table table;
            table.name = table_name;
            std::size_t n_cols;
            std::cin >> n_cols;
            table.col_types.reserve(n_cols);
            table.col_names.reserve(n_cols);
            // loop through col types
            std::size_t i = 0;
            for (; i < n_cols; ++i) {
                std::cin >> temp; // reuse temp
                switch (temp[0]) {
                    case BOOL_C:
                        table.col_types.push_back(EntryType::Bool);
                        break;
                    case INT_C:
                        table.col_types.push_back(EntryType::Int);
                        break;
                    case DOUBLE_C:
                        table.col_types.push_back(EntryType::Double);
                        break;
                    case STRING_C:
                        table.col_types.push_back(EntryType::String);
                        break;
                    default:
                        break;
                }
            }
            // loop through col names
            std::cout << "New table " << table.name << " with column(s) ";
            i = 0;
            for (; i < n_cols; ++i) {
                std::cin >> temp;
                std::cout << temp << " ";
                table.col_names.insert({temp, i});
            }
            std::cout << "created\n";
            this->tables.insert({table.name, table});
            return 1;
        }

        bool remove() {
            std::string table_name;
            std::cin >> table_name;
            if (this->tables.find(table_name) == this->tables.end()) {
                // if table not found, error
                // std::cout << "Error during REMOVE: " << table_name << " does not name a table in the database\n";
                output_error2(REMOVE, table_name);
                std::getline(std::cin, table_name); // since table_name won't be used
                return 0;
            }
            // no error.. remove table
            this->tables.erase(table_name);
            std::cout << "Table " << table_name << " deleted\n";
            return 1;
        }

        bool insert() {
            std::string table_name;
            std::cin >> table_name; // INTO
            std::cin >> table_name;
            auto it_table = this->tables.find(table_name);
            if (it_table == this->tables.end()) {
                // not exist
                output_error2(INSERT, table_name);
                std::size_t n_temp;
                std::cin >> n_temp;
                getline(std::cin, table_name); // (table_name not used) remaining line, "...ROWS"
                // for (size_t i=0; i < n_temp; ++i) {
                //     getline(std::cin, table_name);
                // }
                return 0;
            }
            // no error
            return it_table->second.insert(); //always return 1, since no more possible error
        }

        bool print(bool is_quiet) {
            std::string table_name;
            std::cin >> table_name; //"FROM"
            std::cin >> table_name; // real table_name
            auto it_table = this->tables.find(table_name);
            if (it_table == this->tables.end()) {
                // not exist
                output_error2(PRINT, table_name);
                getline(std::cin, table_name); // remaining line, "...ROWS"
                return 0;
            }
            return it_table->second.print(is_quiet);
        }

        bool generate() {
            std::string table_name;
            std::string index_type;

            std::cin >> table_name; // "FOR"
            std::cin >> table_name; // real
            auto it_table = this->tables.find(table_name);
            if (it_table == this->tables.end()) {
                // not exist
                output_error2(GENERATE, table_name);
                getline(std::cin, table_name); // remaining line
                return 0;
            }
            return it_table->second.generate();
        }

        bool delete_() {
            std::string table_name;

            std::cin >> table_name; // "FROM"
            std::cin >> table_name; // real
            auto it_table = this->tables.find(table_name);
            if (it_table == this->tables.end()) {
                // not exist
                output_error2(DELETE, table_name);
                getline(std::cin, table_name); // remaining line
                return 0;
            }
            return it_table->second.delete_();
        }

        bool join(bool is_quiet) {
            // since it takes two tables, this function is completed in DB instance
            std::string temp;

            std::cin >> temp; //tablename1
            std::unordered_map<std::string, Table>::iterator it_table = this->tables.find(temp);
            if (it_table == this->tables.end()) {
                output_error2(JOIN, temp);
                getline(std::cin, temp); // remaining line
                return 0;
            }
            Table& table1 = it_table->second;
            std::cin >> temp; //"AND"
            std::cin >> temp; //tablename2
            it_table = this->tables.find(temp);
            if (it_table == this->tables.end()) {
                output_error2(JOIN, temp);
                getline(std::cin, temp); // remaining line (trash can still go to temp1)
                return 0;
            }
            Table& table2 = it_table->second; //THIS SHOULD NOT CHANGE table1
            // std::cout << "table1.name: " << table1.name << "\n";
            // std::cout << "table2.name: " << table2.name << "\n";

            // no tablename error
            // read colname 1 and 2 (should be in table 1 and 2 respectively)
            std::cin >> temp; // "WHERE"
            std::cin >> temp; // colname1
            // std::cout << "colname1: " << temp << "\n";
            std::unordered_map<std::string, std::size_t>::iterator it_col = table1.col_names.find(temp);
            if (it_col == table1.col_names.end()) {
                output_error3(JOIN, temp, table1.name);
                getline(std::cin, temp); // remaining line
                return 0;
            }
            std::size_t const& col1 = it_col->second;
            std::cin >> temp; // "="
            std::cin >> temp; // colname2
            // std::cout << "colname2: " << temp << "\n";
            it_col = table2.col_names.find(temp);
            if (it_col == table2.col_names.end()) {
                output_error3(JOIN, temp, table2.name);
                getline(std::cin, temp); // remaining line
                return 0;
            }
            std::size_t const& col2 = it_col->second;

            // std::cout << col1 << " - " << col2 << "\n";


            // no tablename error + no colname error
            std::cin >> temp; //"AND"
            std::cin >> temp; //"PRINT"

            std::size_t n_print_cols;
            std::cin >> n_print_cols;

            std::vector<std::size_t> col_print_indices;
            std::vector<char> col_print_indices_which;
            std::vector<std::string> col_print_names;
            col_print_indices.resize(n_print_cols);
            col_print_indices_which.resize(n_print_cols);
            col_print_names.resize(n_print_cols);

            char which_table = ' ';
            for (std::size_t i = 0; i < n_print_cols; ++i) {
                std::cin >> temp; //col name
                std::cin >> which_table;

                col_print_indices_which[i] = which_table;
                if (which_table == TABLE1_C) {
                    // table 1
                    it_col = table1.col_names.find(temp);
                    if (it_col == table1.col_names.end()) {
                        output_error3(JOIN, temp, table1.name);
                        std::getline(std::cin, temp);
                        return 0;
                    } else {
                        col_print_indices[i] = it_col->second; //stores table1 col print indices
                        col_print_names[i] = temp; //luckily, temp has not been overwritten!
                    }
                } else {
                    // table 2
                    it_col = table2.col_names.find(temp);
                    if (it_col == table2.col_names.end()) {
                        output_error3(JOIN, temp, table2.name);
                        std::getline(std::cin, temp);
                        return 0;
                    } else {
                        col_print_indices[i] = it_col->second; //stores table2 col print indices
                        col_print_names[i] = temp;
                    }
                }
            }
            // both table 1 and 2 store col print indices to the same variable, col_print_indices, they are mixed...
            // their indices are essentially different, should use with caution!!!!!
            if (!is_quiet) {
                for (std::string const& name: col_print_names) {
                    std::cout << name << " ";
                }
                std::cout << "\n";
            }

            // no more errors!
            // start..

            // generate temp hashmap, if needed
            std::unordered_map<TableEntry, std::vector<std::size_t> > hm; //may be unused
            std::unordered_map<TableEntry, std::vector<std::size_t> >* hm_ptr;
            if (table2.index_type == IndexType::Hash && col2 == table2.index_col) {
                hm_ptr = &(table2.hashmap);
            } else {
                // we need to generate a temp one
                // clear table2's hashmap after join finishes (such a waste..)
                hm_ptr = &hm;
                table2.generate_index_hash(hm_ptr, col2);
            }

            // loop through rows in table1
            std::size_t count = 0;
            std::unordered_map<TableEntry, std::vector<std::size_t> >::iterator it_row_indices;
            for (auto& t1_row: table1.content) {
                // TableEntry const& value1 = row[col1];
                it_row_indices = hm_ptr->find(t1_row[col1]);
                if (it_row_indices != hm_ptr->end()) {
                    // table2 rows found, which match with current table1 row's col1 value
                    if (!is_quiet) {
                        // check if any whichs is 2?
                        for (auto& t2_row_index: it_row_indices->second) {
                            // print col values (can be in table2, or 1)
                            for (std::size_t k = 0; k < col_print_indices.size(); ++k) {
                                // line below is WHICH! not without which!!!
                                if (col_print_indices_which[k] == TABLE1_C) {
                                    // col in t1
                                    std::cout << t1_row[col_print_indices[k]] << " ";
                                } else {
                                    // col in t2
                                    std::cout << table2.content[t2_row_index][col_print_indices[k]] << " ";
                                }
                            }
                            std::cout << "\n";
                        }
                    }
                    count += it_row_indices->second.size();
                }
            }
            // hm_ptr will be automatically cleared..?

            std::cout << "Printed " << count << " rows from joining " << table1.name << " to " << table2.name << "\n";
            return 1;
        }
};