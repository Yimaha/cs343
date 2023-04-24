#pragma once

// priority non-blocking signal, urgent queue/stack, flush using N conditions
// for FIFO task service => no starvation or staleness  (Table 9.5, p. 495)
#define AUTOMATIC_SIGNAL \
	struct __AUTOMATIC_SIGNAL_T__ { \
		struct __AUTOMATIC_SIGNAL_NODE_T__ : public uSeqable { \
			uCondition cond; \
		}; \
		uSequence<__AUTOMATIC_SIGNAL_NODE_T__> waiting; \
		__AUTOMATIC_SIGNAL_NODE_T__ *cursor; \
	} __implicit_signal__
#define WAITUNTIL( pred, before, after ) \
	if ( ! __implicit_signal__.waiting.empty() || ! ( pred ) ) { \
		__implicit_signal__.cursor = __implicit_signal__.waiting.head(); \
		__AUTOMATIC_SIGNAL_T__::__AUTOMATIC_SIGNAL_NODE_T__ __implicit_signal_node__; \
		__implicit_signal__.waiting.addTail( &__implicit_signal_node__ ); \
		for ( ;; ) { \
			if ( __implicit_signal__.cursor != 0 ) { \
				__implicit_signal__.cursor->cond.signal(); \
			} \
			before; \
			__implicit_signal_node__.cond.wait(); \
			after; \
		  if ( ( pred ) ) break; \
			__implicit_signal__.cursor = __implicit_signal__.waiting.succ( &__implicit_signal_node__ ); \
		} \
		__implicit_signal__.waiting.remove( &__implicit_signal_node__ ); \
	}
#define EXIT() \
	{ \
		if ( ! __implicit_signal__.waiting.empty() ) { \
			__implicit_signal__.cursor = __implicit_signal__.waiting.head(); \
			__implicit_signal__.cursor->cond.signal(); \
		} \
	}
