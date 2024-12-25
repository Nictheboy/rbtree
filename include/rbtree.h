#pragma once
#include "set.h"

namespace nictheboy {

class RBTreeImpl;
class RBTree : Set {
   private:
    RBTreeImpl* impl;

   public:
    RBTree();
    virtual ~RBTree();
    virtual void Insert(KeyType key, ObjectType object);
    virtual void Delete(KeyType key);
    virtual ObjectType Find(KeyType key);
};

}  // namespace nictheboy
