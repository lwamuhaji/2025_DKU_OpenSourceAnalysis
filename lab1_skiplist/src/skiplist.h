#include <stdlib.h>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <bit>
#include <functional>
#include <mutex>
#include <vector>
#include <climits>

#include <atomic>

#define MAX_INT

typedef std::chrono::high_resolution_clock Clock;

// Key is an 8-byte integer
typedef uint64_t Key;

int compare_(const Key& a, const Key& b) {
    if (a < b) {
        return -1;
    } else if (a > b) {
        return +1;
    } else {
        return 0;
    }
}

template<typename Key>
class SkipList {
   private:
    struct Node;

   public:
    SkipList(int max_level = 16, float probability = 0.5);

    void Insert(const Key& key); // Insertion function (to be implemented by students)
    bool Contains(const Key& key) const; // Lookup function (to be implemented by students)
    std::vector<Key> Scan(const Key& key, const int scan_num); // Range query function (to be implemented by students)
    bool Delete(const Key& key) const; // Delete function (to be implemented by students)

    void Print() const;

   private:
    int RandomLevel(); // Generates a random level for new nodes (to be implemented by students)

    Node* head; // Head node (starting point of the SkipList)
    int max_level; // Maximum level in the SkipList
    float probability; // Probability factor for level increase
};

// SkipList Node structure
template<typename Key>
struct SkipList<Key>::Node {
    Key key;
    std::vector<Node*> next; // Pointer array for multiple levels

    // Constructor for Node
    Node(Key key, int level): key(key), next(level, nullptr) {};
};

// Generate a random level for new nodes
template<typename Key>
int SkipList<Key>::RandomLevel() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 1);

    int level = 1;
    while(dist(gen) > probability && level < max_level) {
        level++;
    }

    return level; // Default return value (students should modify this)
}

// Constructor for SkipList
template<typename Key>
SkipList<Key>::SkipList(int max_level, float probability)
    : max_level(max_level), probability(probability) {
    head = new Node(INT_MIN, max_level);
    Node* tail = new Node(INT_MAX, max_level);
    for(int level = 0; level < max_level; level++) {
        head->next[level] = tail;
    }
}

// Insert function (inserts a key into SkipList)
template<typename Key>
void SkipList<Key>::Insert(const Key& key) {
    Node* current = head;
    std::vector<Node*> target(max_level, nullptr);

    for(int level = max_level - 1; level >= 0; level--) {
        while(current->next[level]->key < key) {
            current = current->next[level];
        }
        target[level] = current;
    }

    int random_level = RandomLevel();
    Node* new_node = new Node(key, random_level);
    for(int level = 0; level < random_level; level++) {
        new_node->next[level] = target[level]->next[level];
        target[level]->next[level] = new_node;
    }
}

// Delete function (removes a key from SkipList)
template<typename Key>
bool SkipList<Key>::Delete(const Key& key) const {
    // To be implemented by students
    return false;
}

// Lookup function (checks if a key exists in SkipList)
template<typename Key>
bool SkipList<Key>::Contains(const Key& key) const {
    Node* current = head;

    for(int level == max_level - 1; level >= 0; level--) {
        // 갈 수 있는 곳까지 순회
        while(current->next[level]->key < key) {
            current = current->next[level];
        }
        // 다음 레벨로 가기 전에 key를 찾았는지 확인
        if (current->next[level]->key == key) return true;
    }
    return false;
}

// Range query function (retrieves scan_num keys starting from key)
template<typename Key>
std::vector<Key> SkipList<Key>::Scan(const Key& key, const int scan_num) { 
    // starting key가 없는 경우 return empty vector
    if (!Contains(key)) return {}

    Node* current = head;
    Node* start_node;

    // 해당 node로 찾아가서 start에 저장
    for(int level == max_level - 1; level >= 0; level--) {
        while(current->next[level]->key < key) {
            current = current->next[level];
        }
        if (current->next[level]->key == key) {
            start_node = current->next[level];
        }
    }

    // start부터 scan_sum 만큼 탐색
    int found = 1;
    while(found < scan_num && current->key != INT_MAX) {
        current
    }

    return {};
}

template<typename Key>
void SkipList<Key>::Print() const {
  std::cout << "SkipList Structure:\n";
  for (int level = max_level - 1; level >= 0; --level) {
    Node* node = head->next[level];
    std::cout << "Level " << level << ": ";
    while (node != nullptr) {
      std::cout << node->key << " ";
      node = node->next[level];
    }
    std::cout << "\n";
  }
}
