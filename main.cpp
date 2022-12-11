
#include <iostream>
#include <string>
// include read_csv
#include "csv.h"

using namespace std;


int main() {
    cout << "Hello, World!" << endl;
    CSV train_data("train");
    CSV test_data(string("test"));
    
    // this gives undefined reference to `CSV::CSV(std::string)'
    // because the constructor is not defined in the header file

    vector<vector<int>> train = train_data.data;
    vector<vector<int>> test = test_data.data;

    cout << "Train: " << train.size() << " rows, " << train[0].size() << " columns" << endl;
    cout << "Test: " << test.size() << " rows, " << test[0].size() << " columns" << endl;

    // get models

    // train models

    // evaluate and predict models


    return 0;
}