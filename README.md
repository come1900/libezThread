# libezThread - Thread Management Library

## Description

libezThread is a comprehensive C++ thread management library that provides high-level abstractions for multi-threaded programming. The library includes:

- **Thread Management**: Create, manage, and control threads with message queues
- **Synchronization Primitives**: Mutexes, semaphores, locks, and condition variables
- **Message Queues**: Inter-thread communication via message queues
- **Timers**: Thread-safe timer functionality
- **Memory Pool**: Efficient memory allocation for thread operations

All implementations support cross-platform development (Linux/Windows) and provide a clean C++ API for building robust multi-threaded applications.

## Quick Start

### Requirements

- **Linux environment** with GCC compiler (C++11 support)
- **pthread** library (standard system library)
- **Make** build tool

### Installation

```bash
cd libezThread
make
make install
```

This will generate the static library `libezThread-$(PLATFORM).a` and install it to:
- Library files: `$(HOME)/libs/lib/`
- Header files: `$(HOME)/libs/include/ezThread/`

### Compiling Tutorial Examples

The library includes tutorial examples in the `tutorial_libezthread` directory:

```bash
cd tutorial_libezthread
make
```

**Note:** Compilation requires libezThread to be installed first (run `make install` in the parent directory).

## Usage

### Basic Thread Creation

```cpp
#include "ezThread/EZThread.h"

class MyThread : public CEZThread {
public:
    MyThread() : CEZThread("MyThread", 0, 100) {}
    
protected:
    virtual void OnThreadMessage(EZTHREAD_PARAM wParam, EZTHREAD_PARAM lParam) {
        // Handle thread messages
    }
};

// Create and start thread
MyThread thread;
thread.CreateThread();
```

### Using Mutexes and Locks

```cpp
#include "ezThread/EZMutex.h"
#include "ezThread/EZLock.h"

CEZMutex mutex;
{
    CEZLock lock(mutex);  // Automatic lock/unlock
    // Critical section
}
```

### Using Semaphores

```cpp
#include "ezThread/EZSemaphore.h"

CEZSemaphore sem(0);  // Initial count = 0
sem.Wait();           // Wait for signal
sem.Post();           // Signal
```

### Using Message Queues

```cpp
#include "ezThread/EZMsgQue.h"

CEZMsgQue msgQueue(100);  // Queue size = 100
msgQueue.Put(msg);        // Send message
msgQueue.Get(msg);        // Receive message
```

### Using Timers

```cpp
#include "ezThread/EZTimer.h"

CEZTimer timer;
timer.Start(1000);  // Start timer with 1000ms interval
timer.Stop();       // Stop timer
```

### Linking in Your Project

When compiling your project, add the following flags:

```bash
-I$(HOME)/libs/include/ezThread -L$(HOME)/libs/lib -lezThread-$(PLATFORM) -lpthread
```

Example Makefile:

```makefile
CFLAGS += -I$(HOME)/libs/include/ezThread
LIBS += -L$(HOME)/libs/lib -lezThread-$(PLATFORM) -lpthread

myapp: myapp.o
	$(CPP) -o $@ $^ $(LIBS)
```

## Architecture

### Core Components

- **EZThread**: Thread class with message queue support
- **EZMutex**: Mutex synchronization primitive
- **EZLock**: RAII-style lock wrapper
- **EZSemaphore**: Semaphore synchronization primitive
- **EZMsgQue**: Thread-safe message queue
- **EZTimer**: Timer functionality
- **EZPoolAllocator**: Memory pool allocator

### Features

- **Cross-platform**: Supports Linux and Windows
- **Message-driven**: Built-in message queue for thread communication
- **RAII**: Automatic resource management with lock guards
- **Thread-safe**: All synchronization primitives are thread-safe
- **Memory efficient**: Optional memory pool allocator for performance

## Tutorial Examples

The `tutorial_libezthread` directory contains several example programs:

- **Thread**: Basic thread creation and management
- **ThreadMsg**: Thread message passing example
- **SemaphoreMutexLock**: Synchronization primitives demonstration
- **Observer**: Observer pattern implementation
- **MyObserver**: Custom observer example

To compile and run:

```bash
cd tutorial_libezthread
make
./Thread
./ThreadMsg
./SemaphoreMutexLock
```

## License

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
