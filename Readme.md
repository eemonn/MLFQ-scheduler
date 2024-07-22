# This is a Multi level feed back queue scheduler for operating system. It is based on the following rules
`Rule 1:` If Priority(A) > Priority(B), A runs (B doesn’t).

`Rule 2:` If Priority(A) = Priority(B), A & B run in round-robin fashion using the time slice (quantum length) of the given queue.

`Rule 3:` When a job enters the system, it is placed at the highest priority (the topmost queue).

`Rule 4:` Once a job uses up its time allotment at a given level (regardless of how many times it has given up the CPU), its priority is  reduced (i.e., it moves down one queue).

`Rule 5:` After some time period S, move all the jobs in the system to the topmost queue.

# How to compile
1. run `make`
2. it will create executables `scheduler`
3. run program using `./scheduler #cpu S filename` (e.g.  ./scheduler 2 3200 tasks.txt)
4. run `make clean` to remove executables

# Report 

| #CPU | 1                     | 2                     | 8                     |
|      | average turn time     | average turn time     | average turn time     |
|      | type0:  11912 usec    | type0: 11662 usec     | type0:5731 usec       |
|      | type1:  36824 usec    | type1: 33990 usec     | type1:17087 usec      |
|      | type2:  45870 usec    | type2: 43766 usec     | type2:22803 usec      |
|      | type3:  38723 usec    | type3: 42652 usec     | type3: 22165 usec     |
| 200  |                       |                       |                       |
|      | average response time | average response time | average response time |
|      | type0:  2605 usec     | type0:2683 usec       | type0:2626 usec       |
|      | type1:  3015 usec     | type1: 3125 usec      | type1: 3077 usec      |
|      | type2:  2665 usec     | type2:2756 usec       | type2: 2722 usec      |
|      | type3:  2335 usec     | type3: 2420 usec      | type3:  2338 usec     |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
| 800  | average turn time     | average turn time     | average turn time     |
|      | type0:12531 usec      | type0:11675 usec      | type0:5858 usec       |
|      | type1:38696 usec      | type1: 41150 usec     | type1: 17239 usec     |
|      | type2:47136 usec      | type2: 45571 usec     | type2: 21853 usec     |
|      | type3: 36206 usec     | type3:41100 usec      | type3: 21175 usec     |
|      |                       |                       |                       |
|      | average response time | average response time | average response time |
|      | type0:2393 usec       | type0: 2560 usec      | type0: 2239 usec      |
|      | type1:2814 usec       | type1:  2978 usec     | type1:  2661 usec     |
|      | type2:2461 usec       | type2: 2635 usec      | type2:  2305 usec     |
|      | type3: 2159 usec      | type3: 2307 usec      | type3:  1994 usec     |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
| 1600 | average turn time     | average response time | average response time |
|      | type0:12587 usec      | type0:11588 usec      | type0: 5978 usec      |
|      | type1: 39039 usec     | type1:34750 usec      | type1:17416 usec      |
|      | type2:42408 usec      | type2:44587 usec      | type2: 22551 usec     |
|      | type3: 37980 usec     | type3:45218 usec      | type3:  22973 usec    |
|      |                       |                       |                       |
|      | average response time | average response time | average response time |
|      | type0: 2726 usec      | type0: 2556 usec      | type0: 2330 usec      |
|      | type1: 3123 usec      | type1: 3049 usec      | type1:  2783 usec     |
|      | type2: 2790 usec      | type2: 2634 usec      | type2:  2391 usec     |
|      | type3: 2461 usec      | type3: 2313 usec      | type3:  2066 usec     |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
|      |                       |                       |                       |
| 3200 | average turn time     | average turn time     | average turn time     |
|      | type0:12603 usec      | type0:11857 usec      | type0:5816 usec       |
|      | type1: 38075 usec     | type1:34121 usec      | type1: 17072 usec     |
|      | type2: 44441 usec     | type2:44868 usec      | type2: 22560 usec     |
|      | type3: 39430 usec     | type3:45311 usec      | type3: 22569 usec     |
|      |                       |                       |                       |
|      | average response time | average response time | average response time |
|      | type0:  2386 usec     | type0:2509 usec       | type0:  2645 usec     |
|      | type1:  2795 usec     | type1: 2925 usec      | type1: 3047 usec      |
|      | type2:   2457 usec    | type2: 2577 usec      | type2: 2723 usec      |
|      | type3:   2154 usec    | type3: 2260 usec      | type3:  2357 usec     |
|      |                       |                       |                       |


# Questions
1. How does the value of S affect turnaround time and response time? Is the difference in turnaround time and response time what you expected to see as S and the number of CPUs change? Why or why not?
- From the table above, We can see that Higher S value causes higher turn around time and response time. As the number of CPUs increases, overall system performance improves, resulting in shorter execution times. This enhancement can be attributed to parallelism, allowing multiple tasks to be processed simultaneously.Regarding the time quantum S, the observed reduction in execution times may stem from the fact that longer tasks have increased opportunities for execution. With smaller time quanta, processes are preempted more frequently, enabling the scheduler to allocate CPU time to a greater number of tasks.

2. How does adjusting the S value in the system affect the turnaround time or response time for long-running and I/O tasks specifically? Does it appear to be highly correlated?
-The correlation between S and turnaround/response times is not necessarily straightforward. When i increase S it appears that at some point  turn around and response time decrease for long-running task and I/O task ,  afterwards i incraese S value more,  then it started increasing turn around time and response time for the tasks. So, it seems that long job will eventually starve for more higher S value.

