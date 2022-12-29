
#include <iostream>
#include <string>
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

vector<vector<int>> get_item_user_matrix(vector<vector<int>> data, int num_users, int num_items) {
    vector<vector<int>> item_user_matrix(num_items, vector<int>(num_users, 0));
    for (int i = 0; i < data.size(); i++) {
        int user = data[i][0];
        int item = data[i][1];
        int rating = data[i][2];
        item_user_matrix[item][user] = rating;
    }
    return item_user_matrix;
}

// write a cosine similarity function that takes in two vectors and returns the cosine similarity

float cosine_similarity(vector<int> v1, vector<int> v2) {
    float dot_product = 0;
    float norm1 = 0;
    float norm2 = 0;
    for (int i = 0; i < v1.size(); i++) {
        dot_product += v1[i] * v2[i];
        norm1 += v1[i] * v1[i];
        norm2 += v2[i] * v2[i];
    }
    return dot_product / (sqrt(norm1) * sqrt(norm2));
}

float pearson_similarity(vector<int> v1, vector<int> v2) {
    float mean1 = 0;
    float mean2 = 0;
    for (int i = 0; i < v1.size(); i++) {
        mean1 += v1[i];
        mean2 += v2[i];
    }
    mean1 /= v1.size();
    mean2 /= v2.size();
    float numerator = 0;
    float denominator1 = 0;
    float denominator2 = 0;
    for (int i = 0; i < v1.size(); i++) {
        numerator += (v1[i] - mean1) * (v2[i] - mean2);
        denominator1 += (v1[i] - mean1) * (v1[i] - mean1);
        denominator2 += (v2[i] - mean2) * (v2[i] - mean2);
    }
    return numerator / (sqrt(denominator1) * sqrt(denominator2));
}

// a third similarity function that takes in two vectors
// and returns the jaccard similarity
float jaccard_similarity(vector<int> v1, vector<int> v2) {
    int intersection = 0;
    int union_set = 0;
    for (int i = 0; i < v1.size(); i++) {
        if (v1[i] != 0 && v2[i] != 0) {
            intersection++;
        }
        if (v1[i] != 0 || v2[i] != 0) {
            union_set++;
        }
    }
    return (float) intersection / union_set;
}

vector<vector<float>> apply_cosine_similarity(vector<vector<int>> user_item_matrix) {
    int num_users = user_item_matrix.size();
    vector<vector<float>> similarity_matrix(num_users, vector<float>(num_users, 0));
    for (int i = 0; i < num_users; i++) {
        for (int j = 0; j < num_users; j++) {
            similarity_matrix[i][j] = cosine_similarity(user_item_matrix[i], user_item_matrix[j]);
        }
        if (i % 100 == 0) {
            cout << "Progress: " << i / num_users << endl;
        }
    }   
    return similarity_matrix;
}


int main() {
    CSV train("train");
    CSV test(string("test"));
    
    // print  first 4 rows of train data
    cout << "First 4 rows of train data: " << endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < train.data[i].size(); j++) {
            cout << train.data[i][j] << ", ";
        }
        cout << endl;
    }

    // print the first 4 rows of test data
    cout << "First 4 rows of test data: " << endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < test.data[i].size(); j++) {
            cout << test.data[i][j] << ", ";
        }
        cout << endl;
    }

    int total_unique_users = train.get_unique_users();
    int total_unique_items = train.get_unique_items();

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
        test.data[i][1] = user_id_map[test.data[i][1]];
        test.data[i][2] = item_id_map[test.data[i][2]];
    }
    printf("IDs reset\n");

    // now we can create a user-item matrix
    vector<vector<int>> user_item_matrix = get_user_item_matrix(train.data, total_unique_users, total_unique_items);
    printf("User-item matrix created\n");

    vector<vector<float>> user_similarity = apply_cosine_similarity(user_item_matrix);
    printf("User similarity matrix created\n");
    
    vector<float> predicted_ratings(test.data.size(), 0);

    // create a item user matrix
    vector<vector<int>> item_user_matrix = get_item_user_matrix(train.data, total_unique_users, total_unique_items);
    printf("Item-user matrix created\n");

    vector<vector<float>> item_similarity = apply_cosine_similarity(item_user_matrix);

    

    // start the inference
    for (int i = 0; i < test.data.size(); i++) {
        int user_id = test.data[i][1];
        int item_id = test.data[i][2];
        float rating = 0;
        vector<float> user_similarity_scores = user_similarity[user_id];
        vector<float> item_similarity_scores = item_similarity[item_id];

        vector<int> movie_ratings = user_item_matrix[item_id];
        vector<int> user_ratings = item_user_matrix[user_id];
        
        // calculate user based collaborative filtering and item based collaborative filtering ensemble
        float user_based_cf = 0;
        float item_based_cf = 0;
        float user_based_cf_count = 0;
        float item_based_cf_count = 0;

        for (int j = 0; j < user_similarity_scores.size(); j++) {
            if (user_similarity_scores[j] > 0 && user_ratings[j] != 0) {
                user_based_cf += user_similarity_scores[j] * user_ratings[j];
                user_based_cf_count += user_similarity_scores[j];
            }
        }

        for (int j = 0; j < item_similarity_scores.size(); j++) {
            if (item_similarity_scores[j] > 0 && movie_ratings[j] != 0) {
                item_based_cf += item_similarity_scores[j] * movie_ratings[j];
                item_based_cf_count += item_similarity_scores[j];
            }
        }

        if (user_based_cf_count > 0) {
            user_based_cf /= user_based_cf_count;
        }

        if (item_based_cf_count > 0) {
            item_based_cf /= item_based_cf_count;
        }

        rating = (user_based_cf + item_based_cf) / 2.0;
    }

    // write the result to a file
    ofstream fout("submission.csv");
    fout << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        fout << test.data[i][0] << "," << predicted_ratings[i] / 2.0 << endl;
    }

    return 0;

}
