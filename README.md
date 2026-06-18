# Quantum Circuit Simulator using C++
part 4, 5 and 6 submitted in a google doc attached with the task .
## Overview

This project implements a small quantum circuit simulator
using C++ without using external quantum libraries.

The simulator supports:

- Quantum state representation
- X gate
- H gate
- Z gate
- CNOT gate
- SWAP gate
- Bernstein Vazirani algorithm
- Performance analysis


## Concepts Implemented

### Quantum State

A quantum state is represented using a state vector:

|ψ> = α|0> + β|1>


Each element stores a complex probability amplitude.


## Gates

Implemented gates:

X:
Bit flip operation

H:
Creates superposition

Z:
Phase change operation

CNOT:
Controlled operation

SWAP:
Exchange qubit states


## Execution

Compile:

g++ filename.cpp -o output


Run:

./output


## Limitations

The simulator uses classical memory,
therefore state size grows exponentially:

2^n

Large quantum systems become difficult to simulate.
