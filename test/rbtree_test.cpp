#include "rbtree.h"
#include <gtest/gtest.h>
#include <memory>
#include <random>
#include "standard_set.h"

void test_set(std::shared_ptr<nictheboy::Set> target,
              std::shared_ptr<nictheboy::Set> standard,
              int key_max,
              int repetition_count) {
    for (int i = 0; i < repetition_count; i++) {
        auto random_key = std::rand() % key_max;
        auto ret_1 = target->Find(random_key);
        auto ret_2 = standard->Find(random_key);
        ASSERT_EQ(ret_1.has_value(), ret_2.has_value());
        if (ret_1.has_value()) {
            ASSERT_EQ(*ret_1, *ret_2);
        } else {
            auto random_object = (nictheboy::ObjectType)(long long)std::rand();
            target->Insert(random_key, random_object);
            standard->Insert(random_key, random_object);
        }
    }
}

TEST(RBTreeTest, TestInsert) {
    auto target = std::make_shared<nictheboy::RBTree>();
    auto standard = std::make_shared<StandardSet>();
    for (int i = 0; i < 100; i++)
        test_set(target, standard, 10000, 100000);
}
