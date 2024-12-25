#pragma once
#include "set.h"

namespace nictheboy {

class RBTreeImpl;
class RBTree : public Set {
   private:
    RBTreeImpl* impl;

   public:
    RBTree();
    virtual ~RBTree();
    virtual void Insert(KeyType key, ObjectType object);
    virtual void Delete(KeyType key);
    virtual std::optional<ObjectType> Find(KeyType key);
};

}  // namespace nictheboy
