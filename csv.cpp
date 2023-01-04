#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "csv.h"
#include <algorithm>

using namespace std;


vector<vector<int>> CSV::read_csv(string filename) {

    ifstream file(filename);
    string line;
    vector<vector<int>> data;
    int i = 0;
    while (getline(file, line)) {

        stringstream ss(line);
        string cell;
        vector<int> row;

        i++;
        if (i == 1) {
            // set column names
            while (getline(ss, cell, ',')) {
                col_names.push_back(cell);
            }
            continue;

        }

        while (getline(ss, cell, ',')) {
            if (cell.find('.') != string::npos) {
                float f = stof(cell);
                f = f * 2;
                int i = f;
                row.push_back(i);
            } else {
                int i = stoi(cell);
                row.push_back(i);
            }
        }

        data.push_back(row);
    }

    return data;
}

int CSV::save_csv (string filename, vector<vector<int>> data) const {
    ofstream file(filename);
    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[i].size(); j++) {
            file << data[i][j];
            if (j < data[i].size() - 1) {
                file << DELIM;
            }
        }
        file << endl;
    }
    return 0;
}
/*
int CSV::get_rows(string filename) {
    ifstream file(filename);
    string line;
    int rows = 0;
    while (getline(file, line)) {
        rows++;
    }
    return rows;
}

int CSV::get_cols(string filename) {
    ifstream file(filename);
    string line;
    getline(file, line);
    stringstream ss(line);
    string cell;
    int cols = 0;
    while (getline(ss, cell, DELIM)) {
        cols++;
    }
    return cols;
}
*/

CSV::CSV(string subset_string) {
    subset = subset_string;
    if (subset == "train") {
        data = read_csv(TRAIN_FILEPATH);
    } else if (subset == "test") {
        data = read_csv(TEST_FILEPATH);
    }
    
}

int CSV::get_unique_users() {
    vector<int> users;
    for (int i = 0; i < data.size(); i++) {
        users.push_back(data[i][0]);
    }
    sort(users.begin(), users.end());
    users.erase(unique(users.begin(), users.end()), users.end());
    return users.size();
}

int CSV::get_unique_items() {
    vector<int> items;
    for (int i = 0; i < data.size(); i++) {
        items.push_back(data[i][1]);
    }
    sort(items.begin(), items.end());
    items.erase(unique(items.begin(), items.end()), items.end());
    return items.size();
}
/*
void CSV::reset_ids() {
    vector<int> users;
    vector<int> items;
    for (int i = 0; i < data.size(); i++) {
        users.push_back(data[i][0]);
        items.push_back(data[i][1]);
    }
    sort(users.begin(), users.end());
    users.erase(unique(users.begin(), users.end()), users.end());
    sort(items.begin(), items.end());
    items.erase(unique(items.begin(), items.end()), items.end());
    for (int i = 0; i < data.size(); i++) {
        data[i][0] = find(users.begin(), users.end(), data[i][0]) - users.begin();
        data[i][1] = find(items.begin(), items.end(), data[i][1]) - items.begin();
    }
}*/
