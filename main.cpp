
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


vector<vector<float>> apply_cosine_similarity(vector<vector<int>> user_item_matrix) {
    int num_users = user_item_matrix.size();
    vector<vector<float>> similarity_matrix(num_users, vector<float>(num_users, 0));
    for (int i = 0; i < num_users; i++) {
        for (int j = 0; j < num_users; j++) {
            similarity_matrix[i][j] = cosine_similarity(user_item_matrix[i], user_item_matrix[j]);
        }
        if (i % 100 == 0) {
            cout << "Done with " << i << " users" << endl;
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
    
    
    // now we can create a user-item matrix

    vector<vector<int>> user_item_matrix = get_user_item_matrix(train.data, total_unique_users, total_unique_items);
    printf("User-item matrix created\n");
    vector<vector<float>> user_similarity = apply_cosine_similarity(user_item_matrix);
    printf("User similarity matrix created\n");
    
    vector<float> predicted_ratings(test.data.size(), 0);

    // start the inference
    for (int i = 0; i < test.data.size(); i++) {
        int user_id = test.data[i][1];
        int item_id = test.data[i][2];
        float rating = 0;
        int count = 0;
        vector<float> similarity_scores = user_similarity[user_id];
        vector<int> movie_ratings = user_item_matrix[item_id];
        // get the indices of the movies that have been rated by the user
        vector<int> rated_movie_indices;
        for (int j = 0; j < movie_ratings.size(); j++) {
            if (movie_ratings[j] != 0) {
                rated_movie_indices.push_back(j);
            }
        }

        // get the similarity scores of the movies that have been rated by the user
        vector<float> similarity_scores_of_rated_movies;
        for (int j = 0; j < rated_movie_indices.size(); j++) {
            similarity_scores_of_rated_movies.push_back(similarity_scores[rated_movie_indices[j]]);
        }

        // get the movie ratings of the movies that have been rated by the user
        vector<int> movie_ratings_of_rated_movies;
        for (int j = 0; j < rated_movie_indices.size(); j++) {
            movie_ratings_of_rated_movies.push_back(movie_ratings[rated_movie_indices[j]]);
        }

        // calculate the weighted average of the movie ratings
        // dot (similatiry_scores_of_rated_movies, movie_ratings_of_rated_movies) / sum(similarity_scores_of_rated_movies)
        float numerator = 0;
        float denominator = 0;
        for (int j = 0; j < similarity_scores_of_rated_movies.size(); j++) {
            numerator += similarity_scores_of_rated_movies[j] * movie_ratings_of_rated_movies[j];
            denominator += similarity_scores_of_rated_movies[j];
        }
        if (denominator != 0) {
            rating = numerator / denominator;
        }
        predicted_ratings[i] = rating;



    }

    // write the result to a file
    ofstream fout("submission.csv");
    fout << "Id,Predicted" << endl;
    for (int i = 0; i < test.data.size(); i++) {
        fout << test.data[i][0] << "," << predicted_ratings[i] / 2.0 << endl;
    }

    return 0;

}
