#include "UniqueIndex.h"

UniqueIndex* UniqueIndex::instance = nullptr;;

/**
 * Static methods should be defined outside the class.
 */
UniqueIndex* UniqueIndex::GetInstance()
{
    /**
     * This is a safer way to create an instance. instance = new Singleton is
     * dangeruous in case two instance threads wants to access at the same time
     */
    if (instance == nullptr) {
        instance = new UniqueIndex(0);
    }
    return instance;
}