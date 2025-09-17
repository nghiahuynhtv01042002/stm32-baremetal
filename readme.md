# STM32 BARE METAL REGISTER - LEVEL PROJECT
 STM32 Bare Metal Register-Level Project

This project demonstrates **bare-metal development on STM32F4** at the register level — no HAL, no vendor libraries.  
It’s designed for **maximum control, minimal code size, and IDE independence**.

---

## ✨ Key Features

- **No IDE Required**  
  The project uses **CMake** for building. You can compile directly from VSCode or command line without relying on proprietary IDEs.

- **Automatic File Inclusion**  
  New source files can be added to the project and compiled automatically — no need for manual makefile edits.

- **Direct On-Board Flashing**  
  Code can be flashed to the STM32 board right from VSCode using provided scripts.

- **Register-Level Programming**  
  All peripherals are controlled directly via registers — no HAL or middleware overhead.

- **Optimized for Code Size**  
  Compiler options are configured to minimize code size while maintaining performance.

---

## 📚 Implemented Modules

- **System Clock Configuration** – Configured to run at **96 MHz**.
- **UART Driver (3 Modes)** – Normal, DMA, and Interrupt-based.
- **GPIO** – Input/Output at register level.
- **NVIC** – Nested Vector Interrupt Controller configuration.
- **USB (WIP)** – USB driver under development.

All modules interact with STM32F4 registers directly.

---

## 🛠️ Requirements

- **CMake** (≥3.15 recommended)  
- **ARM GCC Toolchain** (arm-none-eabi-gcc)  
- **Python** (optional, if using some helper scripts)  
- Ability to run `.bat` files (Windows environment)  

**Hardware**:  
- STM32F4 board (tested on STM32F407)

---

## 🚀 Build & Flash Instructions

1. **Clone the Repository**

``` bash
git clone https://github.com/nghiahuynhtv01042002/stm32-baremetal.git
cd stm32-baremetal
```
2. **Build**

run the script . If you prefer to run the commands manually, open the .bat script in a text editor and follow the listed steps.

``` bash
autobuild-script.bat
```

3. **Flash to board **

```bash
flash_all.bat
```

## 👤 Author / Contact

GitHub: [nghia huynh](https://github.com/nghiahuynhtv01042002)

LinkedIn: [nghia huynh](https://www.linkedin.com/in/nghia-huynh-8b77aa2a3/)

Facebook: [nghia huynh](https://www.facebook.com/camxn142)


## 📝 Notes

The USB stack is still under development and not yet functional.

All code is written in C at register-level for maximum control.

Compiler flags are tuned for minimal code size.
## 🗒️ Personal Note / Motivation

I initially thought this project would be straightforward, but in reality it took quite a lot of time to debug and fine-tune everything.  

This project is intended as a **foundation** for my upcoming work on a **secure bootloader**, where my goal is to **optimize code size as much as possible**.  

You can refer to related resources here:
- [Bootloader with HAL](https://github.com/nghiahuynhtv01042002/stm32-cmake-bootloader-template)
- [RSA2048 Implementation](https://github.com/nghiahuynhtv01042002/RSA_2048)