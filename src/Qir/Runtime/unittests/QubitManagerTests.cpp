// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <iostream>

#include "catch.hpp"

#include "QubitManager.hpp"

using namespace Microsoft::Quantum;

TEST_CASE("Simple allocation and release of one qubit", "[QubitManagerBasic]")
{
    std::unique_ptr<CQubitManager> qm = std::make_unique<CQubitManager>();
    Qubit q = qm->Allocate();
    qm->Release(q);
}

TEST_CASE("Allocation and reallocation of one qubit", "[QubitManagerBasic]")
{
    std::unique_ptr<CQubitManager> qm = std::make_unique<CQubitManager>(1, false, true);
    REQUIRE(qm->GetFreeQubitCount() == 1);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
    Qubit q = qm->Allocate();
    REQUIRE(qm->GetQubitId(q) == 0);
    REQUIRE(qm->GetFreeQubitCount() == 0);
    REQUIRE(qm->GetAllocatedQubitCount() == 1);
    REQUIRE_THROWS(qm->Allocate());
    REQUIRE(qm->GetFreeQubitCount() == 0);
    REQUIRE(qm->GetAllocatedQubitCount() == 1);
    qm->Release(q);
    REQUIRE(qm->GetFreeQubitCount() == 1);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
    Qubit q0 = qm->Allocate();
    REQUIRE(qm->GetQubitId(q0) == 0);
    REQUIRE(qm->GetFreeQubitCount() == 0);
    REQUIRE(qm->GetAllocatedQubitCount() == 1);
    qm->Release(q0);
    REQUIRE(qm->GetFreeQubitCount() == 1);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
}

TEST_CASE("Qubit Status", "[QubitManagerBasic]")
{
    std::unique_ptr<CQubitManager> qm = std::make_unique<CQubitManager>(2, false, true);

    Qubit q0 = qm->Allocate();
    CQubitManager::QubitIdType q0id = qm->GetQubitId(q0);
    REQUIRE(qm->IsValidQubit(q0));
    REQUIRE(qm->IsExplicitlyAllocatedQubit(q0));
    REQUIRE(!qm->IsDisabledQubit(q0));
    REQUIRE(!qm->IsFreeQubitId(q0id));
    REQUIRE(qm->GetAllocatedQubitCount() == 1);

    qm->Disable(q0);
    REQUIRE(qm->IsValidQubit(q0));
    REQUIRE(!qm->IsExplicitlyAllocatedQubit(q0));
    REQUIRE(qm->IsDisabledQubit(q0));
    REQUIRE(!qm->IsFreeQubitId(q0id));
    REQUIRE(qm->GetDisabledQubitCount() == 1);

    qm->Release(q0);
    REQUIRE(!qm->IsFreeQubitId(q0id));
    REQUIRE(qm->GetFreeQubitCount() == 1);

    Qubit q1 = qm->Allocate();
    CQubitManager::QubitIdType q1id = qm->GetQubitId(q1);
    REQUIRE(q0id != q1id);

    REQUIRE(qm->IsValidQubit(q1));
    REQUIRE(qm->IsExplicitlyAllocatedQubit(q1));
    REQUIRE(!qm->IsDisabledQubit(q1));
    REQUIRE(!qm->IsFreeQubitId(q1id));

    REQUIRE(qm->GetAllocatedQubitCount() == 1);
    REQUIRE(qm->GetDisabledQubitCount() == 1);
    REQUIRE(qm->GetFreeQubitCount() == 0);

    qm->Release(q1);
    REQUIRE(qm->IsFreeQubitId(q1id));
}

TEST_CASE("Qubit Counts", "[QubitManagerBasic]")
{
    constexpr int totalQubitCount = 100;
    constexpr int disabledQubitCount = 29;
    constexpr int extraQubitCount = 43;
    static_assert(extraQubitCount <= totalQubitCount);
    static_assert(disabledQubitCount <= totalQubitCount);
    // We don't want capacity to be extended at first...
    static_assert(extraQubitCount + disabledQubitCount <= totalQubitCount);

    std::unique_ptr<CQubitManager> qm = std::make_unique<CQubitManager>(totalQubitCount, true, true);
    REQUIRE(qm->GetQubitCapacity() == totalQubitCount);
    REQUIRE(qm->GetFreeQubitCount() == totalQubitCount);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
    REQUIRE(qm->GetDisabledQubitCount() == 0);

    Qubit* qubits = new Qubit[disabledQubitCount];
    qm->Allocate(qubits, disabledQubitCount);
    REQUIRE(qm->GetQubitCapacity() == totalQubitCount);
    REQUIRE(qm->GetFreeQubitCount() == totalQubitCount-disabledQubitCount);
    REQUIRE(qm->GetAllocatedQubitCount() == disabledQubitCount);
    REQUIRE(qm->GetDisabledQubitCount() == 0);

    qm->Disable(qubits, disabledQubitCount);
    REQUIRE(qm->GetQubitCapacity() == totalQubitCount);
    REQUIRE(qm->GetFreeQubitCount() == totalQubitCount-disabledQubitCount);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
    REQUIRE(qm->GetDisabledQubitCount() == disabledQubitCount);

    qm->Release(qubits, disabledQubitCount);
    REQUIRE(qm->GetQubitCapacity() == totalQubitCount);
    REQUIRE(qm->GetFreeQubitCount() == totalQubitCount-disabledQubitCount);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
    REQUIRE(qm->GetDisabledQubitCount() == disabledQubitCount);
    delete[] qubits;

    qubits = new Qubit[extraQubitCount];
    qm->Allocate(qubits, extraQubitCount);
    REQUIRE(qm->GetQubitCapacity() == totalQubitCount);
    REQUIRE(qm->GetFreeQubitCount() == totalQubitCount-disabledQubitCount-extraQubitCount);
    REQUIRE(qm->GetAllocatedQubitCount() == extraQubitCount);
    REQUIRE(qm->GetDisabledQubitCount() == disabledQubitCount);

    qm->Release(qubits, extraQubitCount);
    REQUIRE(qm->GetQubitCapacity() == totalQubitCount);
    REQUIRE(qm->GetFreeQubitCount() == totalQubitCount-disabledQubitCount);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
    REQUIRE(qm->GetDisabledQubitCount() == disabledQubitCount);
    delete[] qubits;

    qubits = new Qubit[totalQubitCount];
    qm->Allocate(qubits, totalQubitCount);
    REQUIRE(qm->GetQubitCapacity() > totalQubitCount);
    REQUIRE(qm->GetAllocatedQubitCount() == totalQubitCount);
    REQUIRE(qm->GetDisabledQubitCount() == disabledQubitCount);

    qm->Release(qubits, totalQubitCount);
    REQUIRE(qm->GetQubitCapacity() > totalQubitCount);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
    REQUIRE(qm->GetDisabledQubitCount() == disabledQubitCount);
    delete[] qubits;
}

TEST_CASE("Allocation of released qubits when reuse is encouraged", "[QubitManagerBasic]")
{
    std::unique_ptr<CQubitManager> qm = std::make_unique<CQubitManager>(2, false, true);
    REQUIRE(qm->GetFreeQubitCount() == 2);
    Qubit q0 = qm->Allocate();
    Qubit q1 = qm->Allocate();
    REQUIRE(qm->GetQubitId(q0) == 0); // Qubit ids should be in order
    REQUIRE(qm->GetQubitId(q1) == 1);
    REQUIRE_THROWS(qm->Allocate());
    REQUIRE(qm->GetFreeQubitCount() == 0);
    REQUIRE(qm->GetAllocatedQubitCount() == 2);

    qm->Release(q0);
    Qubit q0a = qm->Allocate();
    REQUIRE(qm->GetQubitId(q0a) == 0); // It was the only one available
    REQUIRE_THROWS(qm->Allocate());

    qm->Release(q1);
    qm->Release(q0a);
    REQUIRE(qm->GetFreeQubitCount() == 2);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);

    Qubit q0b = qm->Allocate();
    Qubit q1a = qm->Allocate();
    REQUIRE(qm->GetQubitId(q0b) == 0); // By default reuse is encouraged, LIFO is used
    REQUIRE(qm->GetQubitId(q1a) == 1);
    REQUIRE_THROWS(qm->Allocate());
    REQUIRE(qm->GetFreeQubitCount() == 0);
    REQUIRE(qm->GetAllocatedQubitCount() == 2);

    qm->Release(q0b);
    qm->Release(q1a);
}

TEST_CASE("Extending capacity", "[QubitManager]")
{
    std::unique_ptr<CQubitManager> qm = std::make_unique<CQubitManager>(1, true, true);

    Qubit q0 = qm->Allocate();
    REQUIRE(qm->GetQubitId(q0) == 0);
    Qubit q1 = qm->Allocate(); // This should double capacity
    REQUIRE(qm->GetQubitId(q1) == 1);
    REQUIRE(qm->GetFreeQubitCount() == 0);
    REQUIRE(qm->GetAllocatedQubitCount() == 2);

    qm->Release(q0);
    Qubit q0a = qm->Allocate();
    REQUIRE(qm->GetQubitId(q0a) == 0);
    Qubit q2 = qm->Allocate(); // This should double capacity again
    REQUIRE(qm->GetQubitId(q2) == 2);
    REQUIRE(qm->GetFreeQubitCount() == 1);
    REQUIRE(qm->GetAllocatedQubitCount() == 3);

    qm->Release(q1);
    qm->Release(q0a);
    qm->Release(q2);
    REQUIRE(qm->GetFreeQubitCount() == 4);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);

    Qubit* qqq = new Qubit[3];
    qm->Allocate(qqq, 3);
    REQUIRE(qm->GetFreeQubitCount() == 1);
    REQUIRE(qm->GetAllocatedQubitCount() == 3);
    qm->Release(qqq, 3);
    delete[] qqq;
    REQUIRE(qm->GetFreeQubitCount() == 4);
    REQUIRE(qm->GetAllocatedQubitCount() == 0);
}

TEST_CASE("Restricted Area", "[QubitManager]")
{
    std::unique_ptr<CQubitManager> qm = std::make_unique<CQubitManager>(3, false, true);

    Qubit q0 = qm->Allocate();
    REQUIRE(qm->GetQubitId(q0) == 0);

    qm->StartRestrictedReuseArea();

    // Allocates fresh qubit
    Qubit q1 = qm->Allocate();
    REQUIRE(qm->GetQubitId(q1) == 1);
    qm->Release(q1); // Released, but cannot be used in the next segment.

    qm->NextRestrictedReuseSegment();

    // Allocates fresh qubit, q1 cannot be reused - it belongs to a differen segment.
    Qubit q2 = qm->Allocate();
    REQUIRE(qm->GetQubitId(q2) == 2);
    qm->Release(q2);

    Qubit q2a = qm->Allocate(); // Getting the same one as the one that's just released.
    REQUIRE(qm->GetQubitId(q2a) == 2);
    qm->Release(q2a); // Released, but cannot be used in the next segment.

    qm->NextRestrictedReuseSegment();

    // There's no qubits left here. q0 is allocated, q1 and q2 are from different segments.
    REQUIRE_THROWS(qm->Allocate());

    qm->EndRestrictedReuseArea();

    // Qubits 1 and 2 are available here again.
    Qubit* qqq = new Qubit[2];
    qm->Allocate(qqq, 2);
    // OK to destruct qubit manager while qubits are still allocated.
    REQUIRE_THROWS(qm->Allocate());
}

