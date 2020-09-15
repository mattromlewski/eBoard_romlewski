<a name="introduction"></a>

# E-Board For All

## Introduction

* [Getting Started (Linux)](#getting-started-linux)
* [Building The Transmitter Application](#building-the-transmitter-application)
* [Project Photos](#project-photos)
* [Design Files](#design-files)

This project seeks to enrich the personal electric vehicle building experience, taking it from a common DIY experience to a real engineering excercise. 

This project contains code for a transmitter and receiver used to remote-control an electric skateboard. The necessary electrical and mechanical design documents will also be provided.

This project leverages the FreeRTOS kernel to provide preemptive multitasking, mutexes, and more. It also uses the libopencm3 firmware library for convenient hardware abstraction while working with STM32 microcontrollers. For a detailed guide on how to use these technologies together, see [Beginning STM32 by Warren Gay.](https://www.amazon.ca/Beginning-STM32-Developing-FreeRTOS-libopencm3/dp/1484236238) This book helped make this project possible!

<a name="getting-started-linux"></a>

## Getting Started (Linux)

---

The build environment will run in Docker for repeatability and ease of set-up.<br>
The following scripts will initialize the repository with the necessary build resources and set up Docker on the host.<br><br>
Run the following from the repository root:<br>
```bash
sudo chmod +x scripts/*
./scripts/docker-setup.sh
```

<sub><sup>[back](#introduction)</sub></sup>

<a name="building-the-transmitter-application"></a>

## Building The Transmitter Application

The process for building the transmitter application which runs on the STM32 is autmated and can be started by running:
```bash
./scripts/transmitter-build.sh
```
This will utilize Docker and Docker compsoe to first build the build environemt container (which will take a greater amount of time the first time -- see the Dockerfile for details) and then run the makefile scripts within the container using `docker exec`.

<sub><sup>[back](#introduction)</sub></sup>

<a name="project-photos"></a>

## Project Photos 
### Modified longboard with battery pack (8s1p LiPo with BMS) and control box
![Imgur Image](https://i.imgur.com/Q2lue4Y.png)

### Controller housing
![Imgur Image](https://i.imgur.com/KpHhbp1.png)

### Controller housing showing space for STM32, battery, bluetooth module, charging board, tactile switches, and custom cables
![Imgur Image](https://i.imgur.com/YndguEY.png)

### Transmitter bench setup with STM32F1, HC-05 bluetooth modulem, SPI driven OLED, and ADC driven joystick
![Imgur Image](https://i.imgur.com/HmWyC9H.png)

<sub><sup>[back](#introduction)</sub></sup>

<a name="design-files"></a>

## Design Files
### Mechanical
STL files for 3D printing can be found on GrabCAD here: https://grabcad.com/library/eboard-remote-control-housing-1

### Electrical
BOM and schematic coming soon.

<sub><sup>[back](#introduction)</sub></sup>