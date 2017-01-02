# ESP8266-HomeKit-Demo
User part of the ESP8266-HomeKit foundation

Public Apple's HomeKit protocol code has been around for some time for more potent processors  
(notably [HAP-NodeJS](https://github.com/KhaosT/HAP-NodeJS)). This is a rewrite for the ESP8266 for you to play with.

For buid instructions please refer to the [wiki](https://github.com/HomeACcessoryKid/ESP8266-HomeKit-Demo/wiki).

# Demo

[![HomeKit Demo](https://img.youtube.com/vi/Xnr-utWDIR8/0.jpg)](https://www.youtube.com/watch?v=Xnr-utWDIR8)

# About the code

The code provides an example of how to create a basic accessory with a dimmable light and an on/off led (GPIO2).
It uses the ESP8266-HomeKit API to create your HomeKit device without descending to the lower levels of the HAP protocol.

## Timings

Here are some preliminary timings. 

### Pairing

Pairing is dominated by the SRP algorithm which is very slow and expensive. Fortunately this only happens once when the iOS device is being associated with the HomeKit device:

- Time1: 25 seconds from boot till start of server, so that initial interaction is split second.

- Time2: 30 seconds (based on a build with DEBUG logging which is slow).

### Verify

Verify happens every time an iOS device reconnected to the HomeKit device. Ideally this should be as fast as possible.

- Time: 1.2 seconds

## Memory

The HomeKit code is approximately 400K and about 18K of RAM is left for other purposes. During Pairing so much RAM is used that it is required to launch most code after pairing is done.

# Thanks

I want to thank a number of projects which made this possible:

1. [HAP-NodeJS](https://github.com/KhaosT/HAP-NodeJS) - which documents the HomeKit protocols for IP and allowed me to guess how they
were implemented.

2. https://github.com/aanon4/HomeKit - which inspired this README and should inspire us to look into assembly.

3. [ESP8266_RTOS_SDK](https://github.com/espressif/ESP8266_RTOS_SDK) - Espressif for their great product

# Notes

Please note that this software was produced without any reference to any proprietary documentation or information. I am not a MFi licensee, nor do I have access to any related information.

Espressif uses MIT license.  

# License

Copyright 2016 HomeACcessoryKid - HacK - homeaccessorykid@gmail.com

Licensed under the Apache License, Version 2.0 (the "License");  
you may not use this file except in compliance with the License.  
You may obtain a copy of the License at  

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software  
distributed under the License is distributed on an "AS IS" BASIS,  
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
See the License for the specific language governing permissions and  
limitations under the License.
