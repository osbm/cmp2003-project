
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




int main () {
    CSV train = CSV("train");
    
    int num_users = train.get_unique_users();
    int num_items = train.get_unique_items();

    cout << "This program takes about 2 minutes to run."<< endl;

    // find top 10 most rated items along with their counts
    vector<pair<int, int>> item_counts;
    for (int i = 0; i < num_items; i++) {
        item_counts.push_back(make_pair(i, train.get_item_count(i)));
    }

    sort(item_counts.begin(), item_counts.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second > b.second;
    });

    // print top 10 most rated items
    cout << "Top 10 most rated items:" << endl;
    for (int i = 0; i < 10; i++) {
        cout << item_counts[i].first << "\t"<< item_counts[i].second << endl;
    }

    // find top 10 most active users along with their counts
    vector<pair<int, int>> user_counts;
    for (int i = 0; i < num_users; i++) {
        user_counts.push_back(make_pair(i, train.get_user_count(i)));
    }

    sort(user_counts.begin(), user_counts.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second > b.second;
    });

    cout << endl;

    // print top 10 most active users
    cout << "Top 10 most active users:" << endl;
    for (int i = 0; i < 10; i++) {
        cout << user_counts[i].first << "\t"<< user_counts[i].second << endl;
    }

}
