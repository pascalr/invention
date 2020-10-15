#ifndef _NEAR_DUPLICATES_H
#define _NEAR_DUPLICATES_H

#include <vector>

// Inefficient algorithm when n is large, but in my case n is small. O(n^2) I believe.
template<typename T>
std::vector<std::vector<size_t>> groupNearDuplicates(std::vector<T> items, std::function<bool(T, T)> cmpFunction) {

  std::vector<std::vector<size_t>> result;

  std::vector<int> indices;

  for (unsigned int i = 0; i < items.size(); i++) {

    // An item can only belong to one cluster. So discard if already belongs to one.
    if (count(indices.begin(), indices.end(), i) > 0) continue;

    std::vector<size_t> cluster;
    cluster.push_back(i);

    for (unsigned int j = i+1; j < items.size(); j++) {
      if (cmpFunction(items[i], items[j])) {
        indices.push_back(j);
        cluster.push_back(j);
      }
    }
    result.push_back(cluster);
  }

  return result;
}

#endif
