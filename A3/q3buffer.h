
#ifndef Q3_BUFFER
#define Q3_BUFFER 1
#include "BargingCheck.h"
#include <iostream>
#include <queue>
#include <string>
using namespace std;
#ifdef BUSY // busy waiting implementation

template <typename T> class BoundedBuffer {
  private:
    // 3 locks for mutual exclusion and synchronization
    uOwnerLock m;
    uCondLock producerLock;
    uCondLock consumerLock;
    // count indicate how full is the stack
    unsigned int count;
    // max size
    unsigned int bufferSize;
    // block counter
    int block;
    // the actual queue storing data
    queue<T> buffer;

  public:
    BoundedBuffer( const unsigned int size = 10 );
    unsigned long int blocks();
    void insert( T elem );
    T remove();
};

template <typename T> BoundedBuffer<T>::BoundedBuffer( const unsigned int size ) : count{ 0 }, bufferSize{ size }, block{ 0 }, buffer{ queue<T>() } {};

template <typename T> unsigned long int BoundedBuffer<T>::blocks() { return block; }

template <typename T> void BoundedBuffer<T>::insert( T elem ) {
    m.acquire(); // aquire mutual exclusion
    while ( count == bufferSize ) { // buffer is full, cannot insert
        block += 1;
        producerLock.wait( m ); // go to sleep, release mutual exclusion, reaquire once awake
    } // while
    assert(buffer.size() < bufferSize)
    buffer.push( elem ); // push it on the stack 
    count += 1; // increase the counter
    if ( !consumerLock.empty() ) { // ideally we wake some consumer to eat the produced.
        consumerLock.signal(); 
    } else if ( count < bufferSize && !producerLock.empty() ) { // if not, we should awake a producer to continue produce
        producerLock.signal();
    } // if
    m.release(); // release mutual exclusion
}

template <typename T> T BoundedBuffer<T>::remove() {
    m.acquire(); // aquire mutual exclusion
    while ( count == 0 ) { // buffer is empty, cannot get anything
        block += 1;
        consumerLock.wait( m ); // go to sleep, release mutual exclusion, reaquire once awake
    } // while
    T elem = buffer.front(); // read from buffer
    assert(buffer.size() > 0)
    buffer.pop(); // pop it
    count -= 1; // decrease the counter
    if ( !producerLock.empty() ) {  // ideally we wake some producer to make more
        producerLock.signal();
    } else if ( count > 0 && !consumerLock.empty() ) { // if not and it is not empty, we should consider waking consumer
        consumerLock.signal();
    } // if
    m.release(); // release mutual exclusion
    return elem; // return the element
} 
#endif // BUSY

#ifdef NOBUSY // no busy waiting implementation
template <typename T> class BoundedBuffer {
    BCHECK_DECL;
  private:
    uOwnerLock m;
    uCondLock producerLock;
    uCondLock consumerLock;
    unsigned int count;
    unsigned int bufferSize;
    int block;
    // everything is identical except this flag variable
    bool flag;
    queue<T> buffer;

  public:
    BoundedBuffer( const unsigned int size = 10 );
    unsigned long int blocks();
    void insert( T elem );
    T remove();
};

template <typename T>
BoundedBuffer<T>::BoundedBuffer( const unsigned int size ) : count{ 0 }, bufferSize{ size }, block{ 0 }, flag{ 1 }, buffer{ queue<T>() } {};

template <typename T> unsigned long int BoundedBuffer<T>::blocks() { return block; }

template <typename T> void BoundedBuffer<T>::insert( T elem ) {
    m.acquire();  // aquire mutual exclusion
    PROD_ENTER;
    if ( !flag || count == bufferSize ) { // buffer is full, cannot insert
        block += 1;
        producerLock.wait( m ); // go to sleep, release mutual exclusion, reaquire once awake
    } // if
    assert(buffer.size() < bufferSize)
    buffer.push( elem ); // push the element
    count += 1;
    INSERT_DONE;
    if ( !consumerLock.empty() ) { // if not empty we should consider waking a consumer
        CONS_SIGNAL( consumerLock );
        consumerLock.signal();
        flag = false; // note we turn off the flag for barging avoidance
    } else if ( count < bufferSize && !producerLock.empty() ) { // alternatively wake a producer if stack is not full
        PROD_SIGNAL( producerLock );
        producerLock.signal();
        flag = false; // note we turn off the flag for barging avoidance
    } else {
        flag = true; // if nothing, then just set flag to true and let the next available task go through 
    } // if
    m.release(); // release mutual exclusion
}

template <typename T> T BoundedBuffer<T>::remove() {
    m.acquire(); // aquire mutual exclusion
    CONS_ENTER;
    if ( !flag || count == 0 ) { // buffer is empty, cannot read
        block += 1;
        consumerLock.wait( m ); // go to sleep, release mutual exclusion, reaquire once awake
    } // if
    T elem = buffer.front(); // rea the element
    assert(buffer.size() > 0)
    buffer.pop(); // pop it
    REMOVE_DONE;
    count -= 1; // update the count
    if ( !producerLock.empty() ) { // if not empty we should consider waking a producer
        PROD_SIGNAL( producerLock );
        producerLock.signal();
        flag = false; // note we turn off the flag for barging avoidance
    } else if ( count > 0 && !consumerLock.empty() ) { // alternatively wake a consumer if stack is not empty
        CONS_SIGNAL( consumerLock );
        consumerLock.signal();
        flag = false; // note we turn off the flag for barging avoidance
    } else {
        flag = true; // if no one is available to be awake, just continue
    } // if
    m.release(); // release mutual exclusion
    return elem;
}
#endif // NOBUSY
#endif // Q3_BUFFER