```text
 ╔═══════════════════════════════════════════════════════════════════════════════════╗
 ║                                                                                   ║
 ║    █████╗ ██████╗ ███████╗█╗     ███████╗██████╗ ███╗   ██╗    ██████╗ ████████╗  ║
 ║   ██╔══██╗██╔══██╗██╔════╝█║     ██╔════╝██╔══██╗████╗  ██║    ██╔══██╗╚══██╔══╝  ║
 ║   ███████║██║  ██║█████╗  █║     ███████╗██║  ██║██╔██╗ ██║    ██████╔╝   ██║     ║
 ║   ██╔══██║██║  ██║██╔══╝  █║     ╚════██║██║  ██║██║╚██╗██║    ██╔══██╗   ██║     ║
 ║   ██║  ██║██████╔╝███████╗███████╗███████║██████╔╝██║ ╚████║    ██║  ██║   ██║     ║
 ║   ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝╚══════╝╚═════╝ ╚═╝  ╚═══╝    ╚═╝  ╚═╝   ╚═╝     ║
 ║                                                                                   ║
 ║            PREEMPTIVE AVL-BALANCED REAL-TIME PROCESS SCHEDULER ENGINE             ║
 ╚═══════════════════════════════════════════════════════════════════════════════════╝
```

<div align="center">

![C99](https://img.shields.io/badge/Language-C99-00599C?style=for-the-badge&logo=c&logoColor=white)
![Complexity](https://img.shields.io/badge/Queue_Complexity-O(log_N)-brightgreen?style=for-the-badge)
![Architecture](https://img.shields.io/badge/Architecture-Preemptive_RTOS-orange?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-blue?style=for-the-badge)

</div>

---

## ⚡ Overview

**Adelson-RT** is a high-performance, deterministic Real-Time Operating System (RTOS) process scheduler written in C99. Designed to replace traditional $O(N)$ priority queues, Adelson-RT leverages **self-balancing AVL Binary Search Trees** to guarantee strict $O(\log N)$ bounds for process insertion, priority extraction, and context-switch preemptions.

By combining strict priority ordering, PID tie-breaking, and time-slice quantum management, Adelson-RT provides ultra-low latency process dispatching suited for critical real-time embedded environments.

---

## 🔥 Key Features

* **⚡ $O(\log N)$ Dispatch Guarantee**: AVL-backed task management eliminates linear search overhead during context switches.
* **🎯 Dual-Key Sorting Engine**: Primary sorting on integer priority (`0` = highest priority), with deterministic PID tie-breaking for equal priority tasks.
* **⏱️ Preemptive Time Quantum Slicing**: Configurable CPU time slicing (`TIME_SLICE = 2`) ensures responsive round-robin distribution among peer tasks while allowing higher-priority processes to preempt instantly.
* **🔄 Self-Balancing Tree Engine**: Automatic LL, RR, LR, and RL tree rotations preserve maximum height differential $|BF| \le 1$.
* **🛡️ Clean Memory Management**: Zero-leak Process Control Block (PCB) dynamic memory handling with automated node free operations on process completion.

---

## 🏗️ Architecture & Process Lifecycle

```text
               +-------------------+
               |    HASNTSTARTED   |  <-- Initialized via createProcess()
               +---------+---------+
                         |
                         v  insertProcess()
               +-------------------+
        +----> |    Ready Queue    |  <-- AVL Tree (Priority Sorted)
        |      +---------+---------+
        |                |
        |                | extractMinProcess()
        |                v
  Time Quantum  +-------------------+
    Expired     |    INPROGRESS     |  <-- Running on CPU (Time Quantum = 2)
  (STALLED)     +----+---------+----+
        |            |         |
        +------------+         +-----> Task Finished (Time Remaining <= 0)
                                       |
                                       v
                               +---------------+
                               |   COMPLETED   | <-- Memory Freed
                               +---------------+
```

### 1. System Structures

```c
typedef enum {
    COMPLETED,
    INPROGRESS,
    FAILED,
    STALLED,
    HASNTSTARTED
} process_t;

typedef struct {
    uint32_t processid;       // Unique Process Identifier
    int priority;             // Lower Integer = Higher Priority (0 is Top Priority)
    int execution_time;       // Initial Burst Time
    int time_remaining;      // Remaining Burst Time
    process_t state;          // Current Process State
} process_metadata_t;

typedef struct Node {
    process_metadata_t* pcb;  // Pointer to Process Control Block
    struct Node *left;        // Higher Priority Subtree
    struct Node *right;       // Lower Priority Subtree
    int height;               // Subtree Height for Balancing
} Node;
```

---

## 🌲 AVL Priority Queue Mechanics

The Ready Queue is structured as an AVL tree where **left nodes always hold higher priority** processes.

```text
                        [PID:1001 | Prio: 2 | H:3]
                                   /      \
                                  /        \
                      [PID:1002 | Prio: 0]   [PID:1003 | Prio: 5]
                                  \
                                 [PID:1004 | Prio: 2]
```

* **Extraction (`extractMinProcess`)**: Traverses strictly to the leftmost node in $O(\log N)$ time to fetch the process with highest priority (`priority = 0`), re-balancing the tree upon removal.
* **Insertion (`insertProcess`)**: Places newly spawned or preempted processes back into the tree while performing rotations if balance factors exceed $\pm 1$.

---

## 📊 Complexity & Mathematical Analysis

### 1. Time & Space Complexity Summary

| Operation / Feature | Time Complexity (Best) | Time Complexity (Worst) | Space Complexity | Description / Mechanism |
| :--- | :---: | :---: | :---: | :--- |
| **Process Creation** | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Dynamic PCB allocation on heap |
| **Task Insertion (`insertProcess`)** | $\mathcal{O}(1)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(\log N)$* | Priority & PID BST traversal with height balancing |
| **Priority Dispatch (`extractMinProcess`)** | $\mathcal{O}(1)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(\log N)$* | Leftmost node extraction & post-order rebalance |
| **Tree Rotation (LL, RR, LR, RL)** | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | Pointer re-linking & constant height recalculation |
| **Context Switch Overhead** | $\mathcal{O}(1)$ | $\mathcal{O}(\log N)$ | $\mathcal{O}(\log N)$* | Extract Min + Re-insert preempted task |
| **Overall Ready Queue Storage** | — | — | $\mathcal{O}(N)$ | $N$ active process nodes in memory |

*\*Recursion call-stack depth bounded by tree height $h \le 1.44 \log_2 N$.*

### 2. Theoretical Bound Derivation

The height $h$ of an AVL tree containing $N$ active processes satisfies the Fibonacci recurrence relation:

$$N(h) = N(h-1) + N(h-2) + 1 \implies h \le \frac{\ln(N + 2)}{\ln \phi} \approx 1.4404 \log_2 N$$

Because $h$ is strictly bounded by $1.44 \log_2 N$:
* **Max Traversal Steps**: Never exceeds $1.44 \log_2 N$.
* **Worst-Case Latency Boundedness**: Unlike standard Binary Search Trees which can degrade to $\mathcal{O}(N)$ linked lists under ordered task arrivals, **Adelson-RT guarantees strict logarithmic runtime bounds** for all real-time scheduling operations.

### 3. Data Structure Performance Comparison

```text
  Latency (O)
    ^
  N | . . . . . . . . . . . . . . . . . . . . . . (Unsorted Array / Linked List Queue)
    |
    |                                            
logN| _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ (Adelson-RT AVL Tree)
    +----------------------------------------> Active Tasks (N)
```

| Scheduler Architecture | Ready Queue Data Structure | Task Insertion | Min Extraction | Worst-Case Height $h$ | Real-Time Guarantee |
| :--- | :--- | :---: | :---: | :---: | :---: |
| **Naïve RTOS Queue** | Unsorted Linked List | $\mathcal{O}(1)$ | $\mathcal{O}(N)$ | $N$ | ❌ Poor ($\mathcal{O}(N)$ jitter) |
| **Sorted Array Queue** | Sorted Array | $\mathcal{O}(N)$ | $\mathcal{O}(1)$ | — | ❌ Poor (Memmove cost) |
| **Binary Min-Heap** | Binary Heap | $\mathcal{O}(\log N)$ | $\mathcal{O}(\log N)$ | $\approx 3.32 \log_2 N$ | ⚠️ Moderate (Unbalanced subtrees) |
| **Adelson-RT (AVL)** | **Self-Balancing BST** | $\mathbf{\mathcal{O}(\log N)}$ | $\mathbf{\mathcal{O}(\log N)}$ | $\mathbf{\le 1.44 \log_2 N}$ | **✅ Strict Logarithmic Bound** |

---

## 📁 Repository Layout

```text
RTOScheduler/
├── Include/
│   └── Adelson_RT.h      # Core engine declarations, PCB structures & API definitions
├── src/
│   ├── Adelson_RT.c      # AVL tree rotation, balancing, and memory operations
│   └── scheduler.c       # Main CPU dispatcher loop & process simulator
└── README.md             # System documentation
```

---

## 🚀 Quickstart

### Prerequisites

* Any standard C compiler (`gcc`, `clang`) supporting **C99** or later.

### Building & Running

Compile and launch the kernel simulator using `gcc`:

```bash
# Compile the scheduler binary
gcc -Wall -Wextra -IInclude src/Adelson_RT.c src/scheduler.c -o scheduler

# Execute the dispatcher simulation
./scheduler
```

---

## 🖥️ Execution Output Preview

```text
=== ADELSON-RT KERNEL BOOT SEQUENCE ===

Bootstrapping processes... Beginning Dispatcher Loop.
----------------------------------------------------
[PID: 1002 | Prio: 0 | Time Left:  3] -> DISPATCHED (Running)
[PID: 1002 | Prio: 0 | Time Left:  1] -> PREEMPTED (Re-entering Queue)

[PID: 1002 | Prio: 0 | Time Left:  1] -> DISPATCHED (Running)
[PID: 1002 | Prio: 0 | Time Left:  0] -> TERMINATED (Task Complete)

[PID: 1001 | Prio: 2 | Time Left:  5] -> DISPATCHED (Running)
[PID: 1001 | Prio: 2 | Time Left:  3] -> PREEMPTED (Re-entering Queue)

[PID: 1001 | Prio: 2 | Time Left:  3] -> DISPATCHED (Running)
[PID: 1001 | Prio: 2 | Time Left:  1] -> PREEMPTED (Re-entering Queue)

[PID: 1001 | Prio: 2 | Time Left:  1] -> DISPATCHED (Running)
[PID: 1001 | Prio: 2 | Time Left:  0] -> TERMINATED (Task Complete)

[PID: 1004 | Prio: 2 | Time Left:  4] -> DISPATCHED (Running)
[PID: 1004 | Prio: 2 | Time Left:  2] -> PREEMPTED (Re-entering Queue)

[PID: 1004 | Prio: 2 | Time Left:  2] -> DISPATCHED (Running)
[PID: 1004 | Prio: 2 | Time Left:  0] -> TERMINATED (Task Complete)

[PID: 1003 | Prio: 5 | Time Left:  8] -> DISPATCHED (Running)
[PID: 1003 | Prio: 5 | Time Left:  6] -> PREEMPTED (Re-entering Queue)
...
----------------------------------------------------
SYSTEM HALTED. Total Clock Ticks: 20. Ready Queue Empty.
```

---

## 🛠️ API Reference

| Function Signature | Description | Time Complexity |
| :--- | :--- | :---: |
| `createProcess(pid, priority, exec_time)` | Allocates and initializes a new PCB structure. | $O(1)$ |
| `insertProcess(root, process)` | Inserts a process into the AVL tree and rebalances. | $O(\log N)$ |
| `extractMinProcess(root, &pcb)` | Removes and returns the highest priority process. | $O(\log N)$ |
| `printQueue(root)` | Performs an in-order traversal of the current Ready Queue. | $O(N)$ |

---

## 📜 License

This project is open-source and released under the **MIT License**.
