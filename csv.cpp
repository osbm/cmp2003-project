#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "csv.h"

using namespace std;



// CSV::CSV(char* subset) {
//     if (subset == "train") {
//         data = get_train();
//     } else if (subset == "test") {
//         data = get_test();
//     }
// }


vector<vector<int>> CSV::read_csv(string filename, int rows) {
    cout << "read_csv called" << endl;
    cout << "checkpoint 1" << endl;

    ifstream file(filename);
    cout << "checkpoint 2" << endl;
    string line;
    cout << "checkpoint 3" << endl;
    vector<vector<int>> data;
    cout << "checkpoint 4" << endl;
    int i = 0;
    cout << "checkpoint 5" << endl;
    while (getline(file, line) && i < rows) {
        stringstream ss(line);
        string cell;
        vector<int> row;
        while (getline(ss, cell, DELIM)) {
            row.push_back(stoi(cell));
        }
        data.push_back(row);
        i++;
    }
    cout << "read_csv finished" << endl;
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

CSV::CSV(string subset) {
    cout << "CSV constructor called" << endl;
    if (subset == "train") {
        data = read_csv(TRAIN_FILEPATH, get_rows(TRAIN_FILEPATH));
    } else if (subset == "test") {
        data = read_csv(TEST_FILEPATH, get_rows(TEST_FILEPATH));
    }
    cout << "CSV constructor finished" << endl;
}