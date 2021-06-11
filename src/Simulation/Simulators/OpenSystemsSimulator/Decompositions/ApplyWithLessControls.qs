// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// NB: Copied from Utils.qs.
namespace Microsoft.Quantum.Experimental.Decompositions {
    open Microsoft.Quantum.Intrinsic;

    /// Given a multiply-controlled operation that requires k controls 
    /// applies it using ceiling(k/2) controls and using floor(k/2) temporary qubits
    internal operation ApplyWithLessControlsA<'T> (op : ((Qubit[],'T) => Unit is Adj), (controls : Qubit[], arg : 'T)) : Unit is Adj {
        let numControls = Length(controls);
        let numControlPairs = numControls / 2;
        use temps = Qubit[numControlPairs] {
            within {
                for numPair in 0 .. numControlPairs - 1 { // constant depth
                    PhaseCCX(controls[2 * numPair], controls[2 * numPair + 1], temps[numPair]);
                }
            }
            apply {
                let newControls = numControls % 2 == 0 ? temps | temps + [controls[numControls - 1]];
                op(newControls, arg);
            }
        }
    }

    internal operation PhaseCCX (control1 : Qubit, control2 : Qubit, target : Qubit) : Unit is Adj {
        // https://arxiv.org/pdf/1210.0974.pdf#page=2
        H(target);
        CNOT(target,control1);
        CNOT(control1,control2);
        T(control2);
        Adjoint T(control1);
        T(target);
        CNOT(target,control1);
        CNOT(control1,control2);
        Adjoint T(control2);
        CNOT(target,control2);
        H(target);
    }
}