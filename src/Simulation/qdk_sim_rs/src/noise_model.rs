use crate::common_matrices;
use crate::instrument::Instrument;
use crate::linalg::HasDagger;
use crate::processes::Process;
use crate::processes::ProcessData::{KrausDecomposition, Unitary};
use crate::states::State;
use crate::states::StateData::Mixed;
use crate::C64;
use num_traits::{One, Zero};

use serde::{Deserialize, Serialize};

#[cfg(feature = "python")]
use pyo3::prelude::*;

/// A description of the noise that applies to the state of a quantum system
/// as the result of applying operations.
#[cfg_attr(feature = "python", pyclass(name = "NoiseModel"))]
#[derive(Serialize, Deserialize, Debug)]
pub struct NoiseModel {
    /// The initial state that freshly allocated qubits start off in.
    pub initial_state: State,

    /// The process that applies to the state of a simulator
    /// when the `I` operation is called.
    pub i: Process,

    /// The process that applies to the state of a simulator
    /// when the `X` operation is called.
    pub x: Process,

    /// The process that applies to the state of a simulator
    /// when the `Y` operation is called.
    pub y: Process,

    /// The process that applies to the state of a simulator
    /// when the `Z` operation is called.
    pub z: Process,

    /// The process that applies to the state of a simulator
    /// when the `H` operation is called.
    pub h: Process,

    /// The process that applies to the state of a simulator
    /// when the `S` operation is called.
    pub s: Process,

    /// The process that applies to the state of a simulator
    /// when the `Adjoint S` operation is called.
    pub s_adj: Process,

    /// The process that applies to the state of a simulator
    /// when the `T` operation is called.
    pub t: Process,

    /// The process that applies to the state of a simulator
    /// when the `Adjoint T` operation is called.
    pub t_adj: Process,

    /// The process that applies to the state of a simulator
    /// when the `CNOT` operation is called.
    pub cnot: Process,

    /// The instrument that is used to the measure the state of a simulator
    /// in the $Z$-basis.
    pub z_meas: Instrument,
}

impl NoiseModel {
    /// Returns a serialization of this noise model as a JSON object.
    pub fn as_json(&self) -> String {
        serde_json::to_string(&self).unwrap()
    }

    /// Returns a copy of the ideal noise model; that is, a noise model
    /// describing the case in which no noise acts on the quantum system.
    pub fn ideal() -> NoiseModel {
        let i = Process {
            n_qubits: 1,
            data: Unitary(common_matrices::i()),
        };
        let z = Process {
            n_qubits: 1,
            data: Unitary(common_matrices::z()),
        };
        let z_meas = Instrument {
            effects: vec![
                Process {
                    n_qubits: 1,
                    data: KrausDecomposition(array![[
                        [C64::one(), C64::zero()],
                        [C64::zero(), C64::zero()]
                    ]]),
                },
                Process {
                    n_qubits: 1,
                    data: KrausDecomposition(array![[
                        [C64::zero(), C64::zero()],
                        [C64::zero(), C64::one()]
                    ]]),
                },
            ],
        };
        NoiseModel {
            initial_state: State {
                n_qubits: 1,
                data: Mixed((common_matrices::i() + common_matrices::z()) / 2.0),
            },
            i,
            x: Process {
                n_qubits: 1,
                data: Unitary(common_matrices::x()),
            },
            y: Process {
                n_qubits: 1,
                data: Unitary(common_matrices::y()),
            },
            z,
            h: Process {
                n_qubits: 1,
                data: Unitary(common_matrices::h()),
            },
            t: Process {
                n_qubits: 1,
                data: Unitary(common_matrices::t()),
            },
            t_adj: Process {
                n_qubits: 1,
                data: Unitary(common_matrices::t().dag()),
            },
            s: Process {
                n_qubits: 1,
                data: Unitary(common_matrices::s()),
            },
            s_adj: Process {
                n_qubits: 1,
                data: Unitary(common_matrices::s().dag()),
            },
            cnot: Process {
                n_qubits: 2,
                data: Unitary(common_matrices::cnot()),
            },
            z_meas,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn can_serialize_noise_model() {
        let noise_model = NoiseModel::ideal();
        let _json = serde_json::to_string(&noise_model);
    }
}