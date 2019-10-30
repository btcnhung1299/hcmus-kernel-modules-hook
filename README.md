# Linux Kernal Modules and Hook

This repository contains a coursework of Operation System.

### 1. Linux Kernal Modules
- Write a kernal module to generate a random number.
- Create a character device to pass (specifically, read and write) generated number between Linux user-space program and a loadable kernal module (LKM), which is running in Linux kernal space.

### 2. System Call Hooking
- `open` system call: include PID and name of opened file in dmesg
- `write` system call: include PID and name of written file in dmesg
