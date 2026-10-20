20/10/2026

I am designing a suitable base class for option. The ideal design is:
    Price = Engine( Instrument, MarketData )

The Instrument could be European call, Asian put, digital option etc. The key is that the payoff function should
be detached from the option contract itself. 

For the MarketData, it should contain the spot price, volatility, riskfree rate, and even the dynamics of the 
spot price movement e.g. GBM.

However, there is an old saying: premature optimization is the root of all evil. I will implement the vanilla 
European call and put option first, then I will refactor and generalize the design.

The implementation now is naive and flawed, I can already anticipate it will cause problems when we extend it 
to American option. Our goal is to address this problem. I will implement 4 pricing methods: 
1. analytical (Black-Scholes),
2. binary tree,
3. finite difference method,
4. Monte Carlo simulation.
We can then evaluate it and generalize the design of the option class.