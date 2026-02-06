# Tank2D – Embedded 2D Game on STM32H750

Personal / hobby embedded project

## Overview
Tank2D is a real-time 2D tank game developed for the STM32H750B-DK board.
The project focuses on embedded graphics, DMA acceleration and real-time game logic.

## Features
- Real-time 2D gameplay
- Multiple tanks and bullets
- Sprite rotation (nearest & bilinear)
- Collision detection
- UART-based input
- LCD framebuffer rendering using DMA2D

## Hardware
- STM32H750B-DK
- 480x272 RGB LCD

## Software
- STM32CubeIDE
- STM32 HAL
- Embedded C

## 🎮 Controls
 -Input is received over **UART** and mapped into a compact **bit-field structure**.

- **Player 1:** movement + fire  movement(w a s d), fire(r)
- **Player 2:** movement + fire  movement('up', 'left', 'down', 'right')special keys, fire(num. 1)

## UART Control Script (PC → Board)
 -To control the game, a **Python UART script** must be running on the host PC.  
 -The script reads keyboard input and sends a compact bit-field over UART to the STM32.

## Requirements
 -Python 3  
 -pyserial library  
 -keyboard library  

## Configuration
 -In the script (uart4.py), set the correct COM port used by your system:
 -ser = serial.Serial('COM9', 115200)
 -Replace COM9 with the COM port assigned to the STM32H750B-DK on your PC.

## Usage
 -Connect the STM32H750B-DK to the PC via USB (ST-Link)
 -Flash and run the firmware on the board
 -Run the Python script

## Notes
This project was created as a hobby / portfolio project with focus on
low-level graphics and hardware interaction.
