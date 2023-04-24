#ifndef Q3_PC
#define Q3_PC 1
#include "MPRNG.h"
#include "q3buffer.h"

extern MPRNG mprng;

_Task Consumer {
    void main();
  private:
    // we store the construtor parameters
    BoundedBuffer<int> *buffer;
    const int delay;
    const int sentinel;
    int *sum;
    // variable used for information passing
    int accumulate;

  public:
    Consumer( BoundedBuffer<int> & buffer, const int Delay, const int Sentinel, int &sum )
        : buffer{ &buffer }, delay{ Delay }, sentinel{ Sentinel }, sum{ &sum }, accumulate{ 0 } {};
    ~Consumer(); // for good measure, copy the value during the destruction of the consumer
};

_Task Producer {
    void main(); 

  private:
    // we store the construtor parameters
    BoundedBuffer<int> *buffer;
    const int produce;
    const int delay;

  public:
    Producer( BoundedBuffer<int> & buffer, const int Produce, const int Delay ) : buffer{ &buffer }, produce{ Produce }, delay{ Delay } {};
};

#endif