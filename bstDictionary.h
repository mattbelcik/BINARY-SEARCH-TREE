#ifndef _BST_DICTIONARY_H
#define _BST_DICTIONARY_H

#include <cstdint>   // for uint32_t
#include <stdexcept> // for domain_error

static const uint32_t
    NULL_INDEX = 0xffffffff,
    DEFAULT_INITIAL_CAPACITY = 16;

template <typename KeyType, typename ValueType>
class BSTDictionary
{
public:
    uint32_t getCount(uint32_t root)
    {
        if (root == NULL_INDEX)
        {
            return 0;
        }
        else
        {
            return counts[root];
        }
    }

    uint32_t getHeight(uint32_t root)
    {
        if (root == NULL_INDEX)
        {
            return 0;
        }
        else
        {
            return heights[root];
        }
    }

    explicit BSTDictionary(uint32_t _cap = DEFAULT_INITIAL_CAPACITY)
    {
        // std::cout << "constructor" << std::endl;
        if (nTrees == 0)
        {
            counts = new uint32_t[_cap];
            heights = new uint32_t[_cap];
            left = new uint32_t[_cap];
            right = new uint32_t[_cap];
            keys = new KeyType[_cap];
            values = new ValueType[_cap];

            capacity = _cap;

            // generate free list

            uint32_t start = 0, end = _cap - 1;
            for (uint32_t i = 0; i < end; i++)
            {
                left[i] = i + 1;
            }
            left[end] = NULL_INDEX;
            freeListHead = start;
        }
        // cont.
        nTrees = nTrees + 1;

        root = NULL_INDEX;
    }

    ~BSTDictionary()
    {
        nTrees = nTrees - 1;

        if (nTrees == 0)
        {
            // delete shared arrays
            delete[] counts;
            delete[] heights;
            delete[] left;
            delete[] right;
            delete[] keys;
            delete[] values;
        }
        else
        {
            prvClear(root);
        }
    }

    void clear()
    {
        std::cout << "clear fxn " << std::endl;
        prvClear(root);
        root = NULL_INDEX;
    }

    uint32_t size()
    {
        /*    if (root == NULL_INDEX)
           {
               return 0;
           }

           else
           {
               return getCount(root);
           } */
        return getCount(root);
    }

    uint32_t height()
    {
        /*  if (root == NULL_INDEX)
         {
             return 0;
         }

         else
         {
             return getHeight(root);
         } */
        return getHeight(root);
    }

    bool isEmpty()
    {
        return root == NULL_INDEX;
    }

    ValueType &search(const KeyType &k)
    {
        uint32_t n = root;

        while (n != NULL_INDEX)
        {
            if (k == keys[n])
            {
                return values[n];
                // supposes to be else if
            }
            else if (k < keys[n])
            {
                n = left[n];
            }
            else
            {
                n = right[n];
            }
        }

        throw std::domain_error("Search: key not found");
    }

    ValueType &operator[](const KeyType &k)
    {
        // std::cout << "DEBUG operator" << std::endl;

        // could be int instead
        uint32_t tmp = prvAllocate();

        uint32_t n = tmp;

        root = prvInsert(root, n, k);

        if (n != tmp)
        {
            prvFree(tmp);
        }

        return values[n];
    }

    void remove(const KeyType &k)
    {
        // std::cout << "DEBUG void remove" << std::endl;

        uint32_t ntbd;

        try
        {
            search(k);
        }
        catch (std::domain_error e)
        {
            throw std::domain_error("remove: key not found");
        }

        // search(k);

        root = prvRemove(root, ntbd, k);

        prvFree(ntbd);
    }

private:
    uint32_t
        root; // tree root

    static uint32_t   // this is the shared data
        *counts,      // counts for each node
        *heights,     // heights for each node
        *left,        // left node indexes
        *right,       // right node indexes
        nTrees,       // number of BSTs with this key and value type
        capacity,     // size of the arrays
        freeListHead; // the head of the unused node list (the free list)

    static KeyType
        *keys; // pool of keys
    static ValueType
        *values; // pool of values

    uint32_t prvAllocate()
    {
        // std::cout << "DEBUG PRVALLO" << std::endl;

        if (freeListHead == NULL_INDEX)
        {
            /* Allocate temporary arrays with 2 · capcaty elements
             Copy data from old arrays to temporary arrays
             Delete old arrays
             Point shared pointers to temporary arrays
             Regenerate the free list */

            auto tmpCounts = new uint32_t[2 * capacity];
            auto tmpHeights = new uint32_t[2 * capacity];
            auto tmpLeft = new uint32_t[2 * capacity];
            auto tmpRight = new uint32_t[2 * capacity];
            auto tmpKeys = new KeyType[2 * capacity];
            auto tmpValues = new ValueType[2 * capacity];

            // for (int i = 0; i < capacity; i++)
            for (int i = 0; i < capacity; i++)
            {
                tmpCounts[i] = counts[i];
                tmpHeights[i] = heights[i];
                tmpLeft[i] = left[i];
                tmpRight[i] = right[i];
                tmpKeys[i] = keys[i];
                tmpValues[i] = values[i];
                // finsih for all 6
            }

            delete[] counts;
            delete[] heights;
            delete[] left;
            delete[] right;
            delete[] keys;
            delete[] values;

            counts = tmpCounts;
            heights = tmpHeights;
            left = tmpLeft;
            right = tmpRight;
            keys = tmpKeys;
            values = tmpValues;

            // uint32_t start = capacity, end = (2 * capacity) - 1;
            // uint32_t start = capacity, end = (2 * capacity - 1);
            for (uint32_t i = capacity; i < 2 * capacity - 1; i++)
            {
                left[i] = i + 1;
            }
            left[2 * capacity - 1] = NULL_INDEX;
            // 0 could b start
            freeListHead = capacity;

            capacity = 2 * capacity;
        }

        uint32_t tmp = freeListHead;
        freeListHead = left[freeListHead];

        left[tmp] = NULL_INDEX;
        right[tmp] = NULL_INDEX;
        counts[tmp] = 1;
        heights[tmp] = 1;

        return tmp;
    }

    void prvFree(uint32_t n)
    {
        // std::cout << "DEBUG prv free(is good)" << std::endl;
        left[n] = freeListHead;

        freeListHead = n;
    }

    void prvClear(uint32_t r)
    {
        // std::cout << "DEBUG prv clear(is good)" << std::endl;
        if (r != NULL_INDEX)
        {

            prvClear(left[r]);
            prvClear(right[r]);
            prvFree(r);
        }
    }

    void prvAdjust(uint32_t r)
    {
        // std::cout << "DEBUG prv adjust(is good)" << std::endl;
        counts[r] = getCount(left[r]) + getCount(right[r]) + 1;

        uint32_t lh = getHeight(left[r]), rh = getHeight(right[r]);
        {
            if (lh > rh)
            {
                heights[r] = lh + 1;
            }
            else
            {
                heights[r] = rh + 1;
            }
        }

        // heights[r] = max(getHeight(left[r]), getHeight(right[r])) + 1;
    }

    uint32_t prvInsert(uint32_t r, uint32_t &n, const KeyType &k)
    {
        //  std::cout << "DEBUG prv insert" << std::endl;

        if (r == NULL_INDEX)
        {
            keys[n] = k;

            return n;
        }
        if (k == keys[r])
        {
            n = r;
        }
        // supposed to be else if
        else if (k < keys[r])
        {
            left[r] = prvInsert(left[r], n, k);
        }
        else
        {
            right[r] = prvInsert(right[r], n, k);
        }

        prvAdjust(r);

        return r;
    }

    uint32_t prvRemove(uint32_t r, uint32_t &ntbd, const KeyType &k)
    {
        // std::cout << "DEBUG prv remove" << std::endl;
        if (r == NULL_INDEX)
        {
            throw std::domain_error("Remove: key not found");
        }

        if (k < keys[r])
        {
            left[r] = prvRemove(left[r], ntbd, k);
            // supposed to be else if
        }
        else if (k > keys[r])
        {
            right[r] = prvRemove(right[r], ntbd, k);
        }
        else
        {
            ntbd = r;

            if (left[r] == NULL_INDEX)
            {
                if (right[r] == NULL_INDEX)
                {
                    r = NULL_INDEX;
                }
                else
                {
                    r = right[r];
                }
            }
            else
            {
                if (right[r] == NULL_INDEX)
                {
                    r = left[r];
                }
                else
                {
                    // reduce two-child case to one child case
                    if (getHeight(right[r]) > getHeight(left[r]))
                    {

                        uint32_t tmp = right[r];

                        while (left[tmp] != NULL_INDEX)
                        {
                            tmp = left[tmp];
                        }
                        // swap keys[r] and keys[tmp]
                        // swap values[r] and values[tmp]

                        KeyType temp = keys[r];
                        keys[r] = keys[tmp];
                        keys[tmp] = temp;

                        ValueType temporary = values[r];
                        values[r] = values[tmp];
                        values[tmp] = temporary;

                        right[r] = prvRemove(right[r], ntbd, k);
                    }
                    else
                    {

                        uint32_t tmp = left[r];

                        while (right[tmp] != NULL_INDEX)
                        {
                            tmp = right[tmp];
                        }
                        // swap keys[r] and keys[tmp]
                        // swap values[r] and values[tmp]

                        KeyType temp = keys[r];
                        keys[r] = keys[tmp];
                        keys[tmp] = temp;

                        ValueType temporary = values[r];
                        values[r] = values[tmp];
                        values[tmp] = temporary;

                        left[r] = prvRemove(left[r], ntbd, k);
                    }
                }
            }
        }
        if (r != NULL_INDEX)
        {
            prvAdjust(r);
        }
        return r;
    }
};

template <typename KeyType, typename ValueType>
uint32_t *BSTDictionary<KeyType, ValueType>::counts = nullptr;

template <typename KeyType, typename ValueType>
uint32_t *BSTDictionary<KeyType, ValueType>::heights = nullptr;

template <typename KeyType, typename ValueType>
uint32_t *BSTDictionary<KeyType, ValueType>::left = nullptr;

template <typename KeyType, typename ValueType>
uint32_t *BSTDictionary<KeyType, ValueType>::right = nullptr;

template <typename KeyType, typename ValueType>
uint32_t BSTDictionary<KeyType, ValueType>::nTrees = 0;

template <typename KeyType, typename ValueType>
uint32_t BSTDictionary<KeyType, ValueType>::capacity = 0;

template <typename KeyType, typename ValueType>
uint32_t BSTDictionary<KeyType, ValueType>::freeListHead = 0;

template <typename KeyType, typename ValueType>
KeyType *BSTDictionary<KeyType, ValueType>::keys = nullptr;

template <typename KeyType, typename ValueType>
ValueType *BSTDictionary<KeyType, ValueType>::values = nullptr;

#endif