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

    void Rotate(Node** ref, Direction dir) {
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

    void RotateZigzag(Node** ref, Direction dir) {
        auto opp = Opposite(dir);
        assert((*ref)->child(opp));
        assert((*ref)->child(opp)->child(dir));
        Rotate(&(*ref)->child(opp), opp);
        Rotate(ref, dir);
    }

    struct ParentChild {
        Node *child, *parent;
        std::optional<Direction> parent_to_node;
    };

    ParentChild Rebalance2RedChilds(
        Node* node,
        Node* parent,
        std::optional<Direction> parent_to_node,
        Node* grandparent,
        std::optional<Direction> grandparent_to_parent,
        Node** ref_grandparent,
        KeyType toward_key) {
        assert(node);
        assert(node->color == Color::BLACK);
        assert(node->lchild && node->lchild->color == Color::RED);
        assert(node->rchild && node->rchild->color == Color::RED);
        if (parent && parent->color == Color::RED) {
            assert(parent_to_node.has_value());
            if (grandparent) {
                assert(grandparent_to_parent.has_value());
                auto direction_of_parent_to_node = parent_to_node.value();
                auto direction_of_grandparent_to_parent = grandparent_to_parent.value();
                if (direction_of_parent_to_node == direction_of_grandparent_to_parent) {
                    Rotate(ref_grandparent, Opposite(direction_of_grandparent_to_parent));
                    (*ref_grandparent)->color = Color::BLACK;
                    (*ref_grandparent)->lchild->color = Color::RED;
                    (*ref_grandparent)->rchild->color = Color::RED;
                    node->lchild->color = Color::BLACK;
                    node->rchild->color = Color::BLACK;
                } else {
                    RotateZigzag(ref_grandparent, direction_of_parent_to_node);
                    (*ref_grandparent)->child(direction_of_parent_to_node)->color = Color::RED;
                    (*ref_grandparent)->child(direction_of_parent_to_node)->child(direction_of_grandparent_to_parent)->color = Color::BLACK;
                    (*ref_grandparent)->child(direction_of_grandparent_to_parent)->child(direction_of_parent_to_node)->color = Color::BLACK;
                    parent = *ref_grandparent;
                    parent_to_node = parent->direction(toward_key);
                    node = parent->child(parent_to_node.value());
                }
                assert(parent->color == Color::BLACK);
            } else {
                parent->color = Color::BLACK;
                node->color = Color::RED;
                node->lchild->color = Color::BLACK;
                node->rchild->color = Color::BLACK;
            }
        } else {
            node->color = Color::RED;
            node->lchild->color = Color::BLACK;
            node->rchild->color = Color::BLACK;
        }
        return ParentChild{node, parent, parent_to_node};
    }

    void InsertWithoutRedFatherAndUncle(
        Node* node,
        Node* parent,
        Direction parent_to_node,
        Node* grandparent,
        std::optional<Direction> grandparent_to_parent,
        Node** ref_grandparent) {
        assert(node);
        assert(node->color == Color::RED);
        assert(parent);
        if (parent->color == Color::RED) {
            if (grandparent) {
                assert(grandparent_to_parent.has_value());
                auto direction_of_parent_to_node = parent_to_node;
                auto direction_of_grandparent_to_parent = grandparent_to_parent.value();
                if (direction_of_parent_to_node == direction_of_grandparent_to_parent) {
                    parent->child(parent_to_node) = node;
                    Rotate(ref_grandparent, Opposite(direction_of_grandparent_to_parent));
                    (*ref_grandparent)->color = Color::BLACK;
                    (*ref_grandparent)->child(Opposite(direction_of_grandparent_to_parent))->color = Color::RED;
                } else {
                    parent->child(parent_to_node) = node;
                    RotateZigzag(ref_grandparent, direction_of_parent_to_node);
                    (*ref_grandparent)->color = Color::BLACK;
                    (*ref_grandparent)->child(Opposite(direction_of_grandparent_to_parent))->color = Color::RED;
                }
            } else {
                parent->color = Color::BLACK;
                parent->child(parent_to_node) = node;
            }
        } else {
            parent->child(parent_to_node) = node;
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
        if ((*subtree)->color == Color::RED)
            return DetachOnRedRootSubtree(subtree, key);
        Rotate(&root, direction);
        root->color = Color::BLACK;
        subtree = &root->child(direction);
        (*subtree)->color = Color::RED;
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
                    Rotate(ref_child_dir, dir2);
                    (*ref_child_dir)->color = Color::BLACK;
                    ref = &(*ref_child_dir)->child(dir2);
                    (*ref)->color = Color::RED;
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
                Rotate(ref_child_opp, opp);
                Rotate(ref, dir);
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
                auto ret = Rebalance2RedChilds(current, parent, parent_to_current,
                                               grandparent, grandparent_to_parent,
                                               ref_grandparent, key);
                current = ret.child;
                parent = ret.parent;
                parent_to_current = ret.parent_to_node;
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
        InsertWithoutRedFatherAndUncle(new_node, parent, direction, grandparent, grandparent_to_parent, ref_grandparent);
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
