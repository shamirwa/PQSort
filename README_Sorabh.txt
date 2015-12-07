//////////////////////////////////////
//                              //////
//  Name - Sorabh Hamirwasia    //////
//                              //////
//////////////////////////////////////



******************************
******************************
Input size = 10000000      ***
			               ***
******************************
******************************

Number of thread = 1

Time in Milliseconds:

mc02 94 $ run 1
Parallel Run Time in millisecond to sort is: 4285
============= SORTED ===========
mc02 122 $ run 1
Parallel Run Time in millisecond to sort is: 4282
============= SORTED ===========
mc02 123 $ run 1
Parallel Run Time in millisecond to sort is: 4284
============= SORTED ===========
mc02 124 $ run 1
Parallel Run Time in millisecond to sort is: 4287
============= SORTED ===========

==============================================================================

Number of thread = 4
Time in Milliseconds:

mc02 97 $ run 4
Parallel Run Time in millisecond to sort is: 1211
============= SORTED ===========
mc02 98 $ run 4
Parallel Run Time in millisecond to sort is: 1212
============= SORTED ===========
mc02 99 $ run 4
Parallel Run Time in millisecond to sort is: 1207
============= SORTED ===========
mc02 100 $ run 4
Parallel Run Time in millisecond to sort is: 1223
============= SORTED ===========
mc02 101 $ run 4
Parallel Run Time in millisecond to sort is: 1217
============= SORTED ===========
mc02 104 $ run 4
Parallel Run Time in millisecond to sort is: 1211
============= SORTED ===========

Speed Up observed with 4 threads = 3.55

==============================================================================

Number of thread = 8
Time in Milliseconds:

mc02 111 $ run 8
Parallel Run Time in millisecond to sort is: 694
============= SORTED ===========
mc02 112 $ run 8
Parallel Run Time in millisecond to sort is: 765
============= SORTED ===========
mc02 113 $ run 8
Parallel Run Time in millisecond to sort is: 697
============= SORTED ===========
mc02 114 $ run 8
Parallel Run Time in millisecond to sort is: 695
============= SORTED ===========
mc02 115 $ run 8
Parallel Run Time in millisecond to sort is: 694
============= SORTED ===========

Speed Up observed with 4 threads = 6.17


==============================================================================
With dataset of 64000000
and 8 threads the minimum observed parallel time is 4636. 
I am getting maximum speedup of 6.3
==============================================================================

I have used the min heap library from:- https://github.com/armon/c-minheap-array
Since I am using the comparator function of this heap library so while
compiling its showing the warning related to it.

==================================================================================

