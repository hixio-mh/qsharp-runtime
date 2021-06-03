﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using Microsoft.Quantum.Qir.Tools.Driver;
using Microsoft.Quantum.Qir.Utility;

namespace Microsoft.Quantum.Qir.Tools.Executable
{
    /// <summary>
    /// Class to create and run QIR-based executables that use the full-state simulator.
    /// </summary>
    public class QirFullStateExecutable : QirExecutable
    {
        public override string DriverFileExtension => "cpp";

        public override IList<string> LinkLibraries => new List<string> {
                "Microsoft.Quantum.Qir.Runtime",
                "Microsoft.Quantum.Qir.QSharp.Foundation",
                "Microsoft.Quantum.Qir.QSharp.Core"
            };

        public override IList<DirectoryInfo> HeaderDirectories { get; } = new List<DirectoryInfo>();

        public override IList<DirectoryInfo> LibraryDirectories { get; } = new List<DirectoryInfo>();

        public QirFullStateExecutable(FileInfo executableFile, byte[] qirBytecode, ILogger logger = null)
            : base(executableFile,
                  qirBytecode,
                  new QirFullStateDriverGenerator(),
                  logger)
        {
            var thisModulePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            if (string.IsNullOrWhiteSpace(thisModulePath))
            {
                throw new InvalidOperationException("Could not get a path for the current assembly location.");
            }
            HeaderDirectories.Add(new DirectoryInfo(Path.Combine(thisModulePath, "runtimes", "any", "native", "include")));
            HeaderDirectories.Add(new DirectoryInfo(Path.Combine(thisModulePath, "Externals", "CLI11")));

            var osID = Environment.OSVersion.Platform switch
            {
                PlatformID.Win32NT => "win-x64",
                PlatformID.Unix => "linux-x64",
                PlatformID.MacOSX => "osx-x64",
                _ => throw new ArgumentException("Unsupported operating system architecture."),
            };
            LibraryDirectories.Add(new DirectoryInfo(Path.Combine(thisModulePath, "runtimes", osID, "native")));
            LibraryDirectories.Add(new DirectoryInfo(Path.Combine(thisModulePath, "Simulators", osID)));
        }
    }
}
