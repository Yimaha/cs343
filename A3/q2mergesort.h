
#ifndef Q2_MERGE_SORT
#define Q2_MERGE_SORT 1

#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <uActor.h>
#include <uCobegin.h>
using namespace std;
template <typename T>
void mergesort( T data[], unsigned int size, unsigned int depth )
/*
Note:
It is possible to slam every possible definition together and reduce the amount of duplicate code
based on ACTOR, CBEGIN, TASK

However, I personally found that approach is quiet bad in this case, as the class
becomes quiet complicated and difficult to debug. (and limit modification in future)

I decided to completely split the code into 3 different definition, making easy to mark
and modify as well
*/

#if defined( ACTOR )
{
    struct RecursiveMessage : public uActor::Message {
        T *data;            // orignial data
        T *copy;            // the copy
        unsigned int begin; // start index
        unsigned int end;   // end index
        unsigned int depth; // current depth
        RecursiveMessage( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth )
            : Message( uActor::Delete ), data{ data }, copy{ copy }, begin{ begin }, end{ end }, depth{ depth } {}
    }; // struct

    class Merge {
      private:
        _Actor NodeActor {
            uActor *parent; // need to know parent in order to notify
            int called;     // used to check how many times it is called, if bigger than 2 we should terminate
            // we store the data for merging later
            T *data;
            T *copy;
            unsigned int begin;
            unsigned int end;
            unsigned int depth;

            uActor::Allocation notifyCompletion() {
                // when the actor should be deleted, try to notify parent if needed
                if ( parent != nullptr ) {
                    *parent | uActor::stopMsg;
                } // if
                return uActor::Delete;
            }

            uActor::Allocation receive( uActor::Message & msg ) {
                Case( RecursiveMessage, msg ) {
                    // initilize the actor
                    parent = msg.sender();
                    called = 0;
                    if ( msg_d->end - msg_d->begin <= 1 ) { // if there is only 1 element we just terminate
                        return notifyCompletion();
                    } // if
                    // store data
                    data = msg_d->data;
                    copy = msg_d->copy;
                    begin = msg_d->begin;
                    end = msg_d->end;
                    depth = msg_d->depth;
                    if ( depth > 0 ) { // we can still create more thread
                        recursiveInit( data, copy, begin, end, depth );
                    } else {                                      // we no longer allowed to create more thread
                        sequentialInit( data, copy, begin, end ); // sequential execution instead of making more actor
                        return notifyCompletion();                // should notify parent once sequential finished
                    }                                             // if
                }
                else Case( uActor::StopMsg, msg ) {
                    called += 1;
                    if ( called == 2 ) { // we now ready to merge
                        merge( data, copy, begin, end );
                        return notifyCompletion();
                    }                    // if
                };                       // Case
                return uActor::Nodelete; // general case, we shouldn't delete
            }
        };

      public:
        static void init( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth ) {
            // the first init called to start the process
            *new NodeActor() | *new RecursiveMessage( data, copy, begin, end, depth );
        }

        static void recursiveInit( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth ) {
            // all subsequence call that make new thread go through here
            unsigned int middle = ( begin + end ) / 2;
            *new NodeActor() | *new RecursiveMessage( data, copy, begin, middle, depth - 1 );
            *new NodeActor() | *new RecursiveMessage( data, copy, middle, end, depth - 1 );
        }

        static void sequentialInit( T data[], T copy[], unsigned int begin, unsigned int end ) {
            // all subsequence call that does not make new thread go through here
            unsigned int middle = ( begin + end ) / 2;
            if ( end > begin + 1 ) {
                sequentialInit( data, copy, begin, middle );
                sequentialInit( data, copy, middle, end );
                merge( data, copy, begin, end );
            } else {
                return; // no change needed
            }           // if
        }

        static void merge( T data[], T copy[], unsigned int begin, unsigned int end ) {
            // standard merge sort algorithm
            unsigned int middle = ( begin + end ) / 2; // we find the middle index (round down)
            for ( unsigned int i = begin; i < end; i++ ) {
                copy[i] = data[i]; // copy the whole array
            }                      // for

            unsigned int counter = begin;
            unsigned int c_1 = begin;
            unsigned int c_2 = middle;
            // keep adding until either the 1st 1/2 run out or 2nd 1/2 run out
            while ( c_1 < middle && c_2 < end ) {
                if ( copy[c_1] < copy[c_2] ) {
                    data[c_1 + ( c_2 - middle )] = copy[c_1];
                    c_1 += 1;
                } else {
                    data[c_1 + ( c_2 - middle )] = copy[c_2];
                    c_2 += 1;
                } // if
                counter += 1;
            } // while
            // since the other half ran out, just add in the rest.
            for ( unsigned int i = c_1; i < middle; i++ ) {
                data[counter + ( i - c_1 )] = copy[i];
            } // for

            for ( unsigned int i = c_2; i < end; i++ ) {
                data[counter + ( i - c_2 )] = copy[i];
            } // for
        }
    }; // merge

    T *copy = new T[size];
    uActor::start();
    Merge::init( data, copy, 0, size, depth );
    uActor::stop();
    delete[] copy; // copy does not carry back, thus deleted
}
#elif defined( TASK )
{
    class Merge {
      private:
        _Task T1 {
            // Task also stores the input variable from constructor, later used in main
            T *data;
            T *copy;
            unsigned int begin;
            unsigned int end;
            unsigned int depth;
            // in main we simply call recursive merge
            void main() { recursiveMerge( data, copy, begin, end, depth ); }

          public:
            T1( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth )
                : data{ data }, copy{ copy }, begin{ begin }, end{ end }, depth{ depth } {}
        };

      public:
        static void recursiveMerge( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth ) {
            if ( end > begin + 1 ) {
                unsigned int middle = ( begin + end ) / 2;
                if ( depth > 0 ) { // if we have more depth we create more thread objects
                    {
                        // note that we do the left in the new thread, right in the existing thread
                        T1 t = T1( data, copy, begin, middle, depth - 1 );
                        recursiveMerge( data, copy, middle, end, depth - 1 );
                    }
                } else { // if we don't have more depth we execute rest sequential
                    recursiveMerge( data, copy, begin, middle, depth );
                    recursiveMerge( data, copy, middle, end, depth );
                }                                // if
                merge( data, copy, begin, end ); // ready to merge
            } else {
                return; // at leaf node, so 1 element, then just return
            }           // if
        }
        // identical to the merge in ACTOR, please reference that
        static void merge( T data[], T copy[], unsigned int begin, unsigned int end ) {
            unsigned int middle = ( begin + end ) / 2;
            for ( unsigned int i = begin; i < end; i++ ) {
                copy[i] = data[i];
            } // for
            unsigned int counter = begin;
            unsigned int c_1 = begin;
            unsigned int c_2 = middle;
            while ( c_1 < middle && c_2 < end ) {
                if ( copy[c_1] < copy[c_2] ) {
                    data[c_1 + ( c_2 - middle )] = copy[c_1];
                    c_1 += 1;
                } else {
                    data[c_1 + ( c_2 - middle )] = copy[c_2];
                    c_2 += 1;
                } // if
                counter += 1;
            } // while
            for ( unsigned int i = c_1; i < middle; i++ ) {
                data[counter + ( i - c_1 )] = copy[i];
            } // for

            for ( unsigned int i = c_2; i < end; i++ ) {
                data[counter + ( i - c_2 )] = copy[i];
            } // for
        }
    }; // Merge
    T *copy = new T[size];
    Merge::recursiveMerge( data, copy, 0, size, depth );
    delete[] copy;
}
#elif defined( CBEGIN )
{
    class Merge {
      public:
        // unlike the previous 2 approaches, CBEGIN is quiet simpler to implement
        static void recursiveMerge( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth ) {
            if ( end > begin + 1 ) {
                unsigned int middle = ( begin + end ) / 2;
                if ( depth > 0 ) { // more depth are possible, we keep making more thread
                    COBEGIN
                    BEGIN recursiveMerge( data, copy, begin, middle, depth - 1 ); END 
                    BEGIN recursiveMerge( data, copy, middle, end, depth - 1 ); END
                    COEND
                } else { // no more depth are possible, we execute rest sequential
                    recursiveMerge( data, copy, begin, middle, depth );
                    recursiveMerge( data, copy, middle, end, depth );
                } // if

                merge( data, copy, begin, end ); // ready to merg

            } else {
                return; // at leaf node, so 1 element, then just return
            }           // if
        }
        // identical to the merge in ACTOR, please reference that
        static void merge( T data[], T copy[], unsigned int begin, unsigned int end ) {
            unsigned int middle = ( begin + end ) / 2;
            for ( unsigned int i = begin; i < end; i++ ) {
                copy[i] = data[i];
            } // for
            unsigned int counter = begin;
            unsigned int c_1 = begin;
            unsigned int c_2 = middle;
            while ( c_1 < middle && c_2 < end ) {
                if ( copy[c_1] < copy[c_2] ) {
                    data[c_1 + ( c_2 - middle )] = copy[c_1];
                    c_1 += 1;
                } else {
                    data[c_1 + ( c_2 - middle )] = copy[c_2];
                    c_2 += 1;
                } // if
                counter += 1;
            } // while
            for ( unsigned int i = c_1; i < middle; i++ ) {
                data[counter + ( i - c_1 )] = copy[i];
            } // for

            for ( unsigned int i = c_2; i < end; i++ ) {
                data[counter + ( i - c_2 )] = copy[i];
            } // for
        }
    }; // Merge
    T *copy = new T[size];
    Merge::recursiveMerge( data, copy, 0, size, depth );
    delete[] copy;
}

#endif // ACTOR

#endif // Q2_MERGE_SORT