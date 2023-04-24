
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
#if defined( ACTOR )
{
    struct RecursiveMessage : public uActor::Message {
        T *data;
        T *copy;
        unsigned int begin;
        unsigned int end;
        unsigned int depth;
        RecursiveMessage( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth )
            : Message( uActor::Delete ), data{ data }, copy{ copy }, begin{ begin }, end{ end }, depth{ depth } {}
    };

    class Merge {
      private:
        _Actor NodeActor {
            uActor *parent;
            int called;
            T *data;
            T *copy;
            unsigned int begin;
            unsigned int end;
            unsigned int depth;

            uActor::Allocation notifyCompletion() {
                if ( parent != nullptr ) {
                    *parent | uActor::stopMsg;
                }
                return uActor::Delete;
            }

            uActor::Allocation receive( uActor::Message & msg ) {
                Case( RecursiveMessage, msg ) {
                    parent = msg.sender();
                    called = 0;
                    if ( msg_d->end - msg_d->begin <= 1 ) {
                        return notifyCompletion();
                    }
                    data = msg_d->data;
                    copy = msg_d->copy;
                    begin = msg_d->begin;
                    end = msg_d->end;
                    depth = msg_d->depth;
                    if ( depth > 0 ) {
                        recursiveInit( data, copy, begin, end, depth );
                    } else {
                        sequentialInit( data, copy, begin, end, depth ); // sequential execution instead of making more actor
                        return notifyCompletion();
                    }
                }
                else Case( uActor::StopMsg, msg ) {
                    called += 1;
                    if ( called == 2 ) {
                        merge( data, copy, begin, end );
                        return notifyCompletion();
                    }
                };
                return uActor::Nodelete;
            }
        };

      public:
        static void init( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth ) {
            *new NodeActor() | *new RecursiveMessage( data, copy, begin, end, depth );
        }

        static void recursiveInit( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth ) {
            unsigned int middle = ( begin + end ) / 2;
            *new NodeActor() | *new RecursiveMessage( data, copy, begin, middle, depth - 1 );
            *new NodeActor() | *new RecursiveMessage( data, copy, middle, end, depth - 1 );
        }

        static void sequentialInit( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth ) {
            unsigned int middle = ( begin + end ) / 2;
            if ( end > begin + 1 ) {
                sequentialInit( data, copy, begin, middle, depth - 1 );
                sequentialInit( data, copy, middle, end, depth - 1 );
                merge( data, copy, begin, end );
            } else {
                return; // no change needed
            }
        }

        static void merge( T data[], T copy[], unsigned int begin, unsigned int end ) {
            // cout << begin << " " << end << endl;
            unsigned int middle = ( begin + end ) / 2;
            for ( unsigned int i = begin; i < end; i++ ) {
                copy[i] = data[i];
            }
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
                }
                counter += 1;
            }
            for ( unsigned int i = c_1; i < middle; i++ ) {
                data[counter + ( i - c_1 )] = copy[i];
            }

            for ( unsigned int i = c_2; i < end; i++ ) {
                data[counter + ( i - c_2 )] = copy[i];
            }
        }
    };

    T *copy = new T[size];
    uActor::start();
    Merge::init( data, copy, 0, size, depth );
    uActor::stop();
    for ( unsigned int i = 0; i < size; i++ ) {
        cout << data[i] << " ";
    }
    cout << endl;
    delete[] copy;
}
#else
{
    class Merge {
      private:
#if defined( TASK )
        _Task T1 {
            T *data;
            T *copy;
            unsigned int begin;
            unsigned int end;
            unsigned int depth;

            void main() { recursiveMerge( data, copy, begin, end, depth ); }

          public:
            T1( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth )
                : data{ data }, copy{ copy }, begin{ begin }, end{ end }, depth{ depth } {}
        };
#endif

      public:
        static void recursiveMerge( T data[], T copy[], unsigned int begin, unsigned int end, unsigned int depth ) {
            // cout << begin << " " << end << endl;
            if ( end > begin + 1 ) {
                unsigned int middle = ( begin + end ) / 2;
                if ( depth > 0 ) {
#if defined( CBEGIN )
                    COBEGIN
                    BEGIN recursiveMerge( data, copy, begin, middle, depth - 1 );
                    END BEGIN recursiveMerge( data, copy, middle, end, depth - 1 );
                    END COEND
#elif defined( TASK )
                    {
                        T1 t = T1( data, copy, begin, middle, depth - 1 );
                        recursiveMerge( data, copy, middle, end, depth - 1 );
                    }
#endif
                } else {
                    recursiveMerge( data, copy, begin, middle, depth - 1 );
                    recursiveMerge( data, copy, middle, end, depth - 1 );
                }

                for ( unsigned int i = begin; i < end; i++ ) {
                    copy[i] = data[i];
                }
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
                    }
                    counter += 1;
                }
                for ( unsigned int i = c_1; i < middle; i++ ) {
                    data[counter + ( i - c_1 )] = copy[i];
                }

                for ( unsigned int i = c_2; i < end; i++ ) {
                    data[counter + ( i - c_2 )] = copy[i];
                }

            } else {
                return; // at leaf node, so 1 element, then just return
            }
        }
    };

    T *copy = new T[size];
    Merge::recursiveMerge( data, copy, 0, size, depth );
    for ( unsigned int i = 0; i < size; i++ ) {
        cout << data[i] << " ";
    }
    cout << endl;
    delete[] copy;
}

#endif

#endif