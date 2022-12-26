
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>


using namespace std;

// lets write a print function to print everything like python
template <typename T>
void print(T t) {
    cout << t << endl;
}

class CSV {
    private:
        
        static const char DELIM = ',';
        
    public:
        const string TRAIN_FILEPATH = "data/train.csv";
        const string TEST_FILEPATH = "data/test.csv";
        // array of column names
        vector<string> col_names;
        vector<vector<int>> data;
        vector<vector<int>> read_csv(string filename) {
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
                    row.push_back(stoi(cell)*2);
                }

                data.push_back(row);
            }

            return data;
        }
        int save_csv(string filename, vector<vector<int>> data) const {
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
        int get_rows(string filename) {
            ifstream file(filename);
            string line;
            int rows = 0;
            while (getline(file, line)) {
                rows++;
            }
            return rows;
        }
        int get_cols(string filename) {
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
        int get_unique_users() {
            vector<int> users;
            for (int i = 0; i < data.size(); i++) {
                users.push_back(data[i][0]);
            }
            sort(users.begin(), users.end());
            users.erase(unique(users.begin(), users.end()), users.end());
            return users.size();
        }
        int get_unique_items() {
            vector<int> items;
            for (int i = 0; i < data.size(); i++) {
                items.push_back(data[i][1]);
            }
            sort(items.begin(), items.end());
            items.erase(unique(items.begin(), items.end()), items.end());
            return items.size();
        }
        // void reset_ids();
        CSV(string subset) {
            if (subset == "train") {
                data = read_csv(TRAIN_FILEPATH);
            } else if (subset == "test") {
                data = read_csv(TEST_FILEPATH);
            } else {
                cout << "Invalid subset" << endl;
            }
        }
        
};

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
    
    int total_unique_users = train.get_unique_users();
    int total_unique_items = train.get_unique_items();

    // before we start, we need to reset the IDs of the users and movies
    // so that they are continuous and start from 0
    // create a mapping from old user id to new user id
    vector<int> user_id_map(total_unique_users, 0);
    vector<int> item_id_map(total_unique_items, 0);
    int new_user_id = 0;
    int new_item_id = 0;
    for (int i = 0; i < train.data.size(); i++) {
        int old_user_id = train.data[i][0];
        if (user_id_map[old_user_id] == 0) {
            user_id_map[old_user_id] = new_user_id;
            new_user_id++;
        }
        int old_item_id = train.data[i][1];
        if (item_id_map[old_item_id] == 0) {
            item_id_map[old_item_id] = new_item_id;
            new_item_id++;
        }
    }

    print("Done creating mappings");

    // now we can reset the IDs
    for (int i = 0; i < train.data.size(); i++) {
        int old_user_id = train.data[i][0];
        int old_item_id = train.data[i][1];
        train.data[i][0] = user_id_map[old_user_id];
        train.data[i][1] = item_id_map[old_item_id];
    }
    print("Done resetting IDs for train data");

    for (int i = 0; i < test.data.size(); i++) {
        int old_user_id = test.data[i][0];
        int old_item_id = test.data[i][1];
        test.data[i][0] = user_id_map[old_user_id];
        test.data[i][1] = item_id_map[old_item_id];
    }

    print("Done resetting IDs for test data");    
    
    return 0;
    // why program raises segmentation fault? even though everything has worked

}