#pragma once
#include <assert.h>
#include <algorithm>
#include <functional>
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

    void ReleaseNodesRecursively(Node* node) {
        if (node->lchild)
            ReleaseNodesRecursively(node->lchild);
        if (node->rchild)
            ReleaseNodesRecursively(node->rchild);
        delete node;
    }

    inline void Rotate(Node** ref, Direction dir) {
        auto opp = Opposite(dir);
        auto node_2 = *ref;
        auto node_1 = node_2->child(opp);
        assert(node_1);
        auto subtree_1 = node_1->child(opp);
        auto subtree_2 = node_1->child(dir);
        auto subtree_3 = node_2->child(dir);
        *ref = node_1;
        node_1->child(opp) = subtree_1;
        node_1->child(dir) = node_2;
        node_2->child(opp) = subtree_2;
        node_2->child(dir) = subtree_3;
    }

    inline void RotateZigZag(Node** ref, Direction dir) {
        auto opp = Opposite(dir);
        assert((*ref)->child(opp));
        assert((*ref)->child(opp)->child(dir));
        Rotate(&(*ref)->child(opp), opp);
        Rotate(ref, dir);
    }

    inline void RotateBlackParentRedChild(Node** ref, Direction dir) {
        assert((*ref)->color == Color::BLACK);
        assert((*ref)->child(Opposite(dir)) && (*ref)->child(Opposite(dir))->color == Color::RED);
        Rotate(ref, dir);
        (*ref)->color = Color::BLACK;
        (*ref)->child(dir)->color = Color::RED;
    }

    inline void RebalanceRedParentRedChildWithoutRedUncleZigZig(Node** ref_grandparent, Direction dir_red_parent) {
        auto opp = Opposite(dir_red_parent);
        assert((*ref_grandparent)->color == Color::BLACK);
        assert((*ref_grandparent)->child(dir_red_parent)->color == Color::RED);
        assert((*ref_grandparent)->child(dir_red_parent)->child(dir_red_parent)->color == Color::RED);
        assert(!(*ref_grandparent)->child(opp) || (*ref_grandparent)->child(opp)->color == Color::BLACK);
        RotateBlackParentRedChild(ref_grandparent, opp);
    }

    inline void RebalanceRedParentRedChildWithoutRedUncleZigZag(Node** ref_grandparent, Direction dir_red_parent) {
        auto opp = Opposite(dir_red_parent);
        assert((*ref_grandparent)->color == Color::BLACK);
        assert((*ref_grandparent)->child(dir_red_parent)->color == Color::RED);
        assert((*ref_grandparent)->child(dir_red_parent)->child(opp)->color == Color::RED);
        assert(!(*ref_grandparent)->child(opp) || (*ref_grandparent)->child(opp)->color == Color::BLACK);
        RotateZigZag(ref_grandparent, opp);
        (*ref_grandparent)->color = Color::BLACK;
        (*ref_grandparent)->child(opp)->color = Color::RED;
    }

    inline void RebalancePossibleRedParentRedChildWithoutRedUncle(
        std::optional<Direction> parent_to_node,
        Node* parent,
        std::optional<Direction> grandparent_to_parent,
        Node* grandparent,
        Node** ref_grandparent) {
        if (parent && parent->color == Color::RED) {
            if (grandparent) {
                assert(parent_to_node.has_value());
                assert(grandparent_to_parent.has_value());
                if (parent_to_node == grandparent_to_parent) {
                    RebalanceRedParentRedChildWithoutRedUncleZigZig(ref_grandparent, *grandparent_to_parent);
                } else {
                    RebalanceRedParentRedChildWithoutRedUncleZigZag(ref_grandparent, *grandparent_to_parent);
                }
            } else {
                parent->color = Color::BLACK;
            }
        }
    }

    Node* DetachNode(KeyType key) {
        assert(root);
        if (root->color == Color::RED)
            return DetachOnRedRootSubtree(&root, key);
        if ((!root->lchild || root->lchild->color == Color::BLACK) &&
            (!root->rchild || root->rchild->color == Color::BLACK)) {
            root->color = Color::RED;
            return DetachOnRedRootSubtree(&root, key);
        }
        if (root->key == key) {
            auto direction = (root->lchild && root->lchild->color == Color::RED) ? Direction::LEFT : Direction::RIGHT;
            auto red_subtree_root = &root->child(direction);
            auto minmax = DetachOnRedRootSubtree(red_subtree_root, MinOrMaxOfSubtree(*red_subtree_root, Opposite(direction)));
            std::swap(root->key, minmax->key);
            std::swap(root->object, minmax->object);
            return minmax;
        }
        auto direction = root->direction(key);
        auto subtree = &root->child(direction);
        assert(*subtree);
        if ((*subtree)->color != Color::RED) {
            RotateBlackParentRedChild(&root, direction);
            subtree = &root->child(direction);
        }
        return DetachOnRedRootSubtree(subtree, key);
    }

    Node* DetachOnRedRootSubtree(Node** red_subtree_root, KeyType key) {
        assert(*red_subtree_root);
        Node** ref = red_subtree_root;
        while (true) {
            assert((*ref)->color == Color::RED);
            if ((*ref)->key == key)
                return DetachRedNode(ref);
            auto dir = (*ref)->direction(key);
            auto ref_child_dir = &(*ref)->child(dir);
            assert(*ref_child_dir);
            if ((*ref_child_dir)->lchild || (*ref_child_dir)->rchild) {
                if ((*ref_child_dir)->key == key) {
                    auto child_dir = *ref_child_dir;
                    assert(child_dir->lchild || child_dir->rchild);
                    auto direction = child_dir->lchild ? Direction::LEFT : Direction::RIGHT;
                    auto subtree_2 = child_dir->child(direction);
                    assert(subtree_2);
                    auto minmax = DetachOnRedRootSubtree(ref, MinOrMaxOfSubtree(subtree_2, Opposite(direction)));
                    std::swap(child_dir->key, minmax->key);
                    std::swap(child_dir->object, minmax->object);
                    return minmax;
                }
                auto dir2 = (*ref_child_dir)->direction(key);
                auto ref_child_dir_child_dir2 = &(*ref_child_dir)->child(dir2);
                assert(*ref_child_dir_child_dir2);
                if ((*ref_child_dir_child_dir2)->color == Color::RED) {
                    ref = ref_child_dir_child_dir2;
                    continue;
                }
                auto opp2 = Opposite(dir2);
                auto ref_child_dir_child_opp2 = &(*ref_child_dir)->child(opp2);
                if (ref_child_dir_child_opp2 && (*ref_child_dir_child_opp2)->color == Color::RED) {
                    RotateBlackParentRedChild(ref_child_dir, dir2);
                    continue;
                }
            }
            auto opp = Opposite(dir);
            auto ref_child_opp = &(*ref)->child(opp);
            assert(*ref_child_opp);
            auto ref_child_opp_child_dir = &(*ref_child_opp)->child(dir);
            auto ref_child_opp_child_opp = &(*ref_child_opp)->child(opp);
            if ((!*ref_child_opp_child_dir || (*ref_child_opp_child_dir)->color == Color::BLACK) &&
                (!*ref_child_opp_child_opp || (*ref_child_opp_child_opp)->color == Color::BLACK)) {
                (*ref)->color = Color::BLACK;
                (*ref_child_dir)->color = Color::RED;
                (*ref_child_opp)->color = Color::RED;
                ref = ref_child_dir;
                continue;
            }
            if (*ref_child_opp_child_opp && (*ref_child_opp_child_opp)->color == Color::RED) {
                Rotate(ref, dir);
                (*ref)->color = Color::RED;
                (*ref_child_opp_child_opp)->color = Color::BLACK;
                (*ref)->child(dir)->color = Color::BLACK;
                (*ref_child_dir)->color = Color::RED;
                ref = ref_child_dir;
                continue;
            }
            if (*ref_child_opp_child_dir && (*ref_child_opp_child_dir)->color == Color::RED) {
                RotateZigZag(ref, dir);
                (*ref)->color = Color::RED;
                (*ref)->child(dir)->color = Color::BLACK;
                (*ref_child_dir)->color = Color::RED;
                ref = ref_child_dir;
                continue;
            }
            assert(false);
        }
    }

    Node* DetachRedNode(Node** ref) {
        if (!(*ref)->lchild && !(*ref)->rchild) {
            auto ret = *ref;
            *ref = nullptr;
            return ret;
        } else {
            auto node = *ref;
            auto direction = node->lchild ? Direction::LEFT : Direction::RIGHT;
            auto subtree_2 = node->child(direction);
            assert(subtree_2);
            auto minmax = DetachOnRedRootSubtree(ref, MinOrMaxOfSubtree(subtree_2, Opposite(direction)));
            std::swap(node->key, minmax->key);
            std::swap(node->object, minmax->object);
            return minmax;
        }
    }

    KeyType MinOrMaxOfSubtree(Node* node, Direction dir) {
        assert(node);
        while (node->child(dir))
            node = node->child(dir);
        return node->key;
    }

   public:
    RBTreeImpl() {
        root = nullptr;
    }

    ~RBTreeImpl() {
        if (root)
            ReleaseNodesRecursively(root);
    }

    void Insert(KeyType key, ObjectType object) {
        auto new_node = new Node{key, object, nullptr, nullptr, Color::RED};
        if (!root) {
            root = new_node;
            return;
        }
        Node *current = root, *parent = nullptr, *grandparent = nullptr, **ref_grandparent = nullptr;
        std::optional<Direction> parent_to_current, grandparent_to_parent;
        Direction direction;
        while (current) {
            if ((current->lchild && current->lchild->color == Color::RED &&
                 current->rchild && current->rchild->color == Color::RED)) {
                current->color = Color::RED;
                current->lchild->color = Color::BLACK;
                current->rchild->color = Color::BLACK;
                RebalancePossibleRedParentRedChildWithoutRedUncle(
                    parent_to_current, parent, grandparent_to_parent, grandparent, ref_grandparent);
                if (current->color == Color::BLACK) {  // Zig-Zag Rebalance
                    parent_to_current = current->direction(key);
                    parent = current;
                    current = current->child(*parent_to_current);
                }
            }
            direction = current->direction(key);
            ref_grandparent = grandparent ? &grandparent->child(grandparent_to_parent.value())
                              : parent    ? &root
                                          : nullptr;
            grandparent = parent;
            parent = current;
            current = current->child(direction);
            grandparent_to_parent = parent_to_current;
            parent_to_current = direction;
        }
        parent->child(direction) = new_node;
        RebalancePossibleRedParentRedChildWithoutRedUncle(
            direction, parent, grandparent_to_parent, grandparent, ref_grandparent);
    }

    void Delete(KeyType key) {
        auto node = DetachNode(key);
        delete node;
    }

    std::optional<ObjectType> Find(KeyType key) {
        auto current = root;
        while (current) {
            if (current->key == key)
                return current->object;
            auto direction = current->direction(key);
            current = current->child(direction);
        }
        return std::nullopt;
    }

#ifndef NDEBUG
    void Debug() {
        std::function<void(Node * node, int depth)> PrintNode = [&](Node* node, int depth) {
            if (!node)
                return;
            PrintNode(node->rchild, depth + 1);
            for (int i = 0; i < depth; i++)
                printf("\t");
            printf("[%c] %lld\n", node->color == Color::BLACK ? 'B' : 'R', node->key);
            PrintNode(node->lchild, depth + 1);
        };
        PrintNode(root, 0);
        std::function<int(Node * node)> Validate = [&](Node* node) {
            if (!node)
                return 1;
            if (node->lchild) {
                assert(node->lchild->key < node->key);
                assert(!(node->lchild->color == Color::RED && node->color == Color::RED));
            }
            if (node->rchild) {
                assert(node->rchild->key > node->key);
                assert(!(node->rchild->color == Color::RED && node->color == Color::RED));
            }
            int left_black_height = Validate(node->lchild);
            int right_black_height = Validate(node->rchild);
            assert(left_black_height == right_black_height);
            return left_black_height + (node->color == Color::BLACK ? 1 : 0);
        };
        Validate(root);
    }
#endif
};

};  // namespace nictheboy
