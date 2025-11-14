# Demand_Paging_Lab

A small C++ demand-paging / virtual memory lab that implements a two-level
page table alongside a TLB, clock-based frame replacement, and a simple
MLFQ scheduler. The project reads an access trace and simulates page accesses,
TLB hits/misses, page faults, and frame evictions while logging events to
`events.log`.

**Files:**
- `main.cpp`: Simulation driver. Parses a trace file (`input.txt`), drives
	processes/threads, coordinates CPU/TLB/PageTable/Clock/Scheduler, and
	records events to `events.log`.
- `TwoLevelPageTable.h` / `TwoLevelPageTable.cpp`: Two-level page table
	implementation used to map `(pid, page)` pairs to frame numbers.
- `TLB.h` / `TLB.cpp`: Translation lookaside buffer for fast lookups.
- `Clock.h` / `Clock.cpp`: Clock-based frame replacement algorithm and
	frame bookkeeping.
- `Scheduler.h` / `Scheduler.cpp`: MLFQ scheduler and process bookkeeping.
- `Process.h` / `Process.cpp`, `CPU.h` / `CPU.cpp`, `Thread` helpers:
	Supporting classes used in the simulation.

**Design notes:**
- The page number is derived from an address using an `OFFSET_BITS` shift
	(see `main.cpp`); the `TwoLevelPageTable` splits page numbers into outer
	and inner indices using `INNER_BITS` (10 bits) and an inner mask.
- `TwoLevelPageTable` stores first-level mappings `(pid, outer_page) -> tableID`
	and second-level entries keyed by `(tableID, inner_page)` -> `frame#`.

**Build:**
Simple one-line compile that includes all .cpp sources used by the program:

```bash
g++ main.cpp TwoLevelPageTable.cpp TLB.cpp Clock.cpp Scheduler.cpp Process.cpp CPU.cpp -o main
```

Or compile objects separately and link:
```bash
g++ -c TwoLevelPageTable.cpp -o TwoLevelPageTable.o
g++ -c TLB.cpp -o TLB.o
g++ -c Clock.cpp -o Clock.o
g++ -c Scheduler.cpp -o Scheduler.o
g++ -c Process.cpp -o Process.o
g++ -c CPU.cpp -o CPU.o
g++ -c main.cpp -o main.o
g++ main.o TwoLevelPageTable.o TLB.o Clock.o Scheduler.o Process.o CPU.o -o main

**Run:**
The program expects two command-line arguments: `<tlb_size>` and `<frame_count>`.
It reads a trace file named `input.txt` in the working directory and writes
an event log to `events.log`.

Example:

```bash
./main 16 128
```

**Notes / Troubleshooting:**
- If you see linker errors like "undefined reference to ...", make sure you
	compile/link all `.cpp` files together (see Build section). `main.cpp`
	includes a number of headers and expects their implementations to be
compiled into the final binary.
- Ensure `input.txt` exists and has the expected format: each line should
	contain: `PID TID ADDRESS R|W` (addresses in hex). Lines beginning with
	`#` or empty lines are ignored.

**Next steps / Extensions:**
- Add command-line flags to specify trace file path and logging options.
- Add unit tests for the `TwoLevelPageTable`, `TLB`, and `Clock` modules.
- Add a `Makefile` or `CMakeLists.txt` for easier builds across platforms.

---