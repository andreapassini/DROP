#pragma once

#include <cstdint>

// Not thread safe implementation
// we can keep it fast without using mutex
// https://refactoring.guru/design-patterns/singleton/cpp/example
/**
 * The Singleton class defines the `GetInstance` method that serves as an
 * alternative to constructor and lets clients access the same instance of this
 * class over and over.
 */
class UniqueIndex
{

/**
 * The Singleton's constructor should always be private to prevent direct
 * construction calls with the `new` operator.
 */

protected:
    UniqueIndex(const uint32_t index_val) : index(index_val)
    {
    }

    static UniqueIndex* instance;

    uint32_t index;

public:

    /**
     * Singletons should not be cloneable.
     */
    UniqueIndex(UniqueIndex& other) = delete;
    /**
     * Singletons should not be assignable.
     */
    void operator=(const UniqueIndex&) = delete;
    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates a singleton object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     */

    static UniqueIndex* GetInstance();

    int32_t value() {
        int32_t ret = index;
        index++;
        return ret;
    }

};

