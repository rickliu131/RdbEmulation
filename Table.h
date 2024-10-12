// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include "TableEntry.h"
#include "Error.h"
#include "Comparator.h"

// for error reporting
#define CREATE "CREATE"
#define REMOVE "REMOVE"
#define INSERT "INSERT"
#define PRINT "PRINT"
#define DELETE "DELETE"
#define JOIN "JOIN"
#define GENERATE "GENERATE"

#define WHERE_C 'W'
#define ALL_C 'A'

#define HASH_C 'h'
#define BST_C 'b'

#define GREATER_THAN '>'
#define LESS_THAN '<'
#define EQUAL_TO '='

enum class IndexType : uint8_t { None, Hash, BST };

class Table {

    friend class Database;

    private:
        std::vector<std::vector<TableEntry> > content; //number of rows * n
        
        IndexType index_type = IndexType::None;
        std::size_t index_col = 0;
        // entry -> which row
        std::unordered_map<TableEntry, std::vector<std::size_t> > hashmap; 
        std::map<TableEntry, std::vector<std::size_t> > bst;

        // print setting
        bool is_quiet = false;

        // this is for EXTERNAL use
        // for JOIN
        void generate_index_hash(std::unordered_map<TableEntry, std::vector<std::size_t> >* hm_ext, std::size_t const& j_col_ext) {
            for (std::size_t i = 0; i < this->content.size(); ++i) {
                auto it = hm_ext->find(this->content[i][j_col_ext]);
                if (it == hm_ext->end()) {
                    std::vector<std::size_t> row_indices = { i };
                    hm_ext->insert({this->content[i][j_col_ext], row_indices});
                } else {
                    it->second.push_back(i);
                }
            }
        }

        // before calling, need to set this->index_type, and, this->index_col
        // index_type should NOT be None
        // i = which row start to generate index from
        // will generate index for rows from i to END
        void generate_index_hash(std::size_t i) {
            for (; i < this->content.size(); ++i) {
                auto it = this->hashmap.find(this->content[i][this->index_col]);
                if (it == this->hashmap.end()) {
                    std::vector<std::size_t> row_indices = { i };
                    this->hashmap.insert({this->content[i][this->index_col], row_indices});
                } else {
                    it->second.push_back(i);
                }
            }
        }
        void generate_index_bst(std::size_t i) {
            for (; i < this->content.size(); ++i) {
                auto it = this->bst.find(this->content[i][this->index_col]);
                if (it == this->bst.end()) {
                    std::vector<std::size_t> row_indices = { i };
                    this->bst.insert({this->content[i][this->index_col], row_indices});
                } else {
                    it->second.push_back(i);
                }
            }
        }

        void remove_with_comp(std::size_t const& col_index, char op, TableEntry const& threshold_te) {
            switch (op) {
                case GREATER_THAN: {
                    Greater greater(col_index, threshold_te);
                    this->content.erase(std::remove_if(this->content.begin(), this->content.end(), greater), this->content.end());
                    break;
                }
                case LESS_THAN: {
                    Less less(col_index, threshold_te);
                    this->content.erase(std::remove_if(this->content.begin(), this->content.end(), less), this->content.end());
                    break;
                }
                case EQUAL_TO: {
                    Equal equal(col_index, threshold_te);
                    this->content.erase(std::remove_if(this->content.begin(), this->content.end(), equal), this->content.end());
                    break;
                }
                default:
                    break;
            }
        }

        inline void print_rows_cols(std::vector<std::size_t> const& row_indices, std::vector<std::size_t> const& col_indices) {
            for (std::size_t const& row_index: row_indices) {
                for (std::size_t const& col_index: col_indices) {
                    std::cout << this->content[row_index][col_index] << " ";
                }
                std::cout << "\n";
            }
        }

        template<class T>
        void print_with_comp(std::vector<std::size_t> const& col_indices, T const& comp, std::size_t& count) {
            // count = 0;
            if (!this->is_quiet) {
                for (auto& row: this->content) {
                    if (comp(row)) {
                        for (auto& col_index: col_indices) {
                            std::cout << row[col_index] << " ";
                        }
                        std::cout << "\n";
                        ++count;
                    }
                }
            } else {
                for (auto& row: this->content) {
                    if (comp(row)) {
                        ++count;
                    }
                }
            }
            
        }

        void print_determine(std::vector<std::size_t> const& col_indices, std::size_t const& col_index, char op, TableEntry const& threshold_te) {
            std::size_t count = 0;
            if (this->index_type == IndexType::Hash && col_index == this->index_col && op == EQUAL_TO) {
                // hash, =
                std::vector<std::size_t> const& row_indices = this->hashmap[threshold_te];
                if (!this->is_quiet) {
                    this->print_rows_cols(row_indices, col_indices);
                }
                count = row_indices.size();
            } else if (this->index_type == IndexType::BST && col_index == this->index_col) {
                // bst, <, >, =
                switch (op) {
                    case GREATER_THAN: {
                        auto it = this->bst.upper_bound(threshold_te); //points to the first greater value
                        for (; it != this->bst.end(); ++it) {
                            // it->second = row indices
                            if (!this->is_quiet) {
                                this->print_rows_cols(it->second, col_indices);
                            }
                            count += it->second.size();
                        }
                        break;
                    }
                    case LESS_THAN: {
                        auto out = this->bst.lower_bound(threshold_te); //first value >= threshold
                        for (auto it = this->bst.begin(); it != out; ++it) {
                            if (!this->is_quiet) {
                                this->print_rows_cols(it->second, col_indices);
                            }
                            count += it->second.size();
                        }
                        break;
                    }
                    case EQUAL_TO: {
                        std::vector<std::size_t> const& row_indices = this->bst[threshold_te];
                        if (!this->is_quiet) {
                            this->print_rows_cols(row_indices, col_indices);
                        }
                        count = row_indices.size();
                        break;
                    }
                    default:
                        break;
                }
            } else {
                // none, <, >, =
                switch (op) {
                    case GREATER_THAN: {
                        Greater greater(col_index, threshold_te);
                        this->print_with_comp(col_indices, greater, count);
                        break;
                    }
                    case LESS_THAN: {
                        Less less(col_index, threshold_te);
                        this->print_with_comp(col_indices, less, count);
                        break;
                    }
                    case EQUAL_TO: {
                        Equal equal(col_index, threshold_te);
                        this->print_with_comp(col_indices, equal, count);
                        break;
                    }
                    default:
                        break;
                }
            }
            // count is now ready
            std::cout << "Printed " << count << " matching rows from " << this->name << "\n";
        }

    public:
        std::string name;
        std::vector<EntryType> col_types; //n
        std::unordered_map<std::string, std::size_t> col_names; //n

        bool insert() {
            bool entry_bool;
            double entry_double;
            int entry_int;
            std::string entry_string;

            std::size_t n_rows_insert;
            std::cin >> n_rows_insert;
            std::size_t n_rows_old = this->content.size();
            this->content.resize(this->content.size() + n_rows_insert);
            std::cin >> entry_string; // (will be overwritten!) ROWS

            for (std::size_t i = n_rows_old; i < this->content.size(); ++i) {
                this->content[i].reserve(this->col_names.size());
                for (std::size_t j = 0; j < this->col_names.size(); ++j) {
                    switch (this->col_types[j]) {
                        case EntryType::Bool:
                            std::cin >> entry_bool;
                            this->content[i].emplace_back(TableEntry(entry_bool));
                            break;
                        case EntryType::Int:
                            std::cin >> entry_int;
                            this->content[i].emplace_back(TableEntry(entry_int));
                            break;
                        case EntryType::Double:
                            std::cin >> entry_double;
                            this->content[i].emplace_back(TableEntry(entry_double));
                            break;
                        case EntryType::String:
                            std::cin >> entry_string;
                            this->content[i].emplace_back(TableEntry(entry_string));
                            break;
                        default:
                            break;
                    }
                }
            }

            // also insert new indices
            if (this->index_type == IndexType::Hash) {
                this->generate_index_hash(n_rows_old);
            } else if (this->index_type == IndexType::BST) {
                this->generate_index_bst(n_rows_old);
            }
            

            std::cout << "Added " << n_rows_insert << " rows to " << this->name 
                      << " from position " << n_rows_old << " to " << this->content.size()-1 << "\n";
            return 1;
        }

        bool print(bool is_quiet) {
            this->is_quiet = is_quiet;

            std::size_t n_cols_print;
            std::string temp;
            std::vector<std::size_t> col_print_indices;
            std::vector<std::string> col_names_temp;

            std::cin >> n_cols_print; // N (n of cols to print)
            col_print_indices.resize(n_cols_print);
            col_names_temp.resize(n_cols_print);

            // read col names from 1 to N
            std::unordered_map<std::string, std::size_t>::iterator it_col_name;
            for (std::size_t i = 0; i < n_cols_print; ++i) {
                std::cin >> temp; //col name
                it_col_name = this->col_names.find(temp);
                if (it_col_name == this->col_names.end()) {
                    // col name is not found in table, report error
                    output_error3(PRINT, temp, this->name);
                    std::getline(std::cin, temp);
                    return 0;
                } else {
                    col_print_indices[i] = it_col_name->second; //record col name's index
                    // print at next line at the same time
                    // std::cout << temp << " "; //CHANGE THIS!
                    col_names_temp[i] = temp;
                }
            }

            // WHERE or ALL
            std::cin >> temp;
            if (temp[0] == WHERE_C) {
                std::cin >> temp;
                it_col_name = this->col_names.find(temp);
                if (it_col_name == this->col_names.end()) {
                    output_error3(PRINT, temp, this->name);
                    std::getline(std::cin, temp);
                    return 0;
                }

                // no error for sure, print col names
                if (!this->is_quiet) {
                    for (std::string const& name: col_names_temp) {
                        std::cout << name << " ";
                    }
                    std::cout << "\n";
                }

                // std::string const& colname = temp; // not used!
                char op;
                std::cin >> op;
                EntryType type = this->col_types[it_col_name->second];
                switch (type) {
                    case EntryType::Bool: {
                        bool te_bool;
                        std::cin >> te_bool;
                        this->print_determine(col_print_indices, it_col_name->second, op, TableEntry(te_bool));
                        break;
                    }
                    case EntryType::Int: {
                        int te_int;
                        std::cin >> te_int;
                        this->print_determine(col_print_indices, it_col_name->second, op, TableEntry(te_int));
                        break;
                    }
                    case EntryType::Double: {
                        double te_double;
                        std::cin >> te_double;
                        this->print_determine(col_print_indices, it_col_name->second, op, TableEntry(te_double));
                        break;
                    }
                    case EntryType::String: {
                        std::string te_string;
                        std::cin >> te_string;
                        this->print_determine(col_print_indices, it_col_name->second, op, TableEntry(te_string));
                        break;
                    }
                    default:
                        break;
                }

                // std::cout << "Printed " <<  << " matching rows from " << this->name << "\n";
                // already printed inside print_determine (which is easier)

                return 1;
            } else {
                // all
                // no error for sure, print col names
                if (!this->is_quiet) {
                    // col names
                    for (std::string const& name: col_names_temp) {
                        std::cout << name << " ";
                    }
                    std::cout << "\n";
                    // rows
                    for (std::size_t i = 0; i < this->content.size(); ++i) {
                        for (std::size_t const& index: col_print_indices) {
                            std::cout << this->content[i][index] << " ";
                        }
                        std::cout << "\n";
                    }
                }
                
                std::cout << "Printed " << this->content.size() << " matching rows from " << this->name << "\n";
                return 1;
            }
        }

        bool generate() {
            // clear generates
            this->index_col = 0;
            this->hashmap.clear();
            this->bst.clear();

            std::string type;
            std::cin >> type;
            if (type[0] == HASH_C) {
                this->index_type = IndexType::Hash;
            } else {
                this->index_type = IndexType::BST;
            }
            // type should not be overwritten after this

            std::string temp;
            std::cin >> temp; // "INDEX"
            std::cin >> temp; // "ON"
            std::cin >> temp; // col name
            auto it_col_name = this->col_names.find(temp);
            if (it_col_name == this->col_names.end()) {
                // col name not found
                output_error3(GENERATE, temp, this->name);
                std::getline(std::cin, temp);
                return 0;
            }
            
            // no error, create index table
            this->index_col = it_col_name->second;
            if (this->index_type == IndexType::Hash) {
                this->generate_index_hash(0);
            } else if (this->index_type == IndexType::BST) {
                this->generate_index_bst(0);
            }

            std::cout << "Created " << type << " index for table " << this->name << " on column " << temp << "\n";
            return 1;
        }

        bool delete_() {
            std::string temp;
            std::cin >> temp; //"WHERE"
            std::cin >> temp; //colname
            auto it_col_name = this->col_names.find(temp);
            if (it_col_name == this->col_names.end()) {
                output_error3(DELETE, temp, this->name);
                std::getline(std::cin, temp);
                return 0;
            }

            // from now, no errors..
            std::size_t old_size = this->content.size();

            char op;
            std::cin >> op;
            EntryType type = this->col_types[it_col_name->second];
            switch (type) {
                case EntryType::Bool: {
                    bool te_bool;
                    std::cin >> te_bool;
                    this->remove_with_comp(it_col_name->second, op, TableEntry(te_bool));
                    break;
                }
                case EntryType::Int: {
                    int te_int;
                    std::cin >> te_int;
                    this->remove_with_comp(it_col_name->second, op, TableEntry(te_int));
                    break;
                }
                case EntryType::Double: {
                    double te_double;
                    std::cin >> te_double;
                    this->remove_with_comp(it_col_name->second, op, TableEntry(te_double));
                    break;
                }
                case EntryType::String: {
                    std::string te_string;
                    std::cin >> te_string;
                    this->remove_with_comp(it_col_name->second, op, TableEntry(te_string));
                    break;
                }
                default:
                    break;
            }

            // regenerate index for all
            if (this->index_type == IndexType::Hash) {
                this->hashmap.clear();
                // this->index_col, keep
                this->generate_index_hash(0);
            } else if (this->index_type == IndexType::BST) {
                this->bst.clear();
                this->generate_index_bst(0);
            }

            std::cout << "Deleted " << old_size - this->content.size() << " rows from " << this->name << "\n";
            return 1;
        }

        bool join() {
            return 1;
        }
};