class Potato {
	// YOU ADD MEMBERS HERE
  public:
	_Event Explode {};
	Potato( PRNG & prng, unsigned int maxTicks = 10 );
	void reset( unsigned int maxTicks = 10 );
	void countdown();
};

_Coroutine Player {
	_Event Terminate {
	  public:
		Player & victim;					// delete player
		Terminate( Player & victim ) : victim( victim ) {}
	};
	_Event Election {
	  public:
		Player * player;					// highest player id seen so far
		Election( Player * player ) : player( player ) {}
	};
	Player * partner[2];					// left and right player
	// YOU ADD MEMBERS HERE
	void main();
	void vote();							// resume partner to vote
	void terminate();						// resume umpire
  public:
	static Player * umpire;					// current umpire

	Player( PRNG & prng, int id, Potato & potato );
	void start( Player & lp, Player & rp );	// supply partners
	int getId();							// player id
	void toss();							// tossed potato
};
