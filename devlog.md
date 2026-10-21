21/10/2026
Now the naive implementation is completely updated to adopt only a vector instead of building a binary tree. We first 
compute the option payoffs in the terminal node values, then we do the backward induction in another forloop. This is 
mush more efficient. It also trim off quite a bit of code inside the pricer class.



---
20/10/2026
So we have 4 pricers, and we want to compute the option price. First thought is obviously having a base class for the
pricers, with a virtual function of computing the price. (whats the alternative lol?...). Since each pricing function 
requires different kind of parameters, and the price function is inherited, we can't really inject the parameters through
function argument. Instead, we can inject them through the class constructors.

So, I have built the naive implementation for the binary tree pricer. It discretely models the underlying spot price
movement in different nodes, then we compute the option payoff from the terminal nodes by backward induction. We compute 
the option price by discounting the expected payoff at each node which is calculated using the risk-neutral probability.

There is serval improvements done:
1. spot price is isolated from option class, and is injected through the constructor of the pricer class.

And no, that's it... Apparently, we can do lot better:
1. avoid using recursion in the backward induction (and )
2. use smart pointer instead of raw pointer (this is a big no no tbh)
3. just for god's sake, lets put the implementation into .cpp file instead of living in .hpp!!!

ok just find out there's more evil things happening:
1. the tree is not recombining, which means we are growing our tree exponentially, and producing node with same spot price but different sequence of up and down movement.
2. we are newing the tree node in buildBinaryTree function, but never deleting them, which means we are leaking memory like crazy.
---

19/10/2026

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