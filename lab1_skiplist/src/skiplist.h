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


typedef std::chrono::high_resolution_clock Clock;

// Key is an 8-byte integer
typedef uint64_t Key;

int compare_(const Key& a, const Key& b) {
    if (a < b) {
        return -1;
    }
    else if (a > b) {
        return +1;
    }
    else {
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
    Node(Key key, int level) : key(key), next(level, nullptr) {};
};

// Generate a random level for new nodes
template<typename Key>
int SkipList<Key>::RandomLevel() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 1);

    int level = 1;
    while (dist(gen) > probability && level < max_level) {
        level++;
    }

    return level;
}

// Constructor for SkipList
template<typename Key>
SkipList<Key>::SkipList(int max_level, float probability)
    : max_level(max_level), probability(probability) {
    head = new Node(INT64_MIN, max_level); // INT_MIN을 갖는 head 노드 생성
    Node* tail = new Node(INT64_MAX, max_level); // INT_MAX를 갖는 tail 노드 생성
    for (int level = 0; level < max_level; level++) {
        head->next[level] = tail; // head의 next를 tail로 설정
    }
}

// Insert function (inserts a key into SkipList)
template<typename Key>
void SkipList<Key>::Insert(const Key& key) {
    Node* current = head;
    std::vector<Node*> target(max_level, nullptr);

    // 1. max_level부터 한 단계씩 아래로 내려감
    for (int level = max_level - 1; level >= 0; level--) {
        // 2. 다음 노드로 이동할 수 있을 때까지 이동
        while (current->next[level]->key < key) {
            current = current->next[level];
        }
        // 3. 마지막 노드 저장
        target[level] = current;
    }

    int random_level = RandomLevel(); // 새로운 노드의 레벨을 얻는다.
    Node* new_node = new Node(key, random_level); // 새 노드 생성
    for (int level = 0; level < random_level; level++) {
        new_node->next[level] = target[level]->next[level]; // 새로운 노드의 next를 설정
        target[level]->next[level] = new_node; // 이전 노드의 next를 재설정
    }
}

// Delete function (removes a key from SkipList)
template<typename Key>
bool SkipList<Key>::Delete(const Key& key) const {
    Node* current = head;
    std::vector<Node*> prev_nodes(max_level, nullptr);

    for (int level = max_level - 1; level >= 0; level--) {
        // skip할 수 있는 곳까지 skip
        while (current->next[level]->key < key) {
            current = current->next[level];
        }
        // 다음 레벨로 가기 전에 key를 찾았는지 확인
        if (current->next[level]->key == key) {
            prev_nodes[level] = current; // 삭제 대상 노드의 바로 전 노드인 경우 저장
        }
    }

    // 실제 삭제 작업 수행
    bool deleted = false;
    for (int level = max_level - 1; level >= 0; level--) {
        if (prev_nodes[level] != nullptr) {
            prev_nodes[level]->next[level] = prev_nodes[level]->next[level]->next[level];
            deleted = true; // 삭제가 발생했는지 기록
        }
    }

    return deleted;
}

// Lookup function (checks if a key exists in SkipList)
template<typename Key>
bool SkipList<Key>::Contains(const Key& key) const {
    Node* current = head;

    for (int level = max_level - 1; level >= 0; level--) {
        // skip할 수 있는 곳까지 skip
        while (current->next[level]->key < key) {
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
    if (!Contains(key)) return {};

    Node* current = head;
    Node* start_node;

    // 해당 node로 찾아가서 start에 저장
    for (int level = max_level - 1; level >= 0; level--) {
        // skip할 수 있는 곳까지 skip
        while (current->next[level]->key < key) {
            current = current->next[level];
        }
        if (current->next[level]->key == key) {
            // 일치하는 key를 찾은 경우 start_node에 저장
            start_node = current->next[level];
        }
    }

    // start부터 scan_sum 만큼 탐색
    int found = 1;
    current = start_node;
    std::vector<Key> nodes = { current->key };
    // 다음 노드가 마지막(INT_MAX)이 아니고, found < scan_num 이면 다음 노드로 이동
    while (found < scan_num && current->next[0]->key != INT64_MAX) {
        current = current->next[0];
        nodes.push_back(current->key);
    }

    return nodes;
}

template<typename Key>
void SkipList<Key>::Print() const {
    std::cout << "SkipList Structure:\n";
    for (int level = max_level - 1; level >= 0; --level) {
        Node* node = head->next[level];
        std::cout << "Level " << level << ": ";
        while (node->key != INT64_MAX) {
            std::cout << node->key << " ";
            node = node->next[level];
        }
        std::cout << "\n";
    }
}
