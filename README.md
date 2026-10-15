# Hive Mind Simulator

Multi-agent simulator in **C++17** that coordinates **30+ autonomous delivery agents**. Built on a modular OOP architecture, it uses **A\*** and **BFS** pathfinding together with **dynamic priority allocation** to decide which agent handles which delivery.

Compared to a greedy baseline, the coordinated strategy achieves a **45% increase in simulated profit**.

## Features

- Simulation of 30+ autonomous delivery agents working at the same time
- Pathfinding with A\* and BFS
- Dynamic priority allocation for assigning deliveries to agents
- Benchmark against a greedy baseline strategy
- Modular object-oriented design, easy to extend with new agents or strategies

## Technologies

- C++17
- Object-Oriented Programming
- Graph search algorithms (A\*, BFS)

## Results

| Strategy | Simulated profit |
|---|---|
| Greedy baseline | reference |
| Hive Mind (A\* / BFS + dynamic priority) | **+45%** |

## Build and run

Requires a compiler with C++17 support (GCC, Clang or MSVC).

```bash
g++ -std=c++17 -O2 src/*.cpp -o hive_mind
./hive_mind
```
