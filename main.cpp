
#include <iostream>
#include <string>
#include "csv.h"
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <unordered_map>

using namespace std;

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

float vector_magnitude(vector<int>& vec) {
    float sum = 0;
    for (int i : vec) {
        sum += i * i;
    }
    return sqrt(sum);
}

float dot_product(vector<int>& vec1, vector<int>& vec2) {
    float sum = 0;
    for (int i = 0; i < vec1.size(); i++) {
        sum += vec1[i] * vec2[i];
    }
    return sum;
}


vector<vector<float>> apply_cosine_similarity(vector<vector<int>>& matrix) {
    int size = matrix.size();
    vector<vector<float>> similarity_matrix(size, vector<float>(size, 0));
    unordered_map<int, float> dot_products;
    unordered_map<int, float> magnitudes;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j) similarity_matrix[i][j] = 1;
            if (dot_products.count(i) == 0) {
                dot_products[i] = dot_product(matrix[i], matrix[i]);
                magnitudes[i] = vector_magnitude(matrix[i]);
            }
            if (dot_products.count(j) == 0) {
                dot_products[j] = dot_product(matrix[j], matrix[j]);
                magnitudes[j] = vector_magnitude(matrix[j]);
            }
            similarity_matrix[i][j] = dot_products[i] / (magnitudes[i] * magnitudes[j]);
        }
        if (i % 200 == 0) {
            cout << "Progress: " << i / (float)size << endl;
        }
    }   
    return similarity_matrix;
}


int main() {
    CSV train("train");
    CSV test(string("test"));
    
    int total_unique_users = train.get_unique_users();
    int total_unique_items = train.get_unique_items();

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

    vector<vector<int>> user_item_matrix = get_user_item_matrix(train.data, total_unique_users, total_unique_items);
    vector<vector<int>> item_user_matrix = get_item_user_matrix(train.data, total_unique_users, total_unique_items);
    vector<vector<float>> user_similarity = apply_cosine_similarity(user_item_matrix);
    vector<vector<float>> item_similarity = apply_cosine_similarity(item_user_matrix);
    
    // print 10 by 10 portion of user similarity matrix and item similarity matrix
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            cout << user_similarity[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            cout << item_similarity[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    // print shapes of user similarity matrix and item similarity matrix
    cout << "user_similarity.shape: " << user_similarity.size() << " " << user_similarity[0].size() << endl;
    cout << "item_similarity.shape: " << item_similarity.size() << " " << item_similarity[0].size() << endl;


    vector<float> ubcf_ratings(test.data.size(), 0);
    for (int i = 0; i < test.data.size(); i++) {
        int user_id = test.data[i][1];
        int item_id = test.data[i][2];
        float user_based_rating = 0;
        vector<float> user_similarity_scores = user_similarity[user_id];
        vector<int> item_ratings = user_item_matrix[item_id];
        vector<int> rated_item_indices;
        for (int j = 0; j < item_ratings.size(); j++) {
            if (item_ratings[j] != 0) {
                rated_item_indices.push_back(j);
            }
        }
        vector<float> similarity_scores_of_rated_items;
        for (int j = 0; j < rated_item_indices.size(); j++) {
            similarity_scores_of_rated_items.push_back(user_similarity_scores[rated_item_indices[j]]);
        }
        vector<int> item_ratings_of_rated_items;
        for (int j = 0; j < rated_item_indices.size(); j++) {
            item_ratings_of_rated_items.push_back(item_ratings[rated_item_indices[j]]);
        }
        float numerator = 0;
        float denominator = 0;
        for (int j = 0; j < similarity_scores_of_rated_items.size(); j++) {
            numerator += similarity_scores_of_rated_items[j] * item_ratings_of_rated_items[j];
            denominator += similarity_scores_of_rated_items[j];
        }
        if (denominator != 0) {
            user_based_rating = numerator / denominator;
        }
        ubcf_ratings[i] = user_based_rating;
    }

    printf("Writing the result to a file...\n");
    ofstream fout2("ubcf.csv");
    fout2 << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        fout2 << test.data[i][0] << "," << ubcf_ratings[i] / 2.0 << endl;
    }

    vector<float> ibcf_ratings(test.data.size(), 0);
    for (int i = 0; i < 5000; i++) {
     
        int user_id = test.data[i][1];
        int item_id = test.data[i][2];
        
        vector<float> item_similarity_scores = item_similarity[item_id];
        // we are subtracting 1 because the item ids start from 1
        if (item_similarity_scores.size() == 0) {
            print("item_similarity_scores.size() == 0");
            print("user_id: " + to_string(user_id));
            print("item_id: " + to_string(item_id));
            continue;
        }
        // select only the items that the user has rated
        vector<int> user_ratings = user_item_matrix[user_id];
        // print(user_ratings.size());
        vector<int> rated_item_indices;
        for (int j = 0; j < user_ratings.size(); j++) {
            if (user_ratings[j] != 0) {
                rated_item_indices.push_back(j);
            }
        }
        // print(rated_item_indices.size());
        vector<float> similarity_scores_of_rated_items;
        for (int j = 0; j < rated_item_indices.size(); j++) {
            similarity_scores_of_rated_items.push_back(item_similarity_scores[rated_item_indices[j]]);
        }
        // print(similarity_scores_of_rated_items.size());
        vector<int> item_ratings_of_rated_items;
        for (int j = 0; j < rated_item_indices.size(); j++) {
            item_ratings_of_rated_items.push_back(user_ratings[rated_item_indices[j]]);
        }
        // print(item_ratings_of_rated_items.size());
        float numerator = 0;
        float denominator = 0;
        for (int j = 0; j < similarity_scores_of_rated_items.size(); j++) {
            numerator += similarity_scores_of_rated_items[j] * item_ratings_of_rated_items[j];
            denominator += similarity_scores_of_rated_items[j];
        }
        
        float item_based_rating = 0;
        if (denominator != 0) {
            item_based_rating = numerator / denominator;
        } else {
            print("denominator == 0");
            item_based_rating = 10;
        }

        ibcf_ratings[i] = item_based_rating;
        


        
    }

    printf("Writing the result to a file...\n");
    ofstream fout("ibcf.csv");
    fout << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        fout << test.data[i][0] << "," << ibcf_ratings[i] / 2.0 << endl;
    }

    printf("Writing the result to a file...\n");
    ofstream fout3("submission.csv");
    fout3 << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        fout3 << test.data[i][0] << "," << (ubcf_ratings[i] + ibcf_ratings[i]) / 4.0 << endl;
    }

    return 0;

}
