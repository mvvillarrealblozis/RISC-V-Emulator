# RISC-V Emulation and Cache Simulation Project

## Project Introduction
This project showcases my journey in developing an emulator for a subset of the RISC-V Instruction Set Architecture (ISA), focusing on dynamic analysis of instruction execution and simulating various cache configurations. My aim was to not only emulate a series of both provided and personally developed RISC-V assembly programs but also to offer insights into instruction execution patterns and cache performance under different scenarios.

## Emulation and Analysis
I embarked on creating an emulator capable of interpreting a select group of RISC-V assembly programs. The motivation was to delve deeper into how different instructions interact within a simulated environment and to understand the nuances of RISC-V ISA. My emulator gathers detailed execution metrics, providing valuable data on:

- Total instructions executed
- Breakdown of I-type and R-type instructions
- Load and Store operations
- Jump instruction occurrences
- Conditional branch outcomes (both taken and not taken)

## Cache Simulation
One of the project's highlights is the implementation of a processor cache simulator. This component was designed to explore the impact of cache architecture variations on processing efficiency. I implemented and analyzed the performance of:

- Direct mapped cache (with block sizes of 1 word and 4 words)
- 4-way set associative cache (with block sizes of 1 word and 4 words, incorporating Least Recently Used (LRU) slot replacement)

## Development Process
My development process involved iterative testing and refinement. Starting with a basic emulation logic from a previous lab exercise, I gradually integrated more complex functionalities like dynamic analysis and cache simulation. Key resources that guided my development include:

- Instruction set documentation for decoding logic
- Cache design principles for simulating different cache architectures

## Challenges and Solutions
Throughout the project, I encountered several challenges, particularly in accurately simulating cache behavior and implementing dynamic analysis without impacting emulator performance. Solutions involved deep dives into cache memory concepts, optimizing data structures for performance, and employing a modular design approach to keep the emulator's core functionality separate from the analysis components.
