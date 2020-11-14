//
//  ADS_Set.h
//  Coalesced Hashing
//
//  Created by Rosi-Eliz Dzhurkova on 08.11.20.
//

#ifndef ADS_set_h
#define ADS_set_h

#define RANDOM_A 1375099069
#define RANDOM_B 982669252
#define RANDOM_LARGE_PRIME 2411993867
#define MIN_CAPACITY 127
#define CELLAR_COEFFICIENT 0.1628
#define RESERVE_THRESHOLD 0.75
#include <string>
#include <vector>

template <typename Key, size_t N >
class ADS_set;

template <typename Key, size_t N>
bool operator==(const ADS_set<Key, N>& first, const ADS_set<Key, N>& second);

template <typename Key, size_t N>
bool operator!=(const ADS_set<Key, N>& first, const ADS_set<Key, N>& second);

template <typename Key, size_t N = MIN_CAPACITY>
class ADS_set
{
    // Instance variables
public:
    class Iterator;
    class Bucket;
    using container_type = Bucket;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator =  Iterator; /* constant ForwardIterator */
    using iterator = const_iterator;
    using key_equal = std :: equal_to <key_type>; // hashing
    using hasher = std :: hash <key_type>; // hashing
    
    struct Bucket{
        bool isOccupied = false;
        value_type keyTypeValue;
        Bucket* next = nullptr;
        Bucket() {};
        Bucket(value_type associateValue, Bucket* next = nullptr) : keyTypeValue(keyTypeValue), next(next), isOccupied(true) {}
        bool operator==(const Bucket& other) { return keyTypeValue == other.keyTypeValue && next == other.next; }
        Bucket& operator=(const Bucket& other)
        {
            isOccupied = other.isOccupied;
            keyTypeValue = other.keyTypeValue;
            next = other.next;
            return *this;
        }
        friend std::ostream& operator<<(std::ostream& o, const Bucket& bucket)
        {
            o<<"Key: "<<bucket.keyTypeValue<<"Is occupied: "<<bucket.isOccupied<<"Next element: "<<bucket.next;
            return o;
        }
    };
private:
    Bucket* buckets = nullptr;
    size_t capacity = N;
    
    
    // Instance methods
public:
    ADS_set();
    ADS_set(std::initializer_list<key_type> ilist);
    template<typename InputIt>
    ADS_set(InputIt first, InputIt last);
    ADS_set(const ADS_set &other);
    ~ADS_set();
    
    ADS_set &operator=(const ADS_set &other);
    ADS_set &operator=(std::initializer_list<key_type> ilist);
    
    size_type size() const;
    bool empty() const;
    
    size_type count(const key_type &key) const;
    iterator find(const key_type &key) const;
    
    void clear();
    void swap(ADS_set &other);
    
    void insert(std::initializer_list<key_type> ilist);
    std::pair<iterator,bool> insert(const key_type &key);
    template<typename InputIt>
    void insert(InputIt first, InputIt last);
    
    size_type erase(const key_type &key);
    
    const_iterator begin() const;
    const_iterator end() const;
    
    void dump(std::ostream &o = std::cerr) const;
    friend bool operator==<Key, N>(const ADS_set& first, const ADS_set& second);
    friend bool operator!=<Key, N>(const ADS_set& first, const ADS_set& second);
private:
    void insertIntoPlace(size_t index, key_type key);
    double load_factor() const;
    void reserveAndRehashIfNecessary();
    void insertAndRehash(key_type key);
    size_type index(key_type key) const;
    std::vector<container_type*> chainedBuckets(container_type* origin);
    std::pair<size_type, size_type> cellarDescriptor() const;
    bool bucketIsInCellar(const container_type* bucket) const;
    container_type* assignCollisionElement(container_type* parentBucket, key_type key);
};

template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {

public:
    using container_type_pointer = const container_type*;
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;
    
    explicit Iterator(container_type_pointer ptr, const ADS_set* set) : ptr(ptr), set(set) {} ;
    Iterator() : ptr(nullptr), set(nullptr) {};
    reference operator*() const {return ptr->keyTypeValue;};
    pointer operator->() const {return &(ptr->keyTypeValue);};
    Iterator &operator++()
    {
        if(set->size() == 0) {
            return *this;
        }
        container_type_pointer currentBucket = this->ptr;
        do {
            currentBucket++;
        }
        while (set->end().ptr - currentBucket > 0 && !currentBucket->isOccupied);
        
        auto p = set->end().ptr - currentBucket > 0 ? currentBucket : set->end().ptr;
        ptr = p;
        return *this;
    }
    
    Iterator operator++(int)
    {
        Iterator oldIterator = Iterator(ptr, set);
        ++*this;
        return oldIterator;
    };
    friend difference_type operator-(const Iterator& first, const Iterator& second) {return first.ptr - second.ptr;};
    friend bool operator==(const Iterator &lhs, const Iterator &rhs) {return lhs.ptr == rhs.ptr;};
    friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {return lhs.ptr != rhs.ptr;};
private:
    container_type_pointer ptr;
    const ADS_set* set;
};

template <typename Key, size_t N>
void swap(ADS_set<Key,N>& first, ADS_set<Key,N>& second)
{
    first.swap(second);
}

template <typename Key, size_t N>
bool operator==(const ADS_set<Key, N>& first, const ADS_set<Key, N>& second)
{
   if(first.size() != second.size() || first.capacity != second.capacity)
       return false;
    auto firstBegin = first.begin();
    auto secondBegin = second.begin();
    while(firstBegin != first.end() && secondBegin != second.end())
    {
        if(firstBegin != secondBegin)
            return false;
        firstBegin++;
        secondBegin++;
    }
    return true;
}

template <typename Key, size_t N>
bool  operator!=(const ADS_set<Key, N>& first, const ADS_set<Key, N>& second)
{
    return !(first == second);
}

template <typename Key, size_t N>
ADS_set<Key, N>::ADS_set() : buckets(new Bucket[N]{}) {}

template <typename Key, size_t N>
ADS_set<Key, N>::ADS_set(std::initializer_list<key_type> list) : buckets(new Bucket[N]{})
{
    key_type s;
    container_type * c;
    assignCollisionElement(c,  s);
    insert(list);
}

template <typename Key, size_t N>
ADS_set<Key, N>::ADS_set(const ADS_set &other) : buckets(other.capacity ? new Bucket[other.capacity] : nullptr), capacity(other.capacity)
{
    memmove(buckets, other.buckets, capacity * sizeof(container_type));
}

template <typename Key, size_t N>
template<typename InputIt>
ADS_set<Key, N>::ADS_set(InputIt first, InputIt last) : ADS_set()
{
    insert(first, last);
}

template <typename Key, size_t N>
ADS_set<Key, N>::~ADS_set()
{
    delete[] buckets;
}

template <typename Key, size_t N>
ADS_set<Key, N>& ADS_set<Key, N>::operator=(const ADS_set &other) // changed from using swap function because parameter must remain const qualified
{
    if(this == &other)
        return *this;
    
    delete[] buckets;
    
    buckets = new Bucket[other.capacity];
    memmove(buckets, other.buckets, capacity * sizeof(size_t));
    return *this;
}

template <typename Key, size_t N>
ADS_set<Key, N>& ADS_set<Key, N>::operator=(std::initializer_list<key_type> ilist)
{
    ADS_set<Key, N> helpInstance(ilist);
    
    swap(helpInstance); //tuk polzvashe friend funkciqta i ne q namirashe
    return *this;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::find(const key_type& key) const
{
    size_type keyIndex = index(key);
    if(buckets[keyIndex].isOccupied)
    {
        if(buckets[keyIndex].next == nullptr)
        {
            if(key_equal()(buckets[keyIndex].keyTypeValue, key))
                return iterator(&buckets[keyIndex], this);
            else
                return end();
        }
        else
        {
            Bucket* currentBucket = &buckets[keyIndex];
            while(currentBucket != nullptr && !key_equal()(currentBucket->keyTypeValue, key))
            {
                currentBucket = currentBucket->next;
            }
            if(currentBucket == nullptr )
                return end();
            
            return iterator(currentBucket, this);
        }
    }
    else
        return end();
}


template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::count(const key_type &key) const
{
    if(find(key) != end())
        return 1;
    return 0;
}

template <typename Key, size_t N>
void ADS_set<Key, N>::clear()
{
    if(!empty())
    {
        for(size_type i{0}; i < capacity; i++)
        {
            buckets[i].isOccupied = false;
            buckets[i].next = nullptr;
        }
    }
}

/*
 1. Implement a method, which specifies if a bucket is in a cellar or in a non-cellar
 2. Implement a method, which receives a bucket and a key and returns an already linked bucket for the new element
 */

template <typename Key, size_t N>
bool ADS_set<Key, N>::bucketIsInCellar(const container_type* bucket) const
{
    container_type* cellarBegin = &buckets[cellarDescriptor().first];
    return bucket - cellarBegin >= 0;
}

//Implementing varied insertion
template <typename Key, size_t N>
typename ADS_set<Key, N>::container_type* ADS_set<Key, N>::assignCollisionElement(container_type* parentBucket, key_type key)
{
    auto chainedElements = chainedBuckets(parentBucket);
    container_type* bucketForInsertion = chainedElements.back();
    container_type* currentChainEnd = chainedElements.back();
    container_type* setLastElement = &buckets[static_cast<int>(size()) - 1];
    if(bucketIsInCellar(bucketForInsertion))
    {
        do{
            bucketForInsertion++;
        }
        while(bucketForInsertion != setLastElement && bucketForInsertion->isOccupied);
        
        if(bucketForInsertion == setLastElement && bucketForInsertion->isOccupied)
        {
            bucketForInsertion = &buckets[cellarDescriptor().first];
            do
            {
                bucketForInsertion--;
            }
            while(bucketForInsertion != &buckets[0] && bucketForInsertion->isOccupied);
            
            if(bucketForInsertion == &buckets[0] && bucketForInsertion->isOccupied)
                // Only applicable to static hash sets. Since the current data structure is
                // a dynamic one this should be impossible to happen
                throw std::runtime_error("Hashtable is full");
            
        }
    }
    else
    {
        do {
            bucketForInsertion++;
        } while (bucketForInsertion->isOccupied &&  bucketForInsertion != &buckets[cellarDescriptor().first]);
        
        if(bucketForInsertion == &buckets[cellarDescriptor().first])
        {
            bucketForInsertion = chainedElements.back();
            while(bucketForInsertion != &buckets[0] && bucketForInsertion->isOccupied)
            {
                bucketForInsertion--;
            }
            
            if(bucketForInsertion == &buckets[0] && bucketForInsertion->isOccupied)
            // Only applicable to static hash sets. Since the current data structure is
            // a dynamic one this should be impossible to happen
            throw std::runtime_error("Hashtable is full");
            
        }
    }
    currentChainEnd->next = bucketForInsertion;
    bucketForInsertion->keyTypeValue = key;
    bucketForInsertion->isOccupied = true;
    bucketForInsertion->next = nullptr;
    return bucketForInsertion;
}

template <typename Key, size_t N>
void ADS_set<Key, N>::insertIntoPlace(size_t index, key_type key)
{
    if(count(key) > 0)
        return;
    
    Bucket* currentBucket = &buckets[index];
    if (!currentBucket->isOccupied)
    {
        currentBucket->keyTypeValue = key;
        currentBucket->isOccupied = true;
        return;
    }
    //plugs in new cellar functionality using varied-insertion technique
    
    assignCollisionElement(currentBucket, key);
    
    while(currentBucket->isOccupied)
    {
        if(currentBucket->next == nullptr)
        {
            Bucket* newBucket = currentBucket;
            while (newBucket->isOccupied) {
                //* added ampersant
                if(newBucket == &buckets[capacity - 1])
                    throw std::runtime_error("Out of range");
                newBucket++;
            }
            //* changed to pair
            newBucket->keyTypeValue = key;
            newBucket->isOccupied = true;
            currentBucket->next = newBucket;
            return;
        }
        currentBucket = currentBucket->next;
    }
}

template <typename Key, size_t N>
void ADS_set<Key, N>::swap(ADS_set& other)
{
    std::swap(capacity, other.capacity);
    std::swap(buckets, other.buckets);
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::size() const
{
    int count{0};
    for(size_type i{0}; i < capacity; i++)
    {
        if(buckets[i].isOccupied)
            count++;
    }
    return count;
}

template <typename Key, size_t N>
bool ADS_set<Key, N>::empty() const
{
    return size() == 0;
}

template <typename Key, size_t N>
double ADS_set<Key, N>::load_factor() const
{
    return size() / capacity;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::index(key_type key) const {
    size_type hash = std::hash<key_type>()(key);
    return hash % cellarDescriptor().first;
}

template <typename Key, size_t N>
void ADS_set<Key, N>::reserveAndRehashIfNecessary() {
    if(load_factor() < RESERVE_THRESHOLD)
    {
        return;
    }
    Bucket* oldBuckets = buckets;
    size_type oldCapacity = capacity;
    capacity *= 2;
    buckets = new Bucket[capacity];
    for(size_type i{0}; i < oldCapacity; i++)
    {
        Bucket bucketToCopy = oldBuckets[i];
        if (!bucketToCopy.isOccupied) {
            continue;
        }
        insertIntoPlace(index(bucketToCopy.keyTypeValue), bucketToCopy.keyTypeValue);
    }
    delete [] oldBuckets;
}

template <typename Key, size_t N>
std::pair<typename ADS_set<Key, N>::iterator,bool> ADS_set<Key, N>::insert(const key_type &key)
{
    auto it = find(key);
    if(it != end())
        return std::pair<iterator,bool>(it, false);
    insertAndRehash(key);
    return std::pair<iterator,bool>(find(key), true);
}

template <typename Key, size_t N>
void ADS_set<Key, N>::insertAndRehash(key_type key)
{
    reserveAndRehashIfNecessary();
    size_type indexKey = index(key);
    insertIntoPlace(indexKey, key);
}

template <typename Key, size_t N>
void ADS_set<Key, N>::insert(std::initializer_list<key_type> ilist)
{
    for(key_type key : ilist)
    {
        insertAndRehash(key);
    }
}

template<typename Key, size_t N>
template<typename InputIt>
void ADS_set<Key, N>::insert(InputIt first, InputIt last)
{
    while(first != last)
    {
        insert(*first);
        first++;
    }
}

template <typename Key, size_t N>
std::vector<typename ADS_set<Key, N>::container_type*> ADS_set<Key, N>::chainedBuckets(container_type* origin)
{
    container_type* oldBucket = origin;
    std::vector<container_type*> result;
    while(oldBucket != nullptr)
    {
        result.push_back(oldBucket);
        oldBucket = oldBucket->next;
    }
    
    return result;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::erase(const key_type& key)
{
    if(find(key) == end())
    {
        return 0;
    }
    
    size_type keyIndex = index(key);
    Bucket* currentBucket = &buckets[keyIndex];

    Bucket* predecessor = nullptr;
    while(!key_equal()(currentBucket->keyTypeValue, key))
    {
        predecessor = currentBucket;
        currentBucket = currentBucket->next;
    }
    if(predecessor != nullptr && predecessor->isOccupied)
        predecessor->next = nullptr;
    
    Bucket* successor = currentBucket->next;
    std::vector<Bucket*> chain = chainedBuckets(successor);
    currentBucket->isOccupied = false;
    currentBucket->next = nullptr;
    
    for(Bucket* successor : chain)
    {
        size_type keyIndex = index(successor->keyTypeValue);
        if(buckets[keyIndex].isOccupied) // we hit one of our already processed elements
        {
            Bucket* parentBucket = &buckets[keyIndex];
            while(parentBucket->next != nullptr)
            {
                parentBucket = parentBucket->next;
            }
            parentBucket->next = successor; // our elements remains in place and becomes the end of the chained list of elements of the hit element
            successor->next = nullptr;
        }
        else //we hit a new 'hole'
        {
            buckets[keyIndex] = *successor;
            buckets[keyIndex].isOccupied = true;
            successor->isOccupied = false;
            successor->next = nullptr;
        }
    }
return 1;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::begin() const
{
    if(size() == 0) {
        return iterator(&buckets[0], this);
    }
    int i = 0;
    while (i < capacity && !buckets[i].isOccupied ){
        i++;
        auto currentIterator = iterator(&buckets[i], this);
        if (end() - currentIterator < 0)
            return end();
    }
    return iterator(&buckets[i], this);
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::end() const
{
    if (size() == 0)
    {
        return iterator(&buckets[0], this);
    }
    else
    {
        return iterator(&buckets[capacity], this);
    }
}

template <typename Key, size_t N>
std::pair<typename ADS_set<Key, N>::size_type, typename ADS_set<Key, N>::size_type> ADS_set<Key, N>::cellarDescriptor() const
{
    size_type cellarSize = capacity - ceil(capacity * CELLAR_COEFFICIENT);
    size_type cellarInitialIndex = capacity - cellarSize;
    return {cellarInitialIndex, cellarSize};
}


/*
 Output of the container content to the stream or similar . There is no specification for the functionality of dump () , ie what the method writes to the stream is not prescribed. It is also permissible for the method to return nothing. However, it is advisable to output at least all the elements it contains. When testing, it can also be helpful if the output also shows the state of the data structure in some form.
 
 In the event of errors, the unit test outputs the contents of the container using this method in order to facilitate troubleshooting. However, if the output is too extensive, it can be shortened during the unit test. It can be assumed that during the unit test the container is only instantiated with element data types ( key_type ) that support the output operator ( << ).
 */
template <typename Key, size_t N>
void ADS_set<Key, N>::dump(std::ostream &o) const
{
    o<<"Set size: "<<size()<<std::endl;
    
    bool firstIteration = true;
    for(auto element : *this)
    {
        if(!firstIteration)
            o<<" "<<element;
        else
        {
            o<<element;
            firstIteration = false;
        }
    }
}

#endif /* ADS_set_h */
