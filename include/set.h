#pragma once
#include <optional>
#include "types.h"

namespace nictheboy {

class Set {
   public:
    virtual ~Set() = default;
    virtual void Insert(KeyType key, ObjectType object) = 0;
    virtual void Delete(KeyType key) = 0;
    virtual std::optional<ObjectType> Find(KeyType key) = 0;
};

}  // namespace nictheboy
