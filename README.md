##                 Exam06 – Mini TCP Server

42 exam rank 06 | 42 Cursus complete

![Exam06 Preview](./exam-06.png)
---

## Exam Objective

The goal of exam06 is to implement a small TCP server capable of:
- accepting multiple clients simultaneously using select(2),
- handling real-time connections/disconnections,
- assembling incoming messages from each client until the `\n` character,
- broadcasting each complete line to all other clients,
- announcing the arrival and departure of clients,
- all in a single C file, with no external dependencies.

---
## Compilation & Run

Terminal 1:
```
cc -Wall -Wextra -Werror mini_serv.c -o mini_serv
./mini_serv 4242
```

The server listens locally on 127.0.0.1:4242.  
Quick test with netcat:

Terminal 2:
```
nc 127.0.0.1 4242 # client A
```
Terminal 3:
```
nc 127.0.0.1 4242 # client B
```

Type a line in A → it shows up in B (and vice versa).

---
## Key Implementation Points
- Use of select() to monitor read/write events on sockets.  
- Each new client receives an incremental ID.  
- When a client sends a complete line, it is broadcast in the format:  
  `client X: <message>`  
- On connection:  
  `server: client X just arrived`  
- On disconnection:  
  `server: client X just left`

---
## Particularities of Grademe Tests ⚠️
The grader’s tests change on each run (timing, number of clients, sequence of events).  
It’s possible that a test fails on the first try.  
If one fails, just move to the next (test 2, test 3, etc.).

⚠️⚠️⚠️ In my case, the code was validated on the 4th test without changing anything. ⚠️⚠️⚠️  

So don’t panic if everything doesn’t pass right away — this is expected.  

---
## Result
The exam is validated.  
The server passes Grademe tests, even if sometimes it requires several runs.  
Robustness and handling of edge cases (split messages, multiple lines in a single recv, sudden disconnections) are essential.

---
## Notes
- The IP address `2130706433` corresponds to `127.0.0.1` (localhost).  
- Buffers are intentionally large to handle message assembly.




