# if gtest not added:
if(NOT TARGET gtest)
    add_subdirectory(../third-party/googletest ${CMAKE_BINARY_DIR}/third-party/googletest/googletest EXCLUDE_FROM_ALL)
endif()

if(NOT TARGET rbtree-nictheboy-test)
    add_executable(rbtree-nictheboy-test rbtree_test.cpp)
    target_link_libraries(rbtree-nictheboy-test PRIVATE rbtree-nictheboy gtest gtest_main)
    add_test(NAME rbtree-nictheboy-test COMMAND rbtree-nictheboy-test)
endif()
