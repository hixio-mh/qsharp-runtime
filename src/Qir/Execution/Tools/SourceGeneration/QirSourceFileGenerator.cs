﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Threading.Tasks;
using Microsoft.Quantum.Qir.Utility;
using Microsoft.Quantum.QsCompiler.BondSchemas.EntryPoint;

namespace Microsoft.Quantum.Qir.Tools.SourceGeneration
{
    public class QirSourceFileGenerator : IQirSourceFileGenerator
    {
        public delegate Task DriverGenerationMethod(Stream stream);

        private const string BytecodeFileName = "qir.bc";
        private const string DriverFileName = "driver.cpp";
        private readonly ILogger logger;
        private readonly DriverGenerationMethod driverGenerationMethod;

        public QirSourceFileGenerator(ILogger logger, DriverGenerationMethod driverGenerationMethod)
        {
            this.logger = logger;
            this.driverGenerationMethod = driverGenerationMethod;
        }

        public async Task GenerateQirSourceFilesAsync(DirectoryInfo sourceDirectory, EntryPointOperation entryPointOperation, ArraySegment<byte> bytecode)
        {
            if (sourceDirectory.Exists)
            {
                sourceDirectory.Delete(true);
            }

            sourceDirectory.Create();
            logger.LogInfo($"Created source directory at {sourceDirectory.FullName}.");

            // Create driver.
            var driverFile = new FileInfo(Path.Combine(sourceDirectory.FullName, DriverFileName));
            using var driverFileStream = driverFile.OpenWrite();
            await driverGenerationMethod.Invoke(driverFileStream);
            logger.LogInfo($"Created driver file at {driverFile.FullName}.");

            // Create bytecode file.
            var bytecodeFile = new FileInfo(Path.Combine(sourceDirectory.FullName, BytecodeFileName));
            using var bytecodeFileStream = bytecodeFile.OpenWrite();
            await bytecodeFileStream.WriteAsync(bytecode.Array, bytecode.Offset, bytecode.Count);
            logger.LogInfo($"Created bytecode file at {bytecodeFile.FullName}.");
        }
    }
}