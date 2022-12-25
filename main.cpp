
#include <iostream>
#include <string>
// include read_csv
#include "csv.h"

using namespace std;


// write a function to get the user-item matrix
vector<vector<int>> get_user_item_matrix(vector<vector<int>> data, int num_users, int num_items) {
    vector<vector<int>> user_item_matrix(num_users, vector<int>(num_items, 0));
    for (int i = 0; i < data.size(); i++) {
        int user = data[i][0];
        int item = data[i][1];
        int rating = data[i][2];
        user_item_matrix[user][item] = rating;
    }
    return user_item_matrix;
}

int main() {
    CSV train("train");
    CSV test(string("test"));
    
    // this gives undefined reference to `CSV::CSV(std::string)'
    // because the constructor is not defined in the header file

    // print out data shape
    cout << "Train: " << train.data.size() << " rows, " << train.data[0].size() << " columns" << endl;
    cout << "Test: " << test.data.size() << " rows, " << test.data[0].size() << " columns" << endl;

    // print  first 4 rows of train data
    cout << "First 4 rows of train data: " << endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < train.data[i].size(); j++) {
            cout << train.data[i][j] << ", ";
        }
        cout << endl;
    }
    
    // print out column names
    cout << "Column names: ";
    for (int i = 0; i < train.col_names.size(); i++) {
        cout << train.col_names[i] << ", ";
    }
    cout << endl;

    // print out number of unique users and items
    int total_unique_users = train.get_unique_users();
    int total_unique_items = train.get_unique_items();


    cout << "Number of unique users: " << total_unique_users << endl;
    cout << "Number of unique items: " << total_unique_items << endl;

    vector<vector<int>> user_item_matrix = get_user_item_matrix(train.data, total_unique_users, total_unique_items);

    // write to submission.csv file
    
    return 0;
}