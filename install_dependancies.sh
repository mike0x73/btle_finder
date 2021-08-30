#!/bin/bash
sudo apt install libbtbb-dev bluetooth cmake libusb-1.0-0-dev
sudo git clone https://github.com/greatscottgadgets/ubertooth.git /opt/ubertooth
sudo mkdir /opt/ubertooth/host/build
sudo cmake -S /opt/ubertooth/host -B /opt/ubertooth/host/build
sudo make -C /opt/ubertooth/host/build
sudo make install -C /opt/ubertooth/host/build
