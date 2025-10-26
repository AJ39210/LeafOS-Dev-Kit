# 🌿 LeafOS  
### Lightweight, Experimental, and Open-Source Operating System

---

## 🧠 About LeafOS

LeafOS is a **free, open-source operating system** built entirely from scratch —  
designed to be lightweight, modular, and educational.  

It follows a **monolithic kernel** structure inspired by Linuxi,  
but everything in LeafOS is written to be **minimal and efficient**.  

The goal of LeafOS is to teach the fundamentals of how an OS boots, switches  
into protected mode, handles the kernel, manages I/O devices, and interacts  
with the user through a custom shell.  

If you’re curious how real systems work under the hood, LeafOS is a perfect  
place to explore.

---

## 🧩 Core Features

- **Newest version GRUB bootlaoder**
- **Vga driver and keybaord driver only.maybe an idt driver too.**

---

## 💬 Philosophy

> “Simple. Transparent. Free.”  
> — The LeafOS Development Vision  

LeafOS isn’t meant to compete with Linux, but to **complement** it.  
It’s a hands-on learning project that exposes what’s hidden deep inside modern systems.  

Every single line of code is understandable, modifiable, and extendable —  
from boot to kernel.

---

Original From Romania ! **fuck you if you have a problem whit romania**

## 🛠️ Requirements

Before building, make sure you have these tools installed:

| Tool | Description |
|------|--------------|
| `make` | Build automation tool |
| `nasm` | Assembler for the bootloader and low-level code |
| `gcc` | C compiler for kernel code |
| `ld` | Linker (part of binutils) |
| `binutils` | Provides `ld`, `objcopy`, etc. |
| `qemu-system-x86_64` | Emulator for testing your OS |

On Debian/Ubuntu-based systems, install them using:

```bash
sudo apt install make nasm gcc binutils qemu-system-x86