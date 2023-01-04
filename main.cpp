
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

vector<vector<double>> get_user_item_matrix(vector<vector<int>> data, int num_users, int num_items) {
    vector<vector<double>> user_item_matrix(num_users, vector<double>(num_items, 0));
    for (int i = 0; i < data.size(); i++) {
        int user = data[i][0];
        int item = data[i][1];
        int rating = data[i][2];
        user_item_matrix[user][item] = rating / 2.0;
    }
    return user_item_matrix;
}

vector<vector<double>> get_item_user_matrix(vector<vector<int>> data, int num_users, int num_items) {
    vector<vector<double>> item_user_matrix(num_items, vector<double>(num_users, 0));
    for (int i = 0; i < data.size(); i++) {
        int user = data[i][0];
        int item = data[i][1];
        int rating = data[i][2];
        item_user_matrix[item][user] = rating / 2.0;
    }
    return item_user_matrix;
}

double cosine_similarity(vector<double> v1, vector<double> v2) {
    double dot_product = 0;
    double norm1 = 0;
    double norm2 = 0;
    for (int i = 0; i < v1.size(); i++) {
        dot_product += v1[i] * v2[i];
        norm1 += v1[i] * v1[i];
        norm2 += v2[i] * v2[i];
    }
    return dot_product / (sqrt(norm1) * sqrt(norm2));
}

double vector_magnitude(vector<double>& vec) {
    double sum = 0;
    for (int i : vec) {
        sum += i * i;
    }
    return sqrt(sum);
}

double dot_product(vector<double>& vec1, vector<double>& vec2) {
    double sum = 0;
    for (int i = 0; i < vec1.size(); i++) {
        sum += vec1[i] * vec2[i];
    }
    return sum;
}


vector<vector<double>> apply_cosine_similarity(vector<vector<double>> user_item_matrix) {
    int num_users = user_item_matrix.size();
    vector<vector<double>> similarity_matrix(num_users, vector<double>(num_users, 0));
    for (int i = 0; i < num_users; i++) {
        for (int j = 0; j < num_users; j++) {
            similarity_matrix[i][j] = cosine_similarity(user_item_matrix[i], user_item_matrix[j]);
        }
        if (i % 100 == 0) {
            cout << "Done with " << i / (double) num_users << " users" << endl;
        }
    }   
    return similarity_matrix;
}

int main() {
    CSV train("train");
    CSV test("test");
    
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

    vector<vector<double>> user_item_matrix = get_user_item_matrix(train.data, total_unique_users, total_unique_items);
    vector<vector<double>> item_user_matrix = get_item_user_matrix(train.data, total_unique_users, total_unique_items);

    vector<vector<double>> user_similarity = apply_cosine_similarity(user_item_matrix);
    vector<vector<double>> item_similarity = apply_cosine_similarity(item_user_matrix);
    
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


    vector<double> ubcf_ratings(test.data.size(), 0);
    for (int i = 0; i < test.data.size(); i++) {
        int user_id = test.data[i][1];
        int item_id = test.data[i][2];
        double user_based_rating = 0;
        vector<double> user_similarity_scores = user_similarity[user_id];
        vector<double> item_ratings = user_item_matrix[item_id];
        vector<int> rated_item_indices;
        for (int j = 0; j < item_ratings.size(); j++) {
            if (item_ratings[j] != 0) {
                rated_item_indices.push_back(j);
            }
        }
        vector<double> similarity_scores_of_rated_items;
        for (int j = 0; j < rated_item_indices.size(); j++) {
            similarity_scores_of_rated_items.push_back(user_similarity_scores[rated_item_indices[j]]);
        }
        vector<int> item_ratings_of_rated_items;
        for (int j = 0; j < rated_item_indices.size(); j++) {
            item_ratings_of_rated_items.push_back(item_ratings[rated_item_indices[j]]);
        }
        double numerator = 0;
        double denominator = 0;
        for (int j = 0; j < similarity_scores_of_rated_items.size(); j++) {
            numerator += similarity_scores_of_rated_items[j] * item_ratings_of_rated_items[j];
            denominator += similarity_scores_of_rated_items[j];
        }
        if (denominator != 0) {
            user_based_rating = numerator / denominator;
        } else {
            user_based_rating = 4.0;
        }

        ubcf_ratings[i] = user_based_rating;
    }

    printf("Writing the result to a file...\n");
    ofstream fout2("ubcf.csv");
    fout2 << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        fout2 << test.data[i][0] << "," << ubcf_ratings[i] << endl;
    }

    vector<double> ibcf_ratings(test.data.size(), 0);
    for (int i = 0; i < 5000; i++) {
     
        int user_id = test.data[i][1];
        int item_id = test.data[i][2];
        
        vector<double> item_similarity_scores = item_similarity[item_id];
        // we are subtracting 1 because the item ids start from 1
        if (item_similarity_scores.size() == 0) {
            print("item_similarity_scores.size() == 0");
            print("user_id: " + to_string(user_id));
            print("item_id: " + to_string(item_id));
            continue;
        }
        // select only the items that the user has rated
        vector<double> user_ratings = user_item_matrix[user_id];
        // print(user_ratings.size());
        vector<int> rated_item_indices;
        for (int j = 0; j < user_ratings.size(); j++) {
            if (user_ratings[j] != 0) {
                rated_item_indices.push_back(j);
            }
        }
        // print(rated_item_indices.size());
        vector<double> similarity_scores_of_rated_items;
        for (int j = 0; j < rated_item_indices.size(); j++) {
            similarity_scores_of_rated_items.push_back(item_similarity_scores[rated_item_indices[j]]);
        }
        // print(similarity_scores_of_rated_items.size());
        vector<int> item_ratings_of_rated_items;
        for (int j = 0; j < rated_item_indices.size(); j++) {
            item_ratings_of_rated_items.push_back(user_ratings[rated_item_indices[j]]);
        }
        // print(item_ratings_of_rated_items.size());
        double numerator = 0;
        double denominator = 0;
        for (int j = 0; j < similarity_scores_of_rated_items.size(); j++) {
            numerator += similarity_scores_of_rated_items[j] * item_ratings_of_rated_items[j];
            denominator += similarity_scores_of_rated_items[j];
        }
        
        double item_based_rating = 0;
        if (denominator != 0) {
            item_based_rating = numerator / denominator;
        } else {
            print("denominator == 0");
            item_based_rating = 4.0;
        }

        ibcf_ratings[i] = item_based_rating;
        
    }

    printf("Writing the result to a file...\n");
    ofstream fout("ibcf.csv");
    fout << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        fout << test.data[i][0] << "," << ibcf_ratings[i]<< endl;
    }

    printf("Writing the result to a file...\n");
    ofstream fout3("submission.csv");
    fout3 << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        double final_prediction = (ubcf_ratings[i] + ibcf_ratings[i]) / 2.0;
        // round the final prediction to the nearest half integer
        // final_prediction = llround(final_prediction * 2) / 2.0;
        fout3 << test.data[i][0] << "," << to_string(final_prediction) << endl;
    }

    return 0;

}
