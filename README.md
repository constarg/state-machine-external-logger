# Installition
```
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```

```
% git clone https://github.com/EmbeddedCat/state-machine-external-logger.git
% cd state-machine-external-logger
% mkdir build
% cd build
% cmake ../
% make -j4
```

# Requirements
To build this program you must follow the instructions of the raspberry pi pico documentation, see https://github.com/raspberrypi/pico-examples and https://github.com/raspberrypi/pico-sdk. <br>

Also don't forget to create the enviromental variable below <br>
`export PICO_SDK_PATH=/<path_to_pico_sdk_installition>/pico/pico-sdk/`

# About the state machine
The state machine implementation was not mine. I was only told to track and make logs of the states of the given state machine.
