#include <iostream>
#include <vector>
#include <complex>
#include <cmath>

using Complex = std::complex<double>;
using StateVector = std::vector<Complex>;
using Matrix2x2 = std::vector<std::vector<Complex>>;

class QuantumRegister {
private:
    int num_qubits;
    StateVector state_vector;
                        
    void apply_single_qubit_gate(int target_qubit, const Matrix2x2& U) {
        size_t size = state_vector.size();
        StateVector next_state(size, Complex(0.0, 0.0));

        for (size_t i = 0; i < size; ++i) {
            int bit = (i >> target_qubit) & 1;
            size_t zero_index = i & ~(1ULL << target_qubit);
            size_t one_index = i | (1ULL << target_qubit);

            if (bit == 0) {
                next_state[i] += U[0][0] * state_vector[zero_index] + U[0][1] * state_vector[one_index];
            } else {
                next_state[i] += U[1][0] * state_vector[zero_index] + U[1][1] * state_vector[one_index];
            }
        }
        state_vector = next_state;
    }

public:
    QuantumRegister(int qubits) : num_qubits(qubits) {
        size_t total_states = 1 << num_qubits; 
        state_vector.assign(total_states, Complex(0.0, 0.0));
        state_vector[0] = Complex(1.0, 0.0);
    }

    void X(int target) {
        Matrix2x2 x_matrix = {
            {Complex(0.0, 0.0), Complex(1.0, 0.0)},
            {Complex(1.0, 0.0), Complex(0.0, 0.0)}
        };
        apply_single_qubit_gate(target, x_matrix);
    }

    void H(int target) {
        double inv_sqrt2 = 1.0 / std::sqrt(2.0);
        Matrix2x2 h_matrix = {
            {Complex(inv_sqrt2, 0.0), Complex(inv_sqrt2, 0.0)},
            {Complex(inv_sqrt2, 0.0), Complex(-inv_sqrt2, 0.0)}
        };
        apply_single_qubit_gate(target, h_matrix);
    }

    void CNOT(int control, int target) {
        size_t size = state_vector.size();
        StateVector next_state = state_vector;

        for (size_t i = 0; i < size; ++i) {
            if (((i >> control) & 1) == 1) {
                size_t flipped_index = i ^ (1ULL << target);
                next_state[flipped_index] = state_vector[i];
            }
        }
        state_vector = next_state;
    }

    void Z(int target) {
        Matrix2x2 z_matrix = {
            {Complex(1.0, 0.0), Complex(0.0, 0.0)},
            {Complex(0.0, 0.0), Complex(-1.0, 0.0)}
        };
        apply_single_qubit_gate(target, z_matrix);
    }

    void SWAP(int qubitA, int qubitB) {
        size_t size = state_vector.size();
        StateVector next_state = state_vector;

        for (size_t i = 0; i < size; ++i) {
            int bitA = (i >> qubitA) & 1;
            int bitB = (i >> qubitB) & 1;

            if (bitA != bitB) {
                size_t swapped_index = i ^ ((1ULL << qubitA) | (1ULL << qubitB));
                next_state[swapped_index] = state_vector[i];
            }
        }
        state_vector = next_state;
    }

    void display_probabilities() const {
        std::cout << "--- Quantum State Probability Output ---\n";
        for (size_t i = 0; i < state_vector.size(); ++i) {
            double probability = std::norm(state_vector[i]);
            if (probability > 1e-6) {
                std::cout << "|";
                for (int bit = num_qubits - 1; bit >= 0; --bit) {
                    std::cout << ((i >> bit) & 1);
                }
                std::cout << "> : " << (probability * 100.0) << "%\n";
            }
        }
        std::cout << "----------------------------------------\n";
    }
};

int main() {
    std::cout << "=== Testing Gate Operations ===\n\n";

    // Initialize a 3-qubit register
    QuantumRegister reg(3);

    // Test 1: Fire X gate on Qubit 0
    std::cout << "Applying X(0) [Should flip state to |001>]:\n";
    reg.X(0);
    reg.display_probabilities();

    // Test 2: Fire SWAP gate to swap Qubit 0 and Qubit 2
    std::cout << "Applying SWAP(0, 2) [Should move state from |001> to |100>]:\n";
    reg.SWAP(0, 2);
    reg.display_probabilities();

    // Test 3: Fire H gate on Qubit 1 to introduce a clean superposition
    std::cout << "Applying H(1) [Should create 50/50 superposition of |100> and |110>]:\n";
    reg.H(1);
    reg.display_probabilities();

    // Test 4: NEW - Fire CNOT gate (Control: Qubit 1, Target: Qubit 0)
    // Since Qubit 1 is split between |100> and |110>, it is active (1) only in |110>.
    // This will flip Qubit 0 for that state, transforming |110> into |111>.
    std::cout << "Applying CNOT(control=1, target=0) [Should shift states to |100> and |111>]:\n";
    reg.CNOT(1, 0);
    reg.display_probabilities();

    // Test 5: Fire Z gate to verify phase capability
    std::cout << "Applying Z(1) [Phase modification occurs; probabilities remain identical]:\n";
    reg.Z(1);
    reg.display_probabilities();

    return 0;
}
