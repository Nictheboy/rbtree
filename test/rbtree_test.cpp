#include "rbtree.h"
#include <gtest/gtest.h>
#include <memory>

TEST(RBTreeTest, TestInsert) {
    auto tree = std::make_unique<nictheboy::RBTree>();
    for (int i = 0; i < 1000; i++)
        tree->Insert(i, nullptr);
}
