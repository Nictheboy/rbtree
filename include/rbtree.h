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
    virtual void Insert(KeyType key, ObjectType object) override;
    virtual void Delete(KeyType key) override;
    virtual std::optional<ObjectType> Find(KeyType key) const override;
};

}  // namespace nictheboy
