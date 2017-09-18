#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <vector>

using namespace std;

namespace utils {
    /**
      * This function was designed to work on roulette selection algorithm
      * @param guess - the guess value that you want to find in list
      * @param list - sorted list of elements
      * @param compare function - to compare two elements of list
      * @return The closest element to the guess on the list
      */
    template <typename T>
    int const binarySearch(T const guess, vector<T> const list, int(*compar)(const void *, const void *)) {

        int begin = 0, end = list.size() - 1, mid = (begin + end) / 2;

        while ( !(mid == end || mid == begin) ) {
            // cout << "begin: " << begin << " mid: " << mid << " end: " << end << endl;
            if ( compar(&list[mid], &guess) == 0 ) { // list[mid] == guess
                begin = end = mid;
            } else if ( compar(&guess, &list[mid]) > 0 ) { // guess > list[mid]
                begin = mid;
            } else {
                end = mid;
            }
            mid = (begin + end) / 2;
        }

        //cout << "FINAL: begin: " << begin << " mid: " << mid << " end: " << end << endl;
        
        if ( compar(&guess, &list[mid]) > 0 ) return end; // guess > list[mid]
        else if ( compar(&guess, &list[mid]) < 0 ) return begin; // guess < list[mid]
        else return mid;

    }
    /**
      * find the target on a list based on comparasion function compar
      * @param target - value that we're looking for
      * @param list - list of elements where we wanna find the element
      * @param compare function - to compare two elements of list
      * @return the target index on list if exists, -1 otherwise
      */
    template <typename T>
    int const find(T const target, vector<T> const list, int(*compar)(const void *, const void *)) {
        int found = -1;
        for (int i = 0; i < list.size() && found == -1; i++) {
            if ( compar(target, list[i]) == 0 ) {
                found = i;
            }
        }
        return found;
    }

    // statistics
    double const avg(vector<int> v) {
        double sum = 0.0;
        for (int i = 0; i < v.size(); ++i) {
            sum+= v[i];
        }
        return sum/v.size();
    }

    double const median(vector<int> v) {
        return 0.0;
    }

}

#endif //UTILS_H
