# CSE2025-Project-3
A project about implementing preemptive scheduling system using binomial heaps within Data Structers course.

## How it works
Program reads processes from input.txt file and stores them. By changing quantum size (a time period which processer checks processes) processer computes every processes priority and process most prior process. Meanwhile other processes waits their turn in binomial heap. Finally program finds optimized quantum size which causes processes wait less time by comparing average waiting times, then prints all the information about processes at all quantum sizes.  
