// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;
using Microsoft.Quantum.Simulation.Core;

namespace Microsoft.Quantum.Simulation.Simulators
{

    public partial class QuantumSimulator
    {
        public class QSimRx : Intrinsic.Rx
        {
            [DllImport(QSIM_DLL_NAME, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl, EntryPoint = "R")]
            private static extern void R(uint id, Pauli basis, double angle, uint qubit);

            [DllImport(QSIM_DLL_NAME, ExactSpelling = true, CallingConvention = CallingConvention.Cdecl, EntryPoint = "MCR")]
            private static extern void MCR(uint id, Pauli basis, double angle, uint count, uint[] ctrls, uint qubit);

            private QuantumSimulator Simulator { get; }

            public QSimRx(QuantumSimulator m) : base(m)
            {
                this.Simulator = m;
            }

            public override Func<(double, Qubit), QVoid> __Body__ => (args) =>
            {
                var (angle, target) = args;
                Simulator.CheckQubit(target, nameof(target));
                CheckAngle(angle);
                R(Simulator.Id, Pauli.PauliX, angle, (uint)target.Id);
                return QVoid.Instance;
            };

            public override Func<(double, Qubit), QVoid> __AdjointBody__ => (_args) =>
            {
                var (angle, q1) = _args;

                return this.__Body__.Invoke((-angle, q1));
            };

            public override Func<(IQArray<Qubit>, (double, Qubit)), QVoid> __ControlledBody__ => (args) =>
            {
                var (ctrls, (angle, target)) = args;
                Simulator.CheckQubits(ctrls, target);
                CheckAngle(angle);
                MCR(Simulator.Id, Pauli.PauliX, angle, (uint)ctrls.Length, ctrls.GetIds(), (uint)target.Id);
                return QVoid.Instance;
            };

            public override Func<(IQArray<Qubit>, (double, Qubit)), QVoid> __ControlledAdjointBody__ => (_args) =>
            {
                var (ctrls, (angle, q1)) = _args;

                return this.__ControlledBody__.Invoke((ctrls, (-angle, q1)));
            };
        }
    }
}