#include "rbtree.h"
#include "private-include/rbtree_impl.h"

namespace nictheboy {

RBTree::RBTree() {
    this->impl = new RBTreeImpl();
}

RBTree::~RBTree() {
    delete this->impl;
}

void RBTree::Insert(KeyType key, ObjectType object) {
    this->impl->Insert(key, object);
}

void RBTree::Delete(KeyType key) {
    this->impl->Delete(key);
}

ObjectType RBTree::Find(KeyType key) {
    return this->impl->Find(key);
}

}  // namespace nictheboy
