#pragma once
#include "types.h"

namespace nictheboy {

class Set {
   public:
    virtual ~Set() = default;
    virtual void Insert(KeyType key, ObjectType object) = 0;
    virtual void Delete(KeyType key) = 0;
    virtual ObjectType Find(KeyType key) = 0;
};

}  // namespace nictheboy