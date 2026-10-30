30/10/2026
Modified CMakeLists not to compile everything into one executable. Updated gitignore. Turned on compiler warnings. 
Changed paths to relative paths instead of exact paths. Honestly, these kinds of hygienic tidy-up is more satisfying
than what I am expecting. So now the plan is: 
1. Update finite difference pricer to enable pricing American option.
2. Disable analytical pricer and Monte Carlo pricer for American option. 
3. Update finite difference pricer to use explicit finite difference method. 
4. Write tests.
5. Add dividends `q`.
6. Add greeks: delta, gamma, vega, theta, rho blah blah blah.
7. Implied volatility solver ( wow, I can ditch the excel solver finally! )
8. Benchmarking, see how bad it is gonna be.
9. Compare result with Quantlib.
10. Write a readme, that's probably the most interesting part, write a document that no is gonna read, but just myself
to geek out a bit.

---

25/10/2026
Start to hate this project at this point, but I still implement the model of American option. The difference between it
and European option is that American option allows early exercise. So, the option base class has added a pure virutal
function that is `canExercise()`, that determines whether it can be exercised or not. For European option, it is only 
allowed when the current time is greater than or equal to the maturity. But for American option, it is `true` all the 
time. Binomial option pricer pricing function is also updated to check whether the option is exercisable. That's it for
today, tomorrow I will add some works to the other pricers (except `BlackScholesAnalyticPricer` as American option does
not have closed-form solution).

---

23/10/2026
Alright, so now, we have the last pricer to be implemented, that is the Monte Carlo (not the casino, shocker) simulation
pricer. Under the law of large number, by simulating large amount of price paths, we get the average, aka the expected
payoff to approximate the option price. The implementation is relatively straightforward compare to the finite difference
method in my humble opinion. We need a random number generator to be the source of the GBM process, and compute the 
final payoff at the terminal node, and finally discount back to the present value.

Since we will need to know the standard error when doing Monte Carlo simulation, I have improved the return type of the
price function into a struct that allow us to contain more result.

---

22/10/2026
Today, I am happy to announced that the work is not done yet (shocker)! Our goal is to build two more pricers: finite
difference method and Monte Carlo simulation. Why? It is because not every kind of option can be priced analytically,
and not every kind of option can be priced using binary tree. For example, Asian option cannot be priced analytically.

We decomposed the Black-Scholes PDE into a system of linear equations, so solve it using finite difference method. This
is a interesting and beautiful approach.

The maths of transforming the Black-Scholes PDE into heat equation worth some explanation. It could be mentioned on 
further notes in the future.

---

21/10/2026
Now the naive implementation is completely updated to adopt only a vector instead of building a binary tree. We first 
compute the option payoffs in the terminal node values, then we do the backward induction in another forloop. This is 
mush more efficient. It also trims off quite a bit of code inside the pricer class.

Great, after some intense work, 1 can of redbull, 2 pieces of gum, a bunch of indie music, now the binary tree works
for discrete and CRR model. The option price is computed correctly, the code is much cleaner and flexible. Also, finally
the hpp and cpp files are separated.

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