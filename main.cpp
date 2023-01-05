
#include <iostream>
#include <string>
#include "csv.h"
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <unordered_map>
#include <chrono>
using namespace std;

template <typename T>
void print(T t) {
    cout << t << endl;
}

vector<vector<float>> get_user_item_matrix(vector<vector<int>> data, int num_users, int num_items) {
    vector<vector<float>> pivoted_data(num_users, vector<float>(num_items, 0));
    for (int i = 0; i < data.size(); i++) {
        int user = data[i][0];
        int item = data[i][1];
        int rating = data[i][2];
        pivoted_data[user][item] = rating / 2.0;
    }
    return pivoted_data;
}

vector<vector<float>> get_item_user_matrix(vector<vector<int>> data, int num_users, int num_items) {
    vector<vector<float>> pivoted_data(num_items, vector<float>(num_users, 0));
    for (int i = 0; i < data.size(); i++) {
        int user = data[i][0];
        int item = data[i][1];
        int rating = data[i][2];
        pivoted_data[item][user] = rating / 2.0;
    }
    return pivoted_data;
}

// calculate cosine user_similarity_matrix (num_user x num_user) using unordered_map structure
unordered_map<int, unordered_map<int, float>> get_user_similarity_matrix(vector<vector<float>> pivoted_data) {
    unordered_map<int, unordered_map<int, float>> user_similarity_matrix;
    for (int i = 0; i < pivoted_data.size(); i++) {
        for (int j = i + 1; j < pivoted_data.size(); j++) {
            float dot_product = 0;
            float norm_i = 0;
            float norm_j = 0;
            for (int k = 0; k < pivoted_data[i].size(); k++) {
                dot_product += pivoted_data[i][k] * pivoted_data[j][k];
                norm_i += pivoted_data[i][k] * pivoted_data[i][k];
                norm_j += pivoted_data[j][k] * pivoted_data[j][k];
            }
            float similarity = dot_product / (sqrt(norm_i) * sqrt(norm_j));
            user_similarity_matrix[i][j] = similarity;
            user_similarity_matrix[j][i] = similarity;
        }
        if (i % 100 == 0) {
            cout << "Finished " << i << " users" << endl;
        }
    }
}

float cosine_similarity(vector<float> v1, vector<float> v2) {
    float dot_product = 0;
    float norm_i = 0;
    float norm_j = 0;
    for (int k = 0; k < v1.size(); k++) {
        dot_product += v1[k] * v2[k];
        norm_i += v1[k] * v1[k];
        norm_j += v2[k] * v2[k];
    }
    float similarity = dot_product / (sqrt(norm_i) * sqrt(norm_j));
    return similarity;
}



int main () {
    CSV train = CSV("train");
    CSV test = CSV("test");

    int num_users = train.get_unique_users();
    int num_items = train.get_unique_items();


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

    for (int i = 0; i < train.data.size(); i++) {
        train.data[i][0] = user_id_map[train.data[i][0]];
        train.data[i][1] = item_id_map[train.data[i][1]];
    }
    for (int i = 0; i < test.data.size(); i++) {
        test.data[i][1] = user_id_map[test.data[i][1]];
        test.data[i][2] = item_id_map[test.data[i][2]];
    }

    print(train.data.size());
    print(test.data.size());

    vector<vector<float>> user_item_matrix = get_user_item_matrix(train.data, num_users, num_items);
    vector<vector<float>> item_user_matrix = get_item_user_matrix(train.data, num_users, num_items);


    cout << "Starting user based prediction" << endl;
    auto start = chrono::high_resolution_clock::now();
    vector<float> ubcf_predictions;
    for (int i = 0; i < test.data.size(); i++) {
        if (i % 100 == 0) {
            cout << "Finished " << i << " predictions" << endl;
        }
        int user = test.data[i][1];
        int item = test.data[i][2];
        float prediction = 0;

        // calculate that similarity between the user and all other users
        vector<float> similarities;
        for (int j = 0; j < user_item_matrix.size(); j++) {

            if (user_item_matrix[j][item] != 0) {
                float similarity = cosine_similarity(user_item_matrix[user], user_item_matrix[j]);
                similarities.push_back(similarity);
            } else {
                similarities.push_back(0);
            }
        }  

        // sort the similarities
        sort(similarities.begin(), similarities.end(), greater<float>());

        // get the top 10 similarities
        vector<float> top_similarities;
        for (int j = 0; j < 10; j++) {
            top_similarities.push_back(similarities[j]);
        }

        // average the top 10 similarities
        float sum = 0;
        for (int j = 0; j < top_similarities.size(); j++) {
            sum += top_similarities[j];
        }

        float average = sum / top_similarities.size();

        ubcf_predictions.push_back(average);
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Time taken by function: " << duration.count() / 1000.0 << " seconds" << endl;


    cout << "Starting item based prediction" << endl;
    start = chrono::high_resolution_clock::now();
    vector<float> ibcf_predictions;
    for (int i = 0; i < test.data.size(); i++) {
        
        if (i % 100 == 0) {
            cout << "Finished " << i << " predictions" << endl;
        }

        int user = test.data[i][1];
        int item = test.data[i][2];
        float prediction = 0;

        // calculate that similarity between the user and all other users
        vector<float> similarities;
        for (int j = 0; j < item_user_matrix.size(); j++) {

            if (item_user_matrix[j][user] != 0) {
                float similarity = cosine_similarity(item_user_matrix[item], item_user_matrix[j]);
                similarities.push_back(similarity);
            } else {
                similarities.push_back(0);
            }
        }  

        // sort the similarities
        sort(similarities.begin(), similarities.end(), greater<float>());

        // get the top 10 similarities
        vector<float> top_similarities;
        for (int j = 0; j < 10; j++) {
            top_similarities.push_back(similarities[j]);
        }

        // average the top 10 similarities
        float sum = 0;
        for (int j = 0; j < top_similarities.size(); j++) {
            sum += top_similarities[j];
        }

        float average = sum / top_similarities.size();

        ibcf_predictions.push_back(average);
    }

    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "Time taken by function: " << duration.count() / 1000.0 << " seconds" << endl;


    // write the predictions to a file
    print("Writing the result to a file...\n");
    ofstream fout3("submission.csv");
    fout3 << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        float final_prediction = (ubcf_predictions[i] + ibcf_predictions[i]) / 2.0;
        // final_prediction = llround(final_prediction * 2) / 2.0;
        fout3 << test.data[i][0] << "," << to_string(final_prediction) << endl;
    }

    return 0;

}