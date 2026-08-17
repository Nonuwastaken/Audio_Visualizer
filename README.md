# Audio Visualizer

Compact ESP32-C3 audio visualizer — captures sound via an I2S microphone, displays live levels on an OLED, and drives an RGB LED strip through MOSFET channels for real-time audio-reactive lighting. Built as a sub-project of the WALL-E robot enclosure.

## Overview

The system listens to ambient/line audio through an I2S MEMS microphone, computes amplitude in real time on the ESP32-C3, and maps it to:
- An OLED readout of live audio levels
- RGB LED strip brightness/color, switched via high-power MOSFET modules (one channel per color)

## Hardware

Built around an **ESP32-C3**, an **INMP441 I2S microphone**, a **0.96" I2C OLED**, and an **RGB LED strip** driven through **XY-MOS MOSFET modules**. Full component list and costs are linked below.

## Pin Assignments

- **XY-MOS (R/G/B channels):** GPIO 0 / 1 / 3
- **INMP441 (I2S mic):** SCK = GPIO 4, WS = GPIO 5, SD = GPIO 6
- **SSD1306 OLED (I2C):** SDA = GPIO 21, SCL = GPIO 20

## Firmware Notes

- OLED init is non-blocking, guarded by an `oledFound` flag so the visualizer still runs if the display isn't detected.
- I2S config explicitly sets `.mck_io_num = I2S_PIN_NO_CHANGE`.
- Audio amplitude is smoothed (fast attack, slow decay) and mapped to LED brightness via configurable `NOISE_FLOOR` / `MAX_LOUDNESS` thresholds.
- The OLED renders a scrolling waveform of the smoothed brightness value.

### Code Variants

Several sketches are included, each driving the RGB strip differently while sharing the same I2S/OLED core:

| Sketch | Behavior |
|---|---|
| `multi_colour_audio.ino` | Audio-reactive brightness with a slow, continuously shifting hue cycle across R/G/B |
| `red_audio.ino` | Audio-reactive brightness, fixed red |
| `green_audio.ino` | Audio-reactive brightness, fixed green |
| `blue_audio.ino` | Audio-reactive brightness, fixed blue |
| `cyan_audio.ino` | Audio-reactive brightness, fixed cyan |
| `purple_audio.ino` | Audio-reactive brightness, fixed purple |
| `yellow_audio.ino` | Audio-reactive brightness, fixed yellow |
| `white_audio.ino` | Audio-reactive brightness, fixed white (all channels) |

## Circuit Diagram
https://app.cirkitdesigner.com/project/735b4f31-fd71-42aa-8f38-7ad56208304c

## Component List
V1 = https://docs.google.com/spreadsheets/d/1ALqold5_36gFdbRfQ2sbJwHor0Tc50hgfMq46gXfjWg/edit?usp=sharing 


V2 = https://docs.google.com/spreadsheets/d/10Zfk7NXO29kVGQfq_vPPTgnpW9qup-5_QyMn6JiCvyc/edit?usp=sharing
## Status

**V1** is complete and functional, built on perfboard with screw-terminal connections.

**Currently in progress: V2** — redesigning the build around a **custom PCB** to replace the perfboard/screw-terminal wiring, making the project easier to replicate and more robust.
