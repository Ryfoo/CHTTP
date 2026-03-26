# HTTP Server Prototype (Python)

This directory contains a **minimalist HTTP server** written in Python. It serves as a high-level architectural reference for the core implementation written in raw C.

---

### Purpose
While the C implementation handles the heavy lifting of manual memory management and POSIX socket nuances, this prototype focuses on **readability and logic flow**. 

It is designed to be a "living blueprint" that demonstrates the fundamental HTTP lifecycle without the syntactic noise of low-level pointer arithmetic or buffer overflows.

### Key Concepts
To maintain maximum clarity, this prototype implements the bare essentials of the **Request-Response** cycle:

* **Socket Binding**: Initializing the server to listen on a specific port.
* **Connection Handling**: Accepting a single TCP connection at a time.
* **Request Parsing**: Minimal string splitting to identify the HTTP method and requested path.
* **HTTP Response**: Manually constructing a valid `HTTP/1.1 200 OK` header and body.

### Usage
To run the prototype and verify the logic:
```bash
cd ..
python3 -m HTTP.driver
```