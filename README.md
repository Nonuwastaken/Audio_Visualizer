# Audio Visualizer

Compact ESP32-C3 audio visualizer — captures sound via an I2S microphone, displays live levels on an OLED, and drives an RGB LED strip through MOSFET channels for real-time audio-reactive lighting. 

## Overview

The system listens to ambient/line audio through an I2S MEMS microphone, computes amplitude in real time on the ESP32-C3, and maps it to:

- An OLED readout of live audio levels
- RGB LED strip brightness/color, switched via high-power MOSFET modules (one channel per color)

## Hardware

Built around an ESP32-C3, an INMP441 I2S microphone, a 0.96" I2C OLED, and an RGB LED strip driven through XY-MOS MOSFET modules. Full component list and costs are linked below.

### Pin Assignments

- **XY-MOS (R/G/B channels):** GPIO 0 / 1 / 3
- **INMP441 (I2S mic):** SCK = GPIO 4, WS = GPIO 5, SD = GPIO 6
- **SSD1306 OLED (I2C):** SDA = GPIO 21, SCL = GPIO 20

## Tuning Sensitivity

You can adjust how the visualizer reacts by editing these parameters at the top of each sketch:

```cpp
// 1. VOLUME THRESHOLDS
#define NOISE_FLOOR     5000
#define MAX_LOUDNESS    13500

// 2. FADING SPEEDS (0.01 to 1.0)
#define FADE_UP_SPEED   1.0   // Higher = snappier reaction to loud beats
#define FADE_DOWN_SPEED 0.1   // Lower = slower, smoother fade out between beats
```

| Parameter | What it does | Tune it if... |
|---|---|---|
| `NOISE_FLOOR` | Amplitude below this is treated as silence (LEDs off) | LEDs flicker in a quiet room → raise it. Lights don't react to soft sounds → lower it |
| `MAX_LOUDNESS` | Amplitude at which the LEDs hit full brightness | LEDs are always maxed out → raise it. Never get bright enough → lower it |
| `FADE_UP_SPEED` | How fast brightness rises with sound (0.01–1.0) | Want a snappier reaction to beats → raise it |
| `FADE_DOWN_SPEED` | How fast brightness falls after a sound (0.01–1.0) | Want a smoother, slower fade-out → lower it |

The ideal values depend on your microphone, its placement, and the room, so tune them to your setup. A good approach is to start with `NOISE_FLOOR` just above your quiet-room baseline, then adjust `MAX_LOUDNESS` until the lights peak on your loudest sounds.

## Code Variants

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

## PCB & Enclosure

- **PCB:** The custom V2 PCB design files are available in the `Custom_PCB` folder.
- **Enclosure (CAD):** A custom enclosure was designed for the V2 board. The source CAD files are available in the `CAD Model` folder.

**Onshape CAD:**  
[https://cad.onshape.com/documents/6593946ba04cc1c559037129/w/a2d187fac915f00b7de97ec5/e/4e3bb7f6afe98f245e745010](https://cad.onshape.com/documents/6593946ba04cc1c559037129/w/a2d187fac915f00b7de97ec5/e/4e3bb7f6afe98f245e745010?renderMode=0&uiState=6ac3d8755add55faa27b69ed)

> [!IMPORTANT]
> **Manual wiring required!** The PCB does not connect everything for you:
>
> 1. **Wire GND and VIN- of the MOSFET modules by hand** with a wire.
> 2. **Screw the LED strip wires into the screw terminals** on the MOSFET modules/PCB according to the circuit diagram given.
>
> Skip these and the LED strip won't light up.

### PCB Mounting Holes

The PCB mounting holes in the CAD model are sized for **M3 heat-set inserts**. If you're using something different, adjust the hole size in the CAD model:

- **Other bolt/insert sizes:** change the hole diameter to match your hardware.
- **Screwing directly into the plastic (no inserts):** set the hole diameter to **2.9-3 mm** (For M3).

## Component List

- V1: https://docs.google.com/spreadsheets/d/1ALqold5_36gFdbRfQ2sbJwHor0Tc50hgfMq46gXfjWg/edit?usp=sharing
- V2: https://docs.google.com/spreadsheets/d/10Zfk7NXO29kVGQfq_vPPTgnpW9qup-5_QyMn6JiCvyc/edit?usp=sharing

## Status

- **V1:** complete and functional, built on perfboard with screw-terminal connections.
- **V2:** complete. The perfboard/screw-terminal wiring has been replaced with a custom PCB, and a custom enclosure has been designed to house it, making the project easier to replicate and more robust.
