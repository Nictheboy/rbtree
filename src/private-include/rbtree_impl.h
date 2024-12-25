#pragma once
#include "types.h"

namespace nictheboy {

class RBTreeImpl {
   private:
   public:
    RBTreeImpl();
    virtual ~RBTreeImpl();
    virtual void Insert(KeyType key, ObjectType object);
    virtual void Delete(KeyType key);
    virtual ObjectType Find(KeyType key);
};

};  // namespace nictheboy