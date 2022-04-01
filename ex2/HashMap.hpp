#ifndef EX6_HASHMAP_HPP
#define EX6_HASHMAP_HPP
#include <list>
#include <cstdlib>
#include <functional>
#include <algorithm>
#include <exception>
#include <utility>

#define DEFAULT_CAPACITY 16
#define EMPTY_SIZE 0
#define LOWER_LOAD_FACTOR 0.25
#define UPPER_LOAD_FACTOR 0.75
#define REHASH_UP_FACTOR 2
#define REHASH_DOWN_FACTOR 0.5
#define FIRST_IDX 0
#define ROUND_UP 0.5
#define INVALID_INPUT_EXC "Invalid input"
#define NO_EXIST_KEY "key does not exist"

/**
 * This class represents a hash map
 * @tparam KeyT - the key
 * @tparam ValueT - the value
 */
template <typename KeyT, typename ValueT>
class HashMap
{

private:

    /**
     * This class represents a const iterator of hash map
     */
    class ConstIterator
    {
    private:
        HashMap<KeyT, ValueT> _hashMap; // the hash map we are iterating
        std::pair<KeyT, ValueT> * _cur; // a pointer to the current pair of <KeyT, ValueT>
        int _curIdx; // the index in the _hashMap._hashTable we are in
        typename std::list<std::pair<KeyT, ValueT>>::iterator _curListIter; // the list iterator in
        // _hashMap._hashTable[_curIdx]

        /**
         * Helper function for ++ (pre and post) operators
         */
        void _advanceIterator() noexcept
        {
            _curListIter++;
            if (_curListIter != _hashMap._hashTable[_curIdx].end())
            {
                _cur = &(*_curListIter);
            }
            else // we've reached the end of the current list
            {
                _curIdx++;
                // go to the next non-empty list
                while (_curIdx < _hashMap.capacity() && _hashMap._hashTable[_curIdx].empty())
                {
                    _curIdx++;
                }
                if (_curIdx >= _hashMap.capacity()) // we've reached the end of the map
                {
                    _cur = nullptr;
                }
                else
                {
                    _curListIter = _hashMap._hashTable[_curIdx].begin();
                    _cur = &(*_curListIter);
                }
            }
        }


    public:
        typedef std::pair<KeyT, ValueT> value_type;
        typedef const std::pair<KeyT, ValueT>& reference;
        typedef const std::pair<KeyT, ValueT>* pointer;
        typedef int difference_type;
        typedef std::forward_iterator_tag iterator_category;

        /**
         * Default constructor of ConstIterator
         */
        ConstIterator() : _cur(nullptr), _curIdx(FIRST_IDX) {}

        /**
         * Constructor of ConstIterator
         * @param hashMap - the hash map we are iterating
         */
        explicit ConstIterator(const HashMap<KeyT, ValueT>& hashMap) : _hashMap(hashMap), _curIdx(FIRST_IDX)
        {
            if (_hashMap.empty())
            {
                _cur = nullptr;
            }
            else // go to first element in the map
            {
                while (_hashMap._hashTable[_curIdx].empty())
                {
                    _curIdx++;
                }
                _curListIter = _hashMap._hashTable[_curIdx].begin();
                _cur = &(*_curListIter);
            }
        }

        /**
         * Operator *
         * @return a pair of <KeyT, ValueT>
         */
        value_type operator*() const noexcept
        {
            return *_cur;
        }

        /**
         * Operator ++ (prefix)
         * @return a reference to ConstIterator
         */
        ConstIterator& operator++() noexcept
        {
            _advanceIterator();
            return *this;
        }

        /**
         * Operator ++ (postfix)
         * @return ConstIterator
         */
        ConstIterator operator++(int) noexcept
        {
            ConstIterator tmp = *this;
            _advanceIterator();
            return tmp;
        }

        /**
         * Operator ==
         * @param other - another ConstIterator to compare to
         * @return true if they are the same iterator, false otherwise
         */
        bool operator==(const ConstIterator& other) const noexcept
        {
            return this->_cur == other._cur;
        }

        /**
         * Operator !=
         * @param other - another ConstIterator to compare to
         * @return true if they are not the same iterator, false otherwise
         */
        bool operator!=(const ConstIterator& other) const noexcept
        {
            return this->_cur != other._cur;
        }

        /**
         * Operator ->
         * @return pointer to the element pointed to by the iterator
         */
        pointer operator->() const noexcept
        {
            return &(*_cur);
        }
    };

    size_t _size{}, _capacity{};
    std::list<std::pair<KeyT, ValueT>> * _hashTable; // an array of lists
    int _upperSizeLimit{};  // max allowed size (calculated only once in _init)
    int _lowerSizeLimit{};  // min allowed size (calculated only once in _init)

    /**
     * This function initiates a hash map
     * @param capacity - the capacity of the hash map
     */
    void _init(size_t capacity) noexcept(false)
    {
        _capacity = capacity;
        _hashTable = new std::list<std::pair<KeyT, ValueT>>[_capacity];
        _size = EMPTY_SIZE;
        _upperSizeLimit = (int) (_capacity * UPPER_LOAD_FACTOR);
        _lowerSizeLimit = (int) (_capacity * LOWER_LOAD_FACTOR + ROUND_UP); // round up
    }

    /**
     * This function resizes and rehashes the elements
     * @param factor - the capacity will be changed according to this factor
     * @return true if no exception was thrown, false otherwise
     */
    bool _reHash(const float& factor) noexcept(false)
    {
        std::list<std::pair<KeyT, ValueT>> * oldHashTable = _hashTable;
        size_t oldCapacity = _capacity;
        size_t newCapacity = _capacity * factor;
        _init(newCapacity);
        // add all oldHashTable elements to the new hashTable
        //  _reHash will never be called from _addNew since we already rehashed !
        for (int i = 0; i < (int)oldCapacity; i++)
        {
            for (auto lItr = oldHashTable[i].begin(); lItr != oldHashTable[i].end(); lItr++)
            {
                _addNew(lItr->first, lItr->second);
            }
        }
        delete[] oldHashTable;
        return true;
    }

    /**
     * This function overrides the value of the given key, by the given value (the map contains the key)
     * @param key - KeyT
     * @param value - ValueT
     */
    void _overrideExisting(const KeyT& key, const ValueT& value) noexcept
    {
        size_t hashIdx = _findHash(key);
        for (auto i = _hashTable[hashIdx].begin(); i != _hashTable[hashIdx].end(); i++)
        {
            if (i->first == key)
            {
                i->second = value;
                break;
            }
        }
    }

    /**
     *  This function adds a new pair to the map
     * @param key - KeyT
     * @param value - ValueT
     */
    void _addNew(const KeyT& key, const ValueT& value) noexcept(false)
    {
        if ((int)_size + 1 > _upperSizeLimit) // we need to rehash
        {
            _reHash(REHASH_UP_FACTOR);
        }
        std::pair<KeyT, ValueT> addedPair(key, value); // create a pair with <key, value>
        size_t hashIdx = _findHash(key);
        _hashTable[hashIdx].push_back(addedPair); // we already know that key did not exist in the map
        _size++;
    }

    /**
     * This function adds a new pair (also checks if the map contains the key)
     * @param key - KeyT
     * @param value - ValueT
     */
    void _addAllowOverride(const KeyT& key, const ValueT& value) noexcept(false)
    {
        if (contains_key(key))
        {
            _overrideExisting(key, value);
        }
        else
        {
            _addNew(key, value);
        }
    }

    /**
     * This function finds the hash of the given key
     * @param key - KeyT
     * @return the index according to the hash
     */
    size_t _findHash(const KeyT& key) const noexcept
    {
        return std::hash<KeyT>{}(key) & (_capacity - 1);
    }

public:

    typedef ConstIterator const_iterator;
    typedef ConstIterator iterator;

    /**
     * Default constructor of HashMap
     */
    HashMap() noexcept(false)
    {
        _init(DEFAULT_CAPACITY);
    }

    /**
     * Constructor of HashMap
     * @tparam KeysInputIterator
     * @tparam ValuesInputIterator
     * @param keysBegin - begin input iterator of keys
     * @param keysEnd - end input iterator of keys
     * @param valuesBegin - begin input iterator of values
     * @param valuesEnd - end input iterator of values
     */
    template <typename KeysInputIterator, typename ValuesInputIterator>
    HashMap(const KeysInputIterator keysBegin, const KeysInputIterator keysEnd, const ValuesInputIterator valuesBegin,
              const ValuesInputIterator valuesEnd) noexcept(false)
    {
        int numOfKeys = 0;
        int numOfVals = 0;
        for (auto it = keysBegin; it != keysEnd; it++)
        {
            numOfKeys++;
        }

        for (auto it = valuesBegin; it != valuesEnd; it++)
        {
            numOfVals++;
        }

        if (numOfKeys != numOfVals)
        {
            throw std::invalid_argument(INVALID_INPUT_EXC);
        }

        _init(DEFAULT_CAPACITY);

        for (auto ik = keysBegin, iv = valuesBegin; ik != keysEnd; ik++, iv++)
        {
            _addAllowOverride(*ik, *iv);
        }
    }

    /**
     * Copy constructor of HashMap
     * @param rhs - the map we are copying
     */
    HashMap(const HashMap<KeyT, ValueT>& rhs) noexcept(false)
    {
        _init(rhs._capacity);
        for (int i = 0; i < (int)_capacity; i++)
        {
            _hashTable[i] = rhs._hashTable[i]; // deep copy of the bucket
        }
        _size = rhs._size;
    }

    /**
     * Destructor of HashMap
     */
    ~HashMap()
    {
        delete [] _hashTable;
    }

    /**
     * This function returns the number of elements in the map
     * @return number of elements in the map
     */
    size_t size() const noexcept { return _size; }

    /**
     * This function returns the capacity of the map
     * @return the capacity of the map
     */
    size_t capacity() const noexcept { return _capacity; }

    /**
     * This function checks if the map is empty
     * @return true if empty, false otherwise
     */
    bool empty() const noexcept { return _size == EMPTY_SIZE; }

    /**
     * This function inserts a pair of <KeyT, ValueT> to the map
     * @param key - the KeyT to be inserted
     * @param value - the ValueT to be inserted
     * @return true if insertion succeeded, false otherwise
     */
    bool insert(const KeyT& key, const ValueT& value) noexcept(false)
    {
        if (contains_key(key))
        {
            return false;
        }
        _addNew(key, value); // new key, may insert
        return true;
    }

    /**
     * This function checks if the map contains a given key
     * @param key - we will check if the map contains this key
     * @return true if the map contains the key, false otherwise
     */
    bool contains_key(const KeyT& key) const noexcept
    {
        size_t hashIdx = _findHash(key);
        for (auto i = _hashTable[hashIdx].begin(); i != _hashTable[hashIdx].end(); i++)
        {
            if (key == i->first)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * This function gets a key and returns the value that matches the key
     * @param key - KeyT
     * @return the value that matches the key
     */
    ValueT at(const KeyT& key) const noexcept(false)
    {
        if (!contains_key(key))
        {
            throw std::out_of_range(NO_EXIST_KEY);
        }
        size_t hashIdx = _findHash(key);
        for (auto i = _hashTable[hashIdx].begin(); i != _hashTable[hashIdx].end(); i++)
        {
            if (key == i->first)
            {
                return i->second;
            }
        }
    }

    /**
    * This function gets a key and returns the reference to value that matches the key
    * @param key - KeyT
    * @return the reference to value that matches the key
    */
    ValueT& at(const KeyT& key) noexcept(false)
    {
        if (!contains_key(key))
        {
            throw std::out_of_range(NO_EXIST_KEY);
        }
        size_t hashIdx = _findHash(key);
        for (auto i = _hashTable[hashIdx].begin(); i != _hashTable[hashIdx].end(); i++)
        {
            if (key == i->first)
            {
                return i->second;
            }
        }

        // we are never going to reach these next 2 lines
        auto * dummyPtr = new ValueT;
        return *dummyPtr;
    }

    /**
     * This function erases a pair from the map
     * @param key - KeyT
     * @return true if erased succefully, false otherwise
     */
    bool erase(const KeyT& key) noexcept(false)
    {
        if (!contains_key(key))
        {
            return false;
        }
        size_t hashIdx = _findHash(key);
        for (auto i = _hashTable[hashIdx].begin(); i != _hashTable[hashIdx].end(); i++)
        {
            if (i->first == key)
            {
                _hashTable[hashIdx].erase(i);
                _size--;
                break;
            }
        }

        if (_size < _lowerSizeLimit) // we need to rehash
        {
            _reHash(REHASH_DOWN_FACTOR);
        }
        return true;
    }

    /**
     * This function returns the load factor
     * @return the load factor
     */
    double load_factor() const noexcept { return (double)_size / _capacity; }

    /**
     * This function returns the size of the bucket of the given key
     * @param key - KeyT
     * @return the size of the bucket of the given key
     */
    size_t bucket_size(const KeyT& key) const noexcept(false)
    {
        if (!contains_key(key))
        {
            throw std::out_of_range(NO_EXIST_KEY);
        }
        size_t hashIdx = _findHash(key);
        return _hashTable[hashIdx].size();
    }

    /**
     * This function returns the index of the bucket of the given key
     * @param key - KeyT
     * @return the index of the bucket of the given key
     */
    size_t bucket_index(KeyT key) const noexcept(false)
    {
        if (!contains_key(key))
        {
            throw std::out_of_range(NO_EXIST_KEY);
        }
        return _findHash(key);
    }

    /**
     * This function clears the map
     */
    void clear() noexcept
    {
        for (int i = 0; i < (int)_capacity; i++)
        {
            _hashTable[i].clear();
        }
        _size = EMPTY_SIZE;
    }

    /**
     * Operator =
     * @param rhs - the map we are assigning from
     * @return a reference to this map
     */
    HashMap& operator=(const HashMap<KeyT, ValueT>& rhs) noexcept(false)
    {
        if (this != &rhs)
        {
            delete [] _hashTable;
            _init(rhs._capacity);

            for (int i = 0; i < _capacity; i++)
            {
                _hashTable[i] = rhs._hashTable[i]; // deep copy of the bucket
            }
            _size = rhs._size;
        }
        return *this;
    }

    /**
     * Operator [] (const)
     * @param key - KeyT
     * @return the value that matches the given key
     */
    ValueT operator[](const KeyT& key) const noexcept
    {
        if (!contains_key(key))
        {
            ValueT newVal;
            return newVal;
        }
        size_t hashIdx = _findHash(key);
        for (auto i = _hashTable[hashIdx].begin(); i != _hashTable[hashIdx].end(); i++)
        {
            if (key == i->first)
            {
                return i->second;
            }
        }
    }

    /**
     * Operator [] (non-const)
     * @param key - KeyT
     * @return a reference to the value that matches the given key
     */
    ValueT& operator[](const KeyT& key) noexcept(false)
    {
        if (!contains_key(key))
        {
            ValueT newVal;
            _addNew(key, newVal);
        }

        size_t hashIdx = _findHash(key);
        for (auto i = _hashTable[hashIdx].begin(); i != _hashTable[hashIdx].end(); i++)
        {
            if (key == i->first)
            {
                return i->second;
            }
        }
        // we are never going to reach these next 2 lines
        auto * dummyPtr = new ValueT;
        return *dummyPtr;
    }

    /**
     * Operator ==
     * @param rhs - the map to compare to
     * @return true if they are the same mao, false otherwise
     */
    bool operator==(const HashMap& rhs) const noexcept
    {
        if (_capacity != rhs._capacity || _size != rhs._size)
        {
            return false;
        }

        for (int i = 0; i < (int)_capacity; i++)
        {
            if (_hashTable[i].size() != rhs._hashTable[i].size())
            {
                return false;
            }

            for (auto iter = _hashTable[i].begin(); iter != _hashTable[i].end(); iter++)
            {
                auto rhsIter = std::find(rhs._hashTable[i].begin(), rhs._hashTable[i].end(), *iter);
                if (rhsIter == rhs._hashTable[i].end())
                {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Operator !=
     * @param rhs - the map to compare to
     * @return true if they are not the same map, false otherwise
     */
    bool operator!=(const HashMap& rhs) const noexcept
    {
        return !((*this) == rhs);
    }

    /**
     * This function returns a const iterator to beginning of the map
     * @return const iterator to beginning of the map
     */
    const_iterator begin() const noexcept
    {
        const_iterator constIt = ConstIterator(*this);
        return constIt;
    }

    /**
     * This function returns a const iterator to end of the map
     * @return const iterator to end of the map
     */
    const_iterator end() const noexcept
    {
        const_iterator constIt;
        return constIt;
    }

    /**
     * This function returns a const iterator to beginning of the map
     * @return const iterator to beginning of the map
     */
    const_iterator cbegin() const noexcept
    {
        const_iterator constIt = ConstIterator(*this);
        return constIt;
    }

    /**
     * This function returns a const iterator to end of the map
     * @return const iterator to end of the map
     */
    const_iterator cend() const noexcept
    {
        const_iterator constIt;
        return constIt;
    }

};

#endif //EX6_HASHMAP_HPP