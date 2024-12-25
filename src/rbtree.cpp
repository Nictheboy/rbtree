#include "rbtree.h"
#include <stdio.h>
#include "private-include/rbtree_impl.h"

namespace nictheboy {

RBTree::RBTree() {
    this->impl = new RBTreeImpl();
}

RBTree::~RBTree() {
    delete this->impl;
}

void RBTree::Insert(KeyType key, ObjectType object) {
    printf("Insert(%lld, %p)\n", key, object);
    this->impl->Insert(key, object);
    this->impl->Debug();
}

void RBTree::Delete(KeyType key) {
    this->impl->Delete(key);
}

std::optional<ObjectType> RBTree::Find(KeyType key) {
    return this->impl->Find(key);
}

}  // namespace nictheboy
