﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace Microsoft.Azure.Quantum
{
    using global::Azure.Quantum.Jobs.Models;

    using Microsoft.Azure.Quantum.Utility;

    /// <summary>
    /// Wrapper for Azure.Quantum.Jobs.Models.QuantumJobQuota.
    /// </summary>
    public class QuotaInfo
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="QuotaInfo"/> class.
        /// </summary>
        /// <param name="workspace">The workspace.</param>
        /// <param name="quota">The job details.</param>
        public QuotaInfo(IWorkspace workspace, QuantumJobQuota quota)
        {
            Ensure.NotNull(workspace, nameof(workspace));
            Ensure.NotNull(quota, nameof(quota));

            Workspace = workspace;
            Quota = quota;
        }

        /// <summary>
        /// Gets the workspace.
        /// </summary>
        public virtual IWorkspace Workspace { get; private set; }

        /// <summary>
        /// Gets the quota information.
        /// </summary>
        public virtual QuantumJobQuota Quota { get; private set; }
    }
}
