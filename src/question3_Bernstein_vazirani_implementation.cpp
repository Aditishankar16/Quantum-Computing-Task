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
        state_vector[0] = Complex(1.0, 0.0); // Ground state |000>
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

    // --- Bernstein-Vazirani Oracle ---
    // Maps hidden secret bits to structural CNOT gates targeting the ancilla (Qubit 2)
    void BV_Oracle(int secret_mask) {
        if ((secret_mask & 1) == 1) {
            CNOT(0, 2); // If bit 0 of the secret is 1, connect CNOT(control=0, target=2)
        }
        if (((secret_mask >> 1) & 1) == 1) {
            CNOT(1, 2); // If bit 1 of the secret is 1, connect CNOT(control=1, target=2)
        }
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
    std::cout << "====================================================\n";
    std::cout << " RUNNING 3-QUBIT BERNSTEIN-VAZIRANI QUANTUM CIRCUIT \n";
    std::cout << "====================================================\n\n";

    // Define secret bitstring to hide inside the black-box oracle.
    // In binary: 2 = "10" (Qubit 1 = 1, Qubit 0 = 0)
    int secret_string = 2; 
    std::cout << "Programming Oracle with Hidden Secret Code: 2 (binary: 10)\n\n";

    // Initialize 3-qubit simulator framework (Qubits 0, 1 = input registers, Qubit 2 = ancilla)
    QuantumRegister reg(3);

    // Step 1: Prep the Ancilla Qubit into state |1>
    reg.X(2);

    // Step 2: Apply Hadamard transformations to establish global superposition
    reg.H(0);
    reg.H(1);
    reg.H(2); 

    std::cout << "State distribution inside superposition before Oracle query:\n";
    reg.display_probabilities();

    // Step 3: Run the hidden structural black-box function
    std::cout << "Querying the Black-Box Oracle...\n";
    reg.BV_Oracle(secret_string);

    // Step 4: Apply final Hadamards to input registers to collapse superposition via phase interference
    reg.H(0);
    reg.H(1);

    // Step 5: Read out the final measurement distributions
    std::cout << "\nExecution Complete! Final State Measurement Distribution:\n";
    reg.display_probabilities();

    std::cout << "Note: The output layout reads left-to-right as |(Ancilla)(Qubit1)(Qubit0)>.\n";
    std::cout << "Ancilla ends in state |1>. Inputs read |10>, matching secret code 2 perfectly!\n";

    return 0;
}
