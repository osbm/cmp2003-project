
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
    vector<vector<int>> item_user_matrix = get_item_user_matrix(train.data, total_unique_users, total_unique_items);
    
    vector<vector<float>> user_similarity = apply_cosine_similarity(user_item_matrix);
    vector<vector<float>> item_similarity = apply_cosine_similarity(item_user_matrix);

    printf("Similarity matrices created\n");
    
    vector<float> predicted_ratings(test.data.size(), 0);


    // start the inference
    printf("Starting inference\n");
    for (int i = 0; i < test.data.size(); i++) {

        if (i % 100 == 0) {
            cout << "Progress: " << i / 5000.0 << endl;
        }
        printf("error catcher 1\n");

        int user_id = test.data[i][1];
        int item_id = test.data[i][2];
        printf("error catcher 2\n");
        vector<float> user_similarity_scores = user_similarity[user_id - 1];
        vector<float> item_similarity_scores = item_similarity[item_id - 1];
        printf("error catcher 3\n");

        vector<int> item_ratings = user_item_matrix[item_id];
        vector<int> user_ratings = item_user_matrix[user_id];
        printf("error catcher 4\n");

        printf("error catcher 5\n");
        // get rated user_ids and rated item_ids
        vector<int> rated_user_ids;
        vector<int> rated_item_ids;
        printf("error catcher 6\n");

        for (int j = 0; j < item_ratings.size(); j++) {
            if (item_ratings[j] != 0) {
                rated_user_ids.push_back(j);
            }
        }

        for (int j = 0; j < user_ratings.size(); j++) {
            if (user_ratings[j] != 0) {
                rated_item_ids.push_back(j);
            }
        }

        printf("error catcher 7");
        // get the similarity scores of the rated users and rated items
        vector<float> rated_user_similarity_scores;
        vector<float> rated_item_similarity_scores;

        for (int j = 0; j < rated_user_ids.size(); j++) {
            rated_user_similarity_scores.push_back(user_similarity_scores[rated_user_ids[j]]);
        }

        for (int j = 0; j < rated_item_ids.size(); j++) {
            rated_item_similarity_scores.push_back(item_similarity_scores[rated_item_ids[j]]);
        }

        printf("error catcher 8");
        // get the ratings of the rated users and rated items

        vector<int> user_ratings_of_rated_users;
        vector<int> item_ratings_of_rated_items;

        for (int j = 0; j < rated_user_ids.size(); j++) {
            user_ratings_of_rated_users.push_back(user_ratings[rated_user_ids[j]]);
        }

        for (int j = 0; j < rated_item_ids.size(); j++) {
            item_ratings_of_rated_items.push_back(item_ratings[rated_item_ids[j]]);
        }
        printf("error catcher 9");
        // calculate the weighted average of the ratings of the rated users and rated items
        float user_based = 0;
        float item_based = 0;

        float user_based_denominator = 0;
        float item_based_denominator = 0;

        for (int j = 0; j < rated_user_ids.size(); j++) {
            user_based += rated_user_similarity_scores[j] * user_ratings_of_rated_users[j];
            user_based_denominator += rated_user_similarity_scores[j];
        }

        for (int j = 0; j < rated_item_ids.size(); j++) {
            item_based += rated_item_similarity_scores[j] * item_ratings_of_rated_items[j];
            item_based_denominator += rated_item_similarity_scores[j];
        }

        user_based /= user_based_denominator;
        item_based /= item_based_denominator;
        printf("error catcher 10");

        float rating = (user_based + item_based) / 2.0;

        predicted_ratings[i] = rating;
    }
    printf("Inference complete\n");
    // write the result to a file
    ofstream fout("submission.csv");
    fout << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        fout << test.data[i][0] << "," << predicted_ratings[i] / 2.0 << endl;
    }

    return 0;

}
