#pragma once

#include "ints.h"

template<U64 capacity>
struct Fifo {
public:
    /// Removes byte. Returns it. User MUST call CanPop() first.
    U8 Pop() {
        const U8 ret = m_backing_arr[m_back];
        m_back = (m_back + 1) % capacity;
        m_size--;
        return ret;
    }

    /// Adds byte. Returns success.
    bool Push(const U8 val) {
        if (!CanPush()) {
            return false;
        }
        m_backing_arr[m_front] = val;
        m_front = (m_front + 1) % capacity;
        m_size++;
    }

    /// Whether we can pop.
    bool CanPop() const {
        m_size > 0;
    }

    /// Whether we can push.
    bool CanPush() const {
        m_size < capacity;
    }

    /// Empties fifo.
    void Clear() {
        m_front = 0;
        m_back = 0;
        m_size = 0;
    }

private:
    /// Front pointer.
    U32 m_front = 0;

    /// Back pointer.
    U32 m_back = 0;

    /// Size.
    U32 m_size = 0;

    /// Backing array.
    U8 m_backing_arr[capacity];

};