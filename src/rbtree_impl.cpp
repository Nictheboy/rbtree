#include "private-include/rbtree_impl.h"
#include <assert.h>

namespace nictheboy {

RBTreeImpl::RBTreeImpl() {
    root = nullptr;
}

RBTreeImpl::~RBTreeImpl() {
    if (root)
        ReleaseNodesRecursively(root);
}

void RBTreeImpl::ReleaseNodesRecursively(RBTreeImpl::Node* node) {
    if (node->lchild)
        ReleaseNodesRecursively(node->lchild);
    if (node->rchild)
        ReleaseNodesRecursively(node->rchild);
    delete node;
}

void RBTreeImpl::Refactor3Childs(Node* node,
                                 Direction direction_of_red_child,
                                 Node** ref) {
    assert(node);
    assert(node->color == Color::BLACK);
    auto node_2 = node->child(direction_of_red_child);
    assert(node_2);
    assert(node_2->color == Color::RED);
    auto node_3 = node->child(Opposite(direction_of_red_child));
    assert(!node_3 || node_3->color == Color::BLACK);

    Node* child_1 = node_2->child(direction_of_red_child);
    Node* child_2 = node_2->child(Opposite(direction_of_red_child));
    Node* child_3 = node->child(Opposite(direction_of_red_child));

    *ref = node_2;
    node_2->color = Color::BLACK;
    node_2->child(direction_of_red_child) = child_1;
    node_2->child(Opposite(direction_of_red_child)) = node;
    node->color = Color::RED;
    node->child(direction_of_red_child) = child_2;
    node->child(Opposite(direction_of_red_child)) = child_3;
}

RBTreeImpl::ParentChild
RBTreeImpl::RefactorChildToBlack(Node* node,
                                 Node* parent,
                                 std::optional<Direction> parent_to_node,
                                 Node* grandparent,
                                 std::optional<Direction> grandparent_to_parent,
                                 Node** ref_grandparent,
                                 KeyType toward_key) {
    assert(node);
    assert(node->color == Color::BLACK);
    if (parent && parent->color == Color::RED) {
        assert(parent_to_node.has_value());
        if (grandparent) {
            assert(grandparent_to_parent.has_value());
            auto direction_of_parent_to_node = parent_to_node.value();
            auto direction_of_grandparent_to_parent = grandparent_to_parent.value();
            if (direction_of_parent_to_node == direction_of_grandparent_to_parent) {
                Refactor3Childs(grandparent, direction_of_grandparent_to_parent, ref_grandparent);
            } else {
                // refactor
                *ref_grandparent = node;
                auto child_1 = node->child(direction_of_parent_to_node);
                if (child_1) {
                    child_1->color = Color::BLACK;
                    grandparent->child(direction_of_grandparent_to_parent) = child_1;
                }
                grandparent->color = Color::RED;
                auto child_2 = node->child(direction_of_grandparent_to_parent);
                if (child_2) {
                    child_2->color = Color::BLACK;
                    parent->child(direction_of_parent_to_node) = child_2;
                }
                node->child(direction_of_parent_to_node) = grandparent;
                node->child(direction_of_grandparent_to_parent) = parent;
                // set vars
                parent = node;
                parent_to_node = parent->direction(toward_key);
                node = parent->child(parent_to_node.value());
            }
            assert(parent->color == Color::BLACK);
        } else {
            parent->color = Color::BLACK;
        }
    }
    node->color = Color::RED;
    if (node->lchild)
        node->lchild->color = Color::BLACK;
    if (node->rchild)
        node->rchild->color = Color::BLACK;
    return ParentChild{node, parent, parent_to_node};
}

void RBTreeImpl::Insert(KeyType key, ObjectType object) {
    auto new_node = new Node{key, object, nullptr, nullptr, Color::RED};
    if (!root) {
        root = new_node;
        return;
    }
    Node *current = root, *parent = nullptr, *grandparent = nullptr, **ref_grandparent = nullptr;
    std::optional<Direction> parent_to_current, grandparent_to_parent;
    auto direction = current->direction(key);
    Node* next = current->child(direction);
    if (!next) {
        current->color = Color::BLACK;
        current->child(direction) = new_node;
        return;
    }
    while (true) {
        if ((next && !next->child(next->direction(key)) && next->color == Color::RED) ||
            (current->lchild && current->lchild->color == Color::RED &&
             current->rchild && current->rchild->color == Color::RED)) {
            auto ret = RefactorChildToBlack(current, parent, parent_to_current,
                                            grandparent, grandparent_to_parent,
                                            ref_grandparent, key);
            current = ret.child;
            parent = ret.parent;
            parent_to_current = ret.parent_to_node;
        }
        if (!next) {
            next = current->child(direction) = next;
            return;
        }
        if (grandparent)
            ref_grandparent = &grandparent->child(grandparent_to_parent.value());
        grandparent = parent;
        parent = current;
        current = next;
        grandparent_to_parent = parent_to_current;
        parent_to_current = direction;
        direction = current->direction(key);
        next = current->child(direction);
    }
}

void RBTreeImpl::Delete(KeyType key) {
}

ObjectType RBTreeImpl::Find(KeyType key) {
    return ObjectType();
}

}  // namespace nictheboy
