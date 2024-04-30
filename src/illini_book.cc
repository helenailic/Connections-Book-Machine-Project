#include "illini_book.hpp"

#include <fstream>
#include <queue>
#include <set>

// constructor
IlliniBook::IlliniBook(const std::string& people_fpath,
                       const std::string& relations_fpath) {
  std::ifstream ifs_ppl(people_fpath);
  std::ifstream ifs_relations(relations_fpath);
  if (!ifs_ppl.is_open() || !ifs_relations.is_open()) {
    throw std::invalid_argument("one or more files couldn't open");
  }
  while (ifs_ppl.good()) {
    int uin = 0;
    ifs_ppl >> uin;
    ppl_relations_[uin] = std::map<std::string, std::vector<int>>();
  }

  while (ifs_relations.good()) {
    int uin1 = 0;
    int uin2 = 0;
    std::string relationship;
    ifs_relations >> uin1;
    ifs_relations.ignore(1, ',');
    ifs_relations >> uin2;
    ifs_relations.ignore(1, ',');
    ifs_relations >> relationship;

    if (!(ppl_relations_[uin1].contains(relationship))) {
      ppl_relations_[uin1][relationship] = std::vector<int>();
    }
    ppl_relations_[uin1][relationship].push_back(uin2);
    if (!(ppl_relations_[uin2].contains(relationship))) {
      ppl_relations_[uin2][relationship] = std::vector<int>();
    }
    ppl_relations_[uin2][relationship].push_back(uin1);
  }
}

bool IlliniBook::AreRelated(int uin_1, int uin_2) const {
  // return true if there exists a path between 2 nodes

  // keep queue containing people to visit
  std::queue<int> q;
  // keep set (which contains all the nodes already added to the queue)
  std::set<int> v;

  // start it up with the starting uin's direct friends
  q.push(uin_1);
  v.insert(uin_1);

  // loop until you find uin_2 --> return true or the queue becomes empty -->
  // return false
  while (!(q.empty())) {
    // go through the queue and check if its uin_2
    int current_uin = q.front();
    // removes first value from the q
    q.pop();
    // adds current_uin as visited
    // v.insert(current_uin); --> I changed it so this behavior happens when you
    // initially add them to the queue check if its uin_2 if yes... you're done
    if (current_uin == uin_2) {
      return true;
      // else add all of that uin's connections to the queue if not seen/visited
      // already
    }
    for (auto const& [key, value] : ppl_relations_.at(current_uin)) {
      for (unsigned int i = 0; i < value.size(); ++i) {
        auto it = v.find(value[i]);
        if (it == v.end()) {
          q.push(value[i]);
          v.insert(value[i]);
        }
      }
    }
  }
  return false;
}

bool IlliniBook::AreRelated(int uin_1,
                            int uin_2,
                            const std::string& relationship) const {
  std::queue<int> q;
  std::set<int> v;
  q.push(uin_1);
  v.insert(uin_1);

  while (!(q.empty())) {
    int current_uin = q.front();
    q.pop();
    if (current_uin == uin_2) {
      return true;
    }
    if (ppl_relations_.at(current_uin).contains(relationship)) {
      for (unsigned int i = 0;
           i < ppl_relations_.at(current_uin).at(relationship).size();
           ++i) {
        auto it = v.find(ppl_relations_.at(current_uin).at(relationship)[i]);
        if (it == v.end()) {
          q.push(ppl_relations_.at(current_uin).at(relationship)[i]);
          v.insert(ppl_relations_.at(current_uin).at(relationship)[i]);
        }
      }
    }
  }
  return false;
}

int IlliniBook::GetRelated(int uin_1, int uin_2) const {
  // keep queue containing people to visit
  std::queue<int> q;
  // keep set (which contains all the nodes already added to the queue)
  std::set<int> v;
  std::map<int, int> distances;  // map of each uin we've seen +
                                 // respective distance from uin_1

  // start it up with just the starting uin
  q.push(uin_1);
  v.insert(uin_1);
  distances[uin_1] = 0;

  while (!(q.empty())) {
    // go through the queue and check if its uin_2
    int current_uin = q.front();
    q.pop();
    // adds current_uin as visited
    if (current_uin == uin_2) {
      return distances[current_uin];
      // else add all of that uin's connections to the queue if not seen
    }
    for (auto const& [key, value] : ppl_relations_.at(current_uin)) {
      for (unsigned int i = 0; i < value.size(); ++i) {
        auto it = v.find(value[i]);
        if (it == v.end()) {
          q.push(value[i]);
          v.insert(value[i]);
          distances[value[i]] = distances[current_uin] + 1;
        }
      }
    }
  }

  return -1;
}

int IlliniBook::GetRelated(int uin_1,
                           int uin_2,
                           const std::string& relationship) const {
  std::queue<int> q;
  // keep set (which contains all the nodes already added to the queue) - do I
  // need a seen distinction?
  std::set<int> v;
  std::map<int, int> distances;

  // start it up with the starting uin's direct friends
  q.push(uin_1);
  v.insert(uin_1);

  // loop until you find uin_2 --> return true or the queue becomes empty -->
  // return false
  while (!(q.empty())) {
    // go through the queue and check if its uin_2
    int current_uin = q.front();
    q.pop();
    if (current_uin == uin_2) {
      return distances[current_uin];
    }
    if (ppl_relations_.at(current_uin).contains(relationship)) {
      for (unsigned int i = 0;
           i < ppl_relations_.at(current_uin).at(relationship).size();
           ++i) {
        auto it = v.find(ppl_relations_.at(current_uin).at(relationship)[i]);
        if (it == v.end()) {
          q.push(ppl_relations_.at(current_uin).at(relationship)[i]);
          v.insert(ppl_relations_.at(current_uin).at(relationship)[i]);
          distances[ppl_relations_.at(current_uin).at(relationship)[i]] =
              distances[current_uin] + 1;
        }
      }
    }
  }
  return -1;
}

std::vector<int> IlliniBook::GetSteps(int uin, int n) const {
  std::queue<int> q;
  std::set<int> v;  // set of all nodes already added to queue
  std::set<int> n_uins_set;
  std::map<int, int> distances;
  if (n == 0) {
    std::vector<int> empty;
    return empty;
  }
  q.push(uin);
  v.insert(uin);
  distances[uin] = 0;
  while (!(q.empty())) {
    if (distances[q.front()] >= n) {
      break;
    }
    int current_uin = q.front();
    q.pop();
    for (auto const& [key, value] : ppl_relations_.at(current_uin)) {
      for (unsigned int j = 0; j < value.size(); ++j) {
        auto it = v.find(value[j]);
        if (it == v.end()) {
          q.push(value[j]);
          v.insert(value[j]);
          distances[value[j]] = distances[current_uin] + 1;
        }
      }
    }
  }
  while (!(q.empty())) {
    int current_uin = q.front();
    q.pop();
    n_uins_set.insert(current_uin);
  }
  std::vector<int> uin_vector;
  for (auto it = n_uins_set.begin(); it != n_uins_set.end(); ++it) {
    uin_vector.push_back(*it);
  }
  return uin_vector;
}

size_t IlliniBook::CountGroups() const {
  std::vector<int> vertices;
  for (auto const& [key, value] : ppl_relations_) {
    vertices.push_back(key);
  }
  std::queue<int> q;
  std::set<int> v;
  size_t num = 0;
  while (!(vertices.empty())) {
    q.push(vertices.at(0));
    v.insert(vertices.at(0));
    while (!(q.empty())) {
      int current_uin = q.front();
      q.pop();
      vertices.erase(
          std::remove(vertices.begin(), vertices.end(), current_uin));
      for (auto const& [key, value] : ppl_relations_.at(current_uin)) {
        for (unsigned int i = 0; i < value.size(); ++i) {
          auto it = v.find(value[i]);
          if (it == v.end()) {
            q.push(value[i]);
            v.insert(value[i]);
          }
        }
      }
    }
    num += 1;
  }
  return num;
}

size_t IlliniBook::CountGroups(const std::string& relationship) const {
  std::vector<int> vertices;
  for (auto const& [key, value] : ppl_relations_) {
    if (value.contains(relationship)) {
      vertices.push_back(key);
    }
  }
  std::queue<int> q;
  std::set<int> v;
  size_t num = 0;
  while (!(vertices.empty())) {
    q.push(vertices.at(0));
    v.insert(vertices.at(0));
    while (!(q.empty())) {
      int current_uin = q.front();
      q.pop();
      vertices.erase(
          std::remove(vertices.begin(), vertices.end(), current_uin));
      for (unsigned int i = 0;
           i < ppl_relations_.at(current_uin).at(relationship).size();
           ++i) {
        auto it = v.find(ppl_relations_.at(current_uin).at(relationship)[i]);
        if (it == v.end()) {
          q.push(ppl_relations_.at(current_uin).at(relationship)[i]);
          v.insert(ppl_relations_.at(current_uin).at(relationship)[i]);
        }
      }
    }
    num += 1;
  }
  int alone = 0;
  for (auto const& [key, value] : ppl_relations_) {
    if (!(value.contains(relationship))) {
      alone += 1;
    }
  }
  return num + alone;
}

std::vector<int> IlliniBook::PopulateVertices(
    const std::map<int, std::map<std::string, std::vector<int>>>&
        ppl_relations_,
    const std::vector<std::string>& relationships) const {
  std::vector<int> vertices;
  for (auto const& [key, value] : ppl_relations_) {
    bool already_pushed = false;
    for (unsigned int i = 0; i < relationships.size(); ++i) {
      if (value.contains(relationships[i]) && !(already_pushed)) {
        vertices.push_back(key);
        already_pushed = true;
      }
    }
  }
  return vertices;
}

size_t IlliniBook::CountGroups(
    const std::vector<std::string>& relationships) const {
  std::vector<int> vertices = PopulateVertices(ppl_relations_, relationships);
  std::queue<int> q;
  std::set<int> v;
  size_t num = 0;
  while (!(vertices.empty())) {
    q.push(vertices.at(0));
    v.insert(vertices.at(0));
    while (!(q.empty())) {
      int current_uin = q.front();
      q.pop();
      vertices.erase(
          std::remove(vertices.begin(), vertices.end(), current_uin));
      std::map<std::string, std::vector<int>> thing =
          ppl_relations_.at(current_uin);
      for (unsigned int i = 0; i < relationships.size(); ++i) {
        if (ppl_relations_.at(current_uin).contains(relationships[i])) {
          for (unsigned int j = 0; j < thing.at(relationships[i]).size(); ++j) {
            auto it = v.find(thing.at(relationships[i])[j]);
            if (it == v.end()) {
              q.push(thing.at(relationships[i])[j]);
              v.insert(thing.at(relationships[i])[j]);
            }
          }
        }
      }
    }
    num += 1;
  }
  return AddingNum(ppl_relations_, relationships, num);
}

size_t IlliniBook::AddingNum(
    const std::map<int, std::map<std::string, std::vector<int>>>&
        ppl_relations_,
    const std::vector<std::string>& relationships,
    size_t num) const {
  for (auto const& [key, value] : ppl_relations_) {
    bool exists = false;
    for (unsigned int i = 0; i < relationships.size(); ++i) {
      if (value.contains(relationships[i])) {
        exists = true;
      }
    }
    if (!(exists)) {
      num += 1;
    }
  }
  return num;
}