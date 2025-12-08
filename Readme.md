# InTimeTecAssignment (Week 8 – FCFS Scheduler)

This branch implements a simple First-Come, First-Served (FCFS) CPU scheduler with optional I/O events and process kill events.

## Features
- Read process definitions (name, pid, burst time, I/O start, I/O duration)
- Simulate FCFS with I/O blocking and wake-up
- Handle scheduled KILL events to terminate a process early
- Track waiting time, turnaround time, completion, and kill state

## Build
```bash
gcc fcfs.c -o fcfs
```

## Run
```bash
./fcfs
```

### Expected input format
1) Number of processes
2) For each process: `name pid burst_time io_start_time io_duration`
3) Number of KILL events (0 if none)
4) For each KILL event: `pid kill_time`

### Example session
```
Enter number of processes: 2
Enter process details (name pid burst_time io_start_time io_duration):
P1 1 5 0 0
P2 2 3 0 0
Enter number of KILL events (0 if none): 0
```

## Notes
- Time advances in whole seconds (uses `sleep(1)` per tick); adjust or remove sleep for faster runs.
- Turnaround time is captured at completion (or kill time if terminated early).
- Waiting time accumulates while a process is in the ready queue.