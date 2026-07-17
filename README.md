# FSMCore - Generic Real-Time Finite State Machine Engine

A highly modular, generic Finite State Machine (FSM) engine written in C. Designed with a strict separation of concerns, the core engine processes generic states, queues, and timers independently of any specific logic. 

Currently, the engine features a **5G MAC layer HARQ (Hybrid Automatic Repeat reQuest) protocol** as its primary proof-of-concept, but it is structured to easily integrate additional protocols (e.g., RACH, RRC) simply by adding new rule definitions to the `protocols/` directory.

## 🏗️ Project Architecture
* **Core FSM Engine (`src/`):** A protocol-agnostic, multi-tenant state machine engine that handles event queuing, timer management, and state transitions based on injected contexts.
* **Protocol Definitions (`protocols/`):** Contains the specific state topology and transition rules for individual protocols (currently `harq.c`).
* **Real-Time Hub (`main.c`):** Ingests physical layer network events via UDP, routes them to the correct user's FSM, and broadcasts live state changes to a dashboard via WebSockets.
* **Mock Environment (`simulator/`):** Python-based UDP generator that mocks external systems (like the PHY layer) by generating rapid-fire network events.
* **Live Dashboard (`frontend/`):** HTML/JS UI that connects via WebSockets to provide a real-time, color-coded visualization of the state machine.

---

## ⚙️ Prerequisites
1. **C Compiler:** GCC or Clang installed and available in your system PATH.
2. **Python 3.x:** To run the UDP network simulator.
3. **Web Browser:** Chrome, Edge, Safari, or Firefox to view the dashboard.

---

## 🚀 How to Build and Run

You will need to open **two separate terminal windows** inside the `FSMCore` directory to run the full stack.

### Step 1: Compile the C Backend

**For Windows (MinGW):**
*(Requires linking Winsock2 and mapping MinGW compatibility flags for the Mongoose library).*
```bash
gcc main.c src/*.c protocols/*.c -I./include -o fsm_demo.exe -lws2_32 -Dfseeko=fseeko64 -D_CRT_RAND_S -Dalloca=__builtin_alloca -Wno-implicit-function-declaration
```

**For Linux/MacOS:**
```bash
gcc main.c src/*.c protocols/*.c -I./include -o fsm_demo
```

### Step 2: Start the Real-Time Hub

**In Terminal 1, execute the compiled program:**

*For windows:*
```bash
.\fsm_demo.exe
```

*For Linux/MacOS:*
```bash
gcc main.c src/*.c protocols/*.c -I./include -o fsm_demo
```

### Step 3: Start the Python Network Simulator

**Open Terminal 2 (leave Terminal 1 running) and start the Python simulator:**

```bash
python simulator/udp_simulator.py
```

### Step 3: Launch the live Dashboard

**Open index.html file to view the dashboard**


