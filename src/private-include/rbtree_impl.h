#pragma once
#include <assert.h>
#include <optional>
#include "types.h"

namespace nictheboy {

class RBTreeImpl {
   private:
    enum class Color {
        RED,
        BLACK,
    };

    enum class Direction {
        LEFT,
        RIGHT,
    };

    static Direction Opposite(Direction dir) { return dir == Direction::LEFT ? Direction::RIGHT : Direction::LEFT; }

    struct Node {
        KeyType key;
        ObjectType object;

        Node* lchild;
        Node* rchild;
        Color color;
        Node*& child(Direction dir) { return dir == Direction::LEFT ? lchild : rchild; }
        Direction direction(KeyType next_key) {
            assert(next_key != key);
            return next_key < key ? Direction::LEFT : Direction::RIGHT;
        }
    };

    Node* root;

    void ReleaseNodesRecursively(Node* node);
    void Refactor3Childs(Node* node, Direction direction_of_red_child, Node** ref);

    struct ParentChild {
        Node *child, *parent;
        std::optional<Direction> parent_to_node;
    };

    ParentChild RefactorChildToBlack(
        Node* node,
        Node* parent,
        std::optional<Direction> parent_to_node,
        Node* grandparent,
        std::optional<Direction> grandparent_to_parent,
        Node** ref_grandparent,
        KeyType toward_key);

   public:
    RBTreeImpl();
    ~RBTreeImpl();
    void Insert(KeyType key, ObjectType object);
    void Delete(KeyType key);
    std::optional<ObjectType> Find(KeyType key);
    void Debug();
};

};  // namespace nictheboy
