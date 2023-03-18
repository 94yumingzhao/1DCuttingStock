# 1D Cutting Stock Branch and Price
Solving one dimensional cutting stock problem with branch and price in C++  

 About cutting stock problem:  
 https://en.wikipedia.org/wiki/Cutting_stock_problem  
 
 About column generation:  
 https://en.wikipedia.org/wiki/Column_generation  
 
 About branch and bound:  
 https://en.wikipedia.org/wiki/Branch_and_bound  
 
 About branch and price:  
 https://en.wikipedia.org/wiki/Branch_and_price  

Root node cutting stock model:  

Minimize  
 obj1: X_1 + X_2 + X_3 + X_4 + X_5 + X_6 + X_7 + X_8 + X_9 + X_10 + X_11 + X_12 + X_13 + X_14 + X_15 + X_16 + X_17  
 
Subject To  
 c1:  3 X_1 >= 2  
 c2:  2 X_2 >= 1  
 c3:  2 X_3 >= 1  
 c4:  2 X_4 >= 2  
 c5:  X_5 >= 1  
 c6:  X_6 >= 1  
 c7:  3 X_7 >= 2  
 c8:  4 X_8 >= 1  
 c9:  3 X_9 >= 1  
 c10: 4 X_10 >= 1  
 c11: X_11 >= 1  
 c12: 5 X_12 >= 1  
 c13: 3 X_13 >= 1  
 c14: X_14 >= 1  
 c15: X_15 >= 1  
 c16: X_16 >= 1  
 c17: 3 X_17 >= 1  
 

