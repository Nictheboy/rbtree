#include "rbtree.h"
#include <gtest/gtest.h>
#include <memory>

TEST(RBTreeTest, Hello) {
    auto tree = std::make_unique<RBTree>();
    ASSERT_EQ(tree->hello(), true);
}
