# RISC-V Emulation - Analysis - Cache Simulation

## Overview
This project involves writing an emulator in C for a subset of the RISC-V Instruction Set Architecture (ISA). It focuses not only on emulating a select list of provided and self-written RISC-V assembly programs but also on implementing a dynamic analysis of instruction execution and simulating various configurations of processor caches.

## Emulator Requirements
- **Subset Emulation:** Emulate enough of the RISC-V ISA to run the following programs: `quadratic_s`, `midpoint_s`, `max3_s`, `to_upper`, `get_bitseq_s`, `get_bitseq_signed_s`, `swap_s`, `sort_s`, `fib_rec_s` (your implementation), and `eval_s` (your implementation).
- **Dynamic Analysis Metrics:** Collect metrics on instruction execution, including counts of executed instructions, I-type and R-type instructions, LOAD and STORE instructions, jump instructions, and conditional branches (taken and not taken).
- **Cache Simulation:** Implement cache simulators for direct mapped caches (1-word and 4-word blocks) and 4-way set associative caches (1-word and 4-word blocks with LRU slot replacement).

## Cache Simulator Types
- Direct mapped cache with a block size of 1 word (given)
- Direct mapped cache with a block size of 4 words
- 4-way set associative cache with a block size of 1 word and LRU slot replacement
- 4-way set associative cache with a block size of 4 words and LRU slot replacement

## Support and Guidance
Throughout the lecture and lab sessions, we will cover:
- Decoding machine code and executing the specified operations.
- Direct-mapped cache design, along with data structures and algorithms for set-associative caches.
- **Guide to Cache Memory:** A comprehensive guide to help with cache implementation.

## Grading Rubric
### Automated Testing
- **90 pts:** Passing the automated tests provided through the autograder.

### Interactive Grading
- **10 pts:** Code walkthrough focusing on the implementation of dynamic analysis and instruction cache simulation.

## Code Quality
Maintain a clean repository with consistent naming, indentation, and the avoidance of dead or unnecessarily complex code. Any deductions in this area can be earned back with improvements.

---
