
// a function for reading a csv file into a vector of vectors of integers
// the first argument is the name of the file to be read
// the second argument is the number of rows in the file
#ifndef CSV_H
#define CSV_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>



using namespace std;

class CSV {
    private:
        
        static const char DELIM = ',';
        
    public:
        const string TRAIN_FILEPATH = "data/train.csv";
        const string TEST_FILEPATH = "data/test.csv";
        // array of column names
        vector<string> col_names;
        vector<vector<int>> data;
        vector<vector<int>> read_csv(string filename);
        int save_csv(string filename, vector<vector<int>> data) const;
        int get_rows(string filename);
        int get_cols(string filename);
        CSV(string subset);
        
};

#endif
