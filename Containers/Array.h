#ifndef ARRAY_H
#define ARRAY_H

#include <iostream>
#include "ArrayADT.h"

template <typename T>
class Array: public ArrayADT<T>
{
    enum { MAX_SIZE = 1000 };
private:
    T items[MAX_SIZE];
    int count;

public:
    Array();
    bool isEmpty() const;
    bool insert(const T& newEntry);
    bool remove(int index, T& entry);
    bool entryAt(T& entry, int index) const;
    void printList() const;
    int getCount() const;
};

template <typename T>
inline Array<T>::Array(): count(0)
{}

template <typename T>
inline bool Array<T>::isEmpty() const
{
    return (count == 0);
}

template <typename T>
inline bool Array<T>::insert(const T& newEntry)
{
    if (count == MAX_SIZE - 1) return false;

    items[count++] = newEntry;
    return true;
}

template <typename T>
inline bool Array<T>::remove(int index, T& entry)
{
    if (index >= count) return false;

    entry = items[index];
    items[index] = items[count - 1];
    items[count - 1] = T();
    count--;

    return true;
}

template <typename T>
inline bool Array<T>::entryAt(T& entry, int index) const
{
    if (isEmpty()) return false;

    entry = items[index];
    return true;
}

template <typename T>
inline void Array<T>::printList() const
{
    for (int i = 0; i < count; i++)
    {
        std::cout << items[i];
        if (i != count - 1)
            std::cout << ", ";
    }
}

template <typename T>
inline int Array<T>::getCount() const
{
    return count;
}

#endif