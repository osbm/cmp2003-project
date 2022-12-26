
#include <iostream>
#include <string>
// include read_csv
#include "csv.h"
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>


using namespace std;

// lets write a print function to print everything like python
template <typename T>
void print(T t) {
    cout << t << endl;
}

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
    /*
    cout << "Train: " << train.data.size() << " rows, " << train.data[0].size() << " columns" << endl;
    cout << "Test: " << test.data.size() << " rows, " << test.data[0].size() << " columns" << endl;
    */


    /*
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
    */

    // print out number of unique users and items
    int total_unique_users = train.get_unique_users();
    int total_unique_items = train.get_unique_items();

    /*
    cout << "Number of unique users: " << total_unique_users << endl;
    cout << "Number of unique items: " << total_unique_items << endl;
    */

    // before we start, we need to reset the IDs of the users and movies
    // so that they are continuous and start from 0
    // this is because we will use them as indices in a matrix
    // and we need to be able to access them easily


    // create a mapping from old user id to new user id
    set<int> user_ids;
    for (int i = 0; i < train.data.size(); i++) {
        user_ids.insert(train.data[i][0]);
    }
    map<int, int> user_id_map;
    int new_user_id = 0;
    for (auto it = user_ids.begin(); it != user_ids.end(); it++) {
        user_id_map[*it] = new_user_id;
        new_user_id++;
    }

    // create a mapping from old item id to new item id
    set<int> item_ids;
    for (int i = 0; i < train.data.size(); i++) {
        item_ids.insert(train.data[i][1]);
    }
    map<int, int> item_id_map;
    int new_item_id = 0;
    for (auto it = item_ids.begin(); it != item_ids.end(); it++) {
        item_id_map[*it] = new_item_id;
        new_item_id++;
    }

    // now we can reset the IDs of the users and movies
    for (int i = 0; i < train.data.size(); i++) {
        train.data[i][0] = user_id_map[train.data[i][0]];
        train.data[i][1] = item_id_map[train.data[i][1]];
    }
    for (int i = 0; i < test.data.size(); i++) {
        test.data[i][0] = user_id_map[test.data[i][0]];
        test.data[i][1] = item_id_map[test.data[i][1]];
    }
    
    
    // now we can create a user-item matrix

    vector<vector<int>> user_item_matrix = get_user_item_matrix(train.data, total_unique_users, total_unique_items);
    print(user_item_matrix.size());
    print(user_item_matrix[0].size());

    return 0;
    // why program raises segmentation fault? even though everything has worked

}