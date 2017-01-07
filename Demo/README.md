# ESP8266-HomeKit-Demo
Demo of the User part of the [ESP8266-HomeKit](https://github.com/HomeACcessoryKid/ESP8266-HomeKit) foundation

Public Apple's HomeKit protocol code has been around for some time for more potent processors
(notably [HAP-NodeJS](https://github.com/KhaosT/HAP-NodeJS)).  
This is a rewrite for the ESP8266 for you to play with.

# Build instructions for ESP8266-HomeKit-Demo

## Prerequisites
- [ESP8266-HomeKit](https://github.com/HomeACcessoryKid/ESP8266-HomeKit)  

## Build Instructions

### Setting your WiFi
If you did not set up your WiFi, do so by uncommenting the first block of code in the user_init routine and filling the SSID and password. After, do not forget to to comment it out again and remove your password. Its bad for flash to write it each time and you would not want to upload your password to GitHub by accident.

### Compiling

- Goto the ESP8266-HomeKit-Demo directory
```bash
ESP8266_RTOS_SDK$ cd ESP8266-HomeKit-Demo
```
- Goto the Demo directory
```bash
ESP8266-HomeKit-Demo$ cd Demo
ESP8266-HomeKit-Demo$ ./gen_misc.sh  
```
Use all the defaults or experiment with alternatives...  
This will create (after several minutes) the files:
* ../../bin/eagle.flash.bin  
* ../../bin/eagle.irom0text.bin

In case you use the smallest flash-chip, and/or if you answer all the questions by default, ESP8266-HomeKit will need more space than originally foreseen in ESP8266_RTOS_SDK 1.5.0 which was to start irom at 0x20000. To address this it is needed to change the `ESP8266_RTOS_SDK/ld/eagle.app.v6.ld` file:

```diff
diff ld/eagle.app.v6.ld.0 ld/eagle.app.v6.ld
29c29,30
<   irom0_0_seg :                       	org = 0x40220000, len = 0x5C000
---
> /*irom0_0_seg :                       	org = 0x40220000, len = 0x5C000 */
>   irom0_0_seg :                       	org = 0x40214000, len = 0x67000
```

for convenience also change the master `ESP8266_RTOS_SDK/Makefile`:
```diff
diff ESP8266/source/ESP8266_RTOS_SDK-master-v1.5.0/Makefile Makefile 
271c271
< 	@echo "eagle.irom0text.bin---->0x20000"
---
> 	@echo "eagle.irom0text.bin---->0x14000"
```
### Flashing:

```bash
../../../esptool/esptool.py --baud 230400 -p /dev/yourUSBid write_flash 0x00000 ../../bin/eagle.flash.bin 0x14000 ../../bin/eagle.irom0text.bin
```

### Usage:

- After boot, if the device is not yet paired, an srp-key is calculated in about 25 seconds
- After that the server starts and mulicastdns starts to advertize
- The default pincode is `031-45-154`
- The code writes clients keys to sector 0x13000
- After you unpair from the original device, the keys will be destroyed and you can pair again.
- if something went wrong, go to the foundation, and change the `signature` string to something else and you can start from scratch

### More Fun:

Copy the Demo directory side by side and change whatever you want.  
Expect others to upload their examples to this repository. And if you like yours, share it with us.

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
