// Preprocessor variables declared in a program can have no value (empty), but variables declared with compiler flag -D
// have a value of one, unless explicitly set. So the following trick concatenates a 0 at the end of the variable, so if
// the variable has no value it becomes 0. Otherwise, the value is increased by 10. For example, if the variable is 1,
// it becomes 10 => do the abort; any other value means do not do the abort.

#define DO_EXPAND( VAL ) VAL ## 0
#define EXPAND( VAL ) DO_EXPAND( VAL )

#ifdef BARGINGCHECK
	#define BCHECK( stmt ) stmt
	#if EXPAND(BARGINGCHECK) != 10
		#define ABORT cerr.flush(); abort();
	#else
		#define ABORT
	#endif
#else
	#define BCHECK( stmt )
#endif // BARGINGCHECK

#define __BCHECK_DECL__ \
unsigned int __groupTick__ = 0, __callCnt__ = 0; \
unsigned int __groupno__ = 0; \
\
unsigned int __prodEnter__() { \
	unsigned int temp = __groupTick__; \
	__callCnt__ += 1; \
	if ( __callCnt__ % group == 0 ) __groupTick__ += 1; \
	return temp; \
} \
void __votedone__( unsigned int ticket,  unsigned int groupno ) {		\
	if ( ticket != groupno ) osacquire( cerr ) << "BARGING ERROR!! voter over voter" << endl; ABORT \
} \

#define BCHECK_DECL \
	BCHECK( __BCHECK_DECL__ )
#define VOTER_ENTER \
	BCHECK( unsigned int __voter_ticket__ = __prodEnter__(); )
#define VOTE_DONE \
	BCHECK( __votedone__( __voter_ticket__, __groupno__ ); )
#define GROUP_DONE \
	BCHECK( __groupno__ += 1; )
