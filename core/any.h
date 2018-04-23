#pragma once

#include <type_traits>
#include <utility>
#include <typeinfo>
#include <cassert>
#include "log.h"

namespace dronecore {

// Any class taken from:
// https://codereview.stackexchange.com/questions/20058/c11-any-class


template <class T>
using StorageType = typename std::decay<T>::type;

struct Any {
    bool is_null() const { return !ptr; }
    bool not_null() const { return ptr; }

    template<typename U> Any(U &&value)
        : ptr(new Derived<StorageType<U>>(std::forward<U>(value))) {}

    template<class U> bool is() const
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*>(ptr);

        return derived;
    }

    template<class U>
    StorageType<U> &as() const
    {
        typedef StorageType<U> T;

        auto derived = dynamic_cast<Derived<T>*>(ptr);

        if (!derived) {
            // FIXME: We don't have exceptions, so this is commented out
            //        and we'll abort instead.
            //throw bad_cast();
            LogErr() << "Need to abort because of a bad_cast";
            abort();
        }

        return derived->value;
    }

    template<class U>
    operator U() const
    {
        return as<StorageType<U>>();
    }

    Any()
        : ptr(nullptr) {}

    Any(Any &that)
        : ptr(that.clone()) {}

    Any(Any &&that)
        : ptr(that.ptr)
    {
        that.ptr = nullptr;
    }

    Any(const Any &that)
        : ptr(that.clone()) {}

    Any(const Any &&that)
        : ptr(that.clone()) {}

    Any &operator=(const Any &a)
    {
        if (ptr == a.ptr) {
            return *this;
        }

        auto old_ptr = ptr;

        ptr = a.clone();

        delete old_ptr;

        return *this;
    }

    Any &operator=(Any &&a)
    {
        if (ptr == a.ptr) {
            return *this;
        }

        std::swap(ptr, a.ptr);

        return *this;
    }

    ~Any()
    {
        delete ptr;
    }

private:
    struct Base {
        virtual ~Base() {}

        virtual Base *clone() const = 0;
    };

    template<typename T>
    struct Derived : Base {
        template<typename U> Derived(U &&value_tmp) : value(std::forward<U>(value_tmp)) { }

        T value;

        Base *clone() const { return new Derived<T>(value); }
    };

    Base *clone() const
    {
        if (ptr) {
            return ptr->clone();
        } else {
            return nullptr;
        }
    }

    Base *ptr;
};

} // namespace dronecore
