#pragma once

#include <set>
#include "set.h"

using namespace nictheboy;

class StandardSet : public Set {
   private:
    struct Element {
        KeyType key;
        ObjectType object;

        bool operator<(const Element& another) const { return key < another.key; }
    };
    std::set<Element> impl;

   public:
    virtual ~StandardSet() = default;
    virtual void Insert(KeyType key, ObjectType object) {
        impl.insert(Element{key, object});
    }
    virtual void Delete(KeyType key) {
        impl.erase(Element{key, nullptr});
    }
    virtual std::optional<ObjectType> Find(KeyType key) const {
        auto ret = impl.find(Element{key, nullptr});
        return ret != impl.end() ? std::make_optional(ret->object) : std::nullopt;
    }
};
