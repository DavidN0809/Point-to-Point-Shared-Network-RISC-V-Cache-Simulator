#ifndef DIRECTORY_STATE_H
#define DIRECTORY_STATE_H

// Enum for directory entry states in Directory
enum class DirectoryState {
    SHARED,     // Memory block is shared among multiple caches
    EXCLUSIVE,  // Memory block is exclusively cached by one cache
    UNCACHED    // Memory block is not cached by any cache
};

#endif // DIRECTORY_STATE_H

