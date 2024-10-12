// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <iostream>
#include <vector>
#include "TableEntry.h"

class Less {
    public:
        std::size_t index_col;
        TableEntry threshold_te;
        Less(std::size_t index_col_in, TableEntry threshold_te_in): index_col(index_col_in), threshold_te(threshold_te_in) {}

        bool operator()(const std::vector<TableEntry> &row) const {
            return row[index_col] < this->threshold_te;
        }
};

class Equal {
    public:
        std::size_t index_col;
        TableEntry threshold_te;
        Equal(std::size_t index_col_in, TableEntry threshold_te_in): index_col(index_col_in), threshold_te(threshold_te_in) {}

        bool operator()(const std::vector<TableEntry> &row) const {
            return row[index_col] == this->threshold_te;
        }
};

class Greater {
    public:
        std::size_t index_col;
        TableEntry threshold_te;
        Greater(std::size_t index_col_in, TableEntry threshold_te_in): index_col(index_col_in), threshold_te(threshold_te_in) {}

        bool operator()(const std::vector<TableEntry> &row) const {
            return row[index_col] > this->threshold_te;
        }
};