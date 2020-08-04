from bitnami/minideb:jessie 
RUN apt-get update && apt-get install -y wget tar bzip2 build-essential git unzip python
# ARM cross compiler
WORKDIR /opt
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
RUN tar -xjf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
RUN rm gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
RUN mv gcc-arm-none-eabi-9-2020-q2-update gcc-arm
ENV PATH "/opt/gcc-arm/bin/:$PATH"
# clone the hardware abstraction libraries, RTOS kernel, and book software
WORKDIR /usr/src/project
RUN git clone https://github.com/ve3wwg/stm32f103c8t6
RUN mv stm32f103c8t6 stm32
WORKDIR /usr/src/project/stm32
RUN git clone https://github.com/libopencm3/libopencm3.git
WORKDIR /usr/src/project/stm32/rtos
RUN wget https://github.com/FreeRTOS/FreeRTOS/releases/download/V10.3.1/FreeRTOSv10.3.1.zip
RUN unzip FreeRTOSv10.3.1.zip
RUN sed -i 's/FreeRTOSv10.0.1/FreeRTOSv10.3.1/g' Project.mk
WORKDIR /usr/src/project/stm32
RUN make -j1
# cmake
WORKDIR /opt
RUN wget https://github.com/Kitware/CMake/releases/download/v3.15.2/cmake-3.15.2.tar.gz
RUN tar -zxvf cmake-3.15.2.tar.gz
WORKDIR /opt/cmake-3.15.2
RUN ./bootstrap
RUN make -j4
RUN make install
# libusb 1.0.23 
RUN apt-get update && apt-get install -y libudev-dev
WORKDIR /opt
RUN wget  https://github.com//libusb/libusb/releases/download/v1.0.23/libusb-1.0.23.tar.bz2
RUN tar -xf libusb-1.0.23.tar.bz2
WORKDIR /opt/libusb-1.0.23
RUN ./configure --prefix=/usr --disable-static && make
RUN make install
# st-link
WORKDIR /opt
RUN git clone https://github.com/texane/stlink.git
WORKDIR /opt/stlink
RUN cmake .
RUN make
ENV PATH "/opt/stlink/bin/:$PATH"