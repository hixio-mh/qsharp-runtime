// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// This test uses all operations from the quantum instruction set that targets full state simulator.
// We are not validating the result, only that the test can compile and run against the simulator.
namespace Microsoft.Quantum.Testing.QIR
{
    open Microsoft.Quantum.Intrinsic;

    operation InvokeAllVariants(op : (Qubit => Unit is Adj + Ctl)) : Int
    {
        mutable res = 0;
        using((target, ctls) = (Qubit(), Qubit[2]))
        {
            op(target);
            Adjoint op(target);
            if (M(target) != Zero) { let res = 1; }
            else
            {
                H(ctls[0]);
                H(ctls[1]);
                Controlled op(ctls, target);
                Adjoint Controlled op(ctls, target);
                if (M(target) != Zero) { return 2; }
                H(ctls[0]);
                H(ctls[1]);
            }
        }
        return res;
    }

    @EntryPoint()
    operation Test_Simulator_QIS() : Int
    {
        mutable res = 0;
        let res = InvokeAllVariants(X);
        if (res != 0) { return res; }

        let res = InvokeAllVariants(Y);
        if (res != 0) { return 10 + res; }

        let res = InvokeAllVariants(Z);
        if (res != 0) { return 20 + res; }

        let res = InvokeAllVariants(H);
        if (res != 0) { return 30 + res; }

        let res = InvokeAllVariants(S);
        if (res != 0) { return 40 + res; }

        let res = InvokeAllVariants(T);
        if (res != 0) { return 50 + res; }

        let res = InvokeAllVariants(R(PauliX, 0.42, _));
        if (res != 0) { return 60 + res; }

        using((targets, ctls) = (Qubit[2], Qubit[2]))
        {
            let theta = 0.42;
            Exp([PauliX, PauliY], theta, targets);
            Adjoint Exp([PauliX, PauliY], theta, targets);
            if (M(target) != Zero) { let res = 1; }

            H(ctls[0]);
            H(ctls[1]);
            Controlled Exp(ctls, ([PauliX, PauliY], theta, targets));
            Adjoint Controlled Exp(ctls, ([PauliX, PauliY], theta, targets));
            H(ctls[0]);
            H(ctls[1]);
            if (M(target) != Zero) { let res = 70 + 2; }
        }
        if (res != 0) { return 70 + res; }

        using (qs = Qubit[3])
        {
            H(qs[0]);
            H(qs[2]);
            if (Measure([PauliX, PauliZ, PauliX], qs) != zero) { let res = 80; }
        }
        return res;
    }
}
