
#include <iostream>
#include <string>
// include read_csv
#include "csv.h"

using namespace std;


int main() {
    CSV train("train");
    CSV test(string("test"));
    
    // this gives undefined reference to `CSV::CSV(std::string)'
    // because the constructor is not defined in the header file

    vector<vector<int>> train_data = train.data;
    vector<vector<int>> test_data = test.data;

    // print out data shape
    cout << "Train: " << train_data.size() << " rows, " << train_data[0].size() << " columns" << endl;
    cout << "Test: " << test_data.size() << " rows, " << test_data[0].size() << " columns" << endl;

    // print out column names
    cout << "Column names: ";
    for (int i = 0; i < train.col_names.size(); i++) {
        cout << train.col_names[i] << ", ";
    }
    cout << endl;

    // get models

    // train models

    // evaluate and predict models

    // write to submission.csv file
    

    return 0;
}