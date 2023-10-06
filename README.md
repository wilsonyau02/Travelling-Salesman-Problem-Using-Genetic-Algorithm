# 🌍 Travelling Salesman Problem Using Genetic Algorithm

> 202305 BMCS3003 Distributed Systems and Parallel Computing

**Description:** We employ distributed and parallel computing techniques such as OpenMP, MPI, and Threading to solve the classic Travelling Salesman Problem (TSP).

## 💡 Techniques Used
- **Language:** C++
- **Parallel Techniques:** OpenMP, MPI, Threading

## ⚙️ MPI Setup

### Configuring MPI in Visual Studio

1. **Download and Install Microsoft MPI:** 
   - Get it [here](https://www.microsoft.com/en-us/download/details.aspx?id=57467).
   - Install `msmpisetup.exe` (for MPI executable file).
   - Install `msmpisdk.msi` (for MPI library).

2. **Include Directories Configuration:**
   - Navigate to: Project properties → C/C++ → Additional Include Directories.
   - Add the path to `mpi.h` (usually at “C:\Program Files (x86)\Microsoft SDKs\MPI\Include”).
   - **Note:** Avoid “...\Include\x86...”.

3. **Library Directories Configuration:**
   - Navigate to: Project properties → Linker → Additional library Directories.
   - Add the path to `msmpi.lib` (usually at “C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x86”).

4. **Additional Dependencies Configuration:**
   - Navigate to: Project properties → Linker → Input → Additional Dependencies.
   - Add `msmpi.lib`.

5. **Running your MPI Program:**
   - Add your C++ code from P6Q1.
   - Run the executable in Powershell using the command:
     ```bash
     mpiexec -n 4 filename.exe
     ```
     The `-n <np>` flag specifies the number of processes to use.

## 📊 Datasets

- `burma14.tsp`: Ideal for initial tests to ensure your program runs smoothly.
- `d5000.tsp`
- `pcb3038.tsp`

**Note:** Update the dataset pathname in the program before running. For MPI, always use the absolute path, like "C:\\Users\user\...", rather than a relative path.

## 🚀 Team Members

1. 🚀 **[Wilson Yau Kai Chun](https://github.com/wilsonyau02)**
2. 🍌 **[Lee Chun Kai](https://github.com/BananaKing123)**
3. 🦖 **[Lan Ke En](https://github.com/error323dino)** 
