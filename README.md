# E-Board For All


This project seeks to enrich the personal electric vehicle building experience, taking it from a common DIY experience to a real engineering excercise. 

This project contains code for a transmitter and receiver used to remote-control an electric skateboard.

This project leverages the FreeRTOS kernel to provide preemptive multitasking, mutexes, and more. It also uses the libopencm3 firmware library for convenient hardware abstraction while working with STM32 microcontrollers. For a detailed guide on how to use these technologies together, see [Beginning STM32 by Warren Gay.](https://www.amazon.ca/Beginning-STM32-Developing-FreeRTOS-libopencm3/dp/1484236238) This book helped make this project possible!

## Getting Started (Linux)

---

The build environment will run in Docker for repeatability and ease of set-up.
```bash
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker <your-user>
```

