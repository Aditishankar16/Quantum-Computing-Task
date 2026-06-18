#include <iostream>
#include <vector>
#include <complex>
#include <cmath>

using namespace std;

class QuantumState{
public:
    int qubits;
    vector<complex<double>> state;
    QuantumState(int n)
{
  qubits = n;
  int size = pow(2,n);
  state.resize(size,0);
  // initial state |000....0>
  state[0] = 1;
}
void displayState()
{
  cout<< "Quantum State:"<< endl;
  for(int i=0; i< state.size(); i++){
    cout<< "|"<<i<< ">:" << state[i]<< endl;
  }
}
void showProbability()
{
  cout<< "\n Measurement Probabilities: "<< endl;
  for (int i=0; i<state.size(); i++)
    {
      double probability = norm(state[i]);
      cout<< "State |"<< i<< ">:"<< probability<< endl;
    }
}
};

int main()
{
  QuantumState q(2);
  q.displayState();
  q.showProbability();
  return 0;
}













