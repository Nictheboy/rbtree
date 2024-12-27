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
#ifdef NDEBUG
    this->impl->Insert(key, object);
#else
    printf("Insert(%lld, %p)\n", key, object);
    this->impl->Insert(key, object);
    this->impl->Debug();
#endif
}

void RBTree::Delete(KeyType key) {
#ifdef NDEBUG
    this->impl->Delete(key);
#else
    printf("Delete(%lld)\n", key);
    this->impl->Delete(key);
    this->impl->Debug();
#endif
}

std::optional<ObjectType> RBTree::Find(KeyType key) {
    return this->impl->Find(key);
}

}  // namespace nictheboy
