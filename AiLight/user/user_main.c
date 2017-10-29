/*
 *  Copyright 2016-2017 HomeACcessoryKid - HacK - homeaccessorykid@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*****************************************************************************************
 * Welcome to the HomeACcessoryKid hkc AiLight demo
 * With a few lines of code we demonstrate the easy setup of your ESP8266 as an accessory.
 * Start defining your accessory in hkc_user_init and execute other pending init tasks.
 * For each Service characteristic a callback function is defined.
 * An ACC_callback will be called in different modes.
 * - mode=0: initialize your service (init)
 * - mode=1: a request for a change  is received on which you could act (write)
 * - mode=2: a request for a refresh is received where you might update  (read)
 * A callback should return QUICKLY, else use a Task as demonstrated below.
 *
 * If something changes from inside, you can use change_value and send_events in return.
 * You use aid and iid to know which characteristic to handle and cJSON for the value.
 *
 * Use iOS10+ Home app or Eve or other app to test all the features and enjoy
*****************************************************************************************/
 
#include "esp_common.h"
#include "mjpwm.h"
#include "hkc.h"
#include "gpio.h"
#include "queue.h"
#include "math.h"

xQueueHandle identifyQueue;

#define PIN_DI 				13
#define PIN_DCKI 			15

extern acc_item    acc_items[];

struct { //keep track of aid and 4 iids
    int aid;
    int on;
    int bri;
    int hue;
    int sat;
} mylight;

//http://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white
void hsi2rgbw(float H, float S, float I, int* rgbw) {
  int r, g, b, w;
  float cos_h, cos_1047_h;
  //H = fmod(H,360); // cycle H around to 0-360 degrees
  H = 3.14159*H/(float)180; // Convert to radians.
  S /=(float)100; I/=(float)100; //from percentage to ratio
  S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
  I = I>0?(I<1?I:1):0;
  
  if(H < 2.09439) {
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    r = S*4095*I/3*(1+cos_h/cos_1047_h);
    g = S*4095*I/3*(1+(1-cos_h/cos_1047_h));
    b = 0;
    w = 4095*(1-S)*I;
  } else if(H < 4.188787) {
    H = H - 2.09439;
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    g = S*4095*I/3*(1+cos_h/cos_1047_h);
    b = S*4095*I/3*(1+(1-cos_h/cos_1047_h));
    r = 0;
    w = 4095*(1-S)*I;
  } else {
    H = H - 4.188787;
    cos_h = cos(H);
    cos_1047_h = cos(1.047196667-H);
    b = S*4095*I/3*(1+cos_h/cos_1047_h);
    r = S*4095*I/3*(1+(1-cos_h/cos_1047_h));
    g = 0;
    w = 4095*(1-S)*I;
  }

  rgbw[0]=r;
  rgbw[1]=g;
  rgbw[2]=b;
  rgbw[3]=w;
}

void lightSET(void)
{
    int on;
    float hue,sat,bri;
    int rgbw[4];
    char *out;
    on=cJSON_GetObjectItem(acc_items[mylight.on].json,"value")->type;
    if (on) {
        bri=cJSON_GetObjectItem(acc_items[mylight.bri].json,"value")->valueint;
        hue=cJSON_GetObjectItem(acc_items[mylight.hue].json,"value")->valueint;
        sat=cJSON_GetObjectItem(acc_items[mylight.sat].json,"value")->valueint;
        os_printf("h=%d,s=%d,b=%d\n",(int)hue,(int)sat,(int)bri);
        
        hsi2rgbw(hue,sat,bri,rgbw);
        os_printf("r=%d,g=%d,b=%d,w=%d\n",rgbw[0],rgbw[1],rgbw[2],rgbw[3]);
        
        mjpwm_send_duty(rgbw[0],rgbw[1],rgbw[2],rgbw[3]);
    } else {
        mjpwm_send_duty(     0,      0,      0,      0 );
    }
}

void lightO(int aid, int iid, cJSON *value, int mode)
{
    switch (mode) {
        case 1: { //changed by gui
            char *out; out=cJSON_Print(value);  os_printf("on %s\n",out);  free(out);  // Print to text, print it, release the string.
            if (value) lightSET();
        }break;
        case 0: { //init
            mylight.aid=aid; mylight.on=iid;
        }break;
        case 2: { //update
        }break;
        default: {            //print an error?
        }break;
    }
}

void lightH(int aid, int iid, cJSON *value, int mode)  //Hue
{
    switch (mode) {
        case 1: { //changed by gui
            char *out; out=cJSON_Print(value);  os_printf("hue %s\n",out);  free(out);  // Print to text, print it, release the string.
            if (value) lightSET();
        }break;
        case 0: { //init
            mylight.hue=iid;
        }break;
        case 2: { //update
        }break;
        default: {            //print an error?
        }break;
    }
}

void lightS(int aid, int iid, cJSON *value, int mode)  //Saturation
{
    switch (mode) {
        case 1: { //changed by gui
            char *out; out=cJSON_Print(value);  os_printf("sat %s\n",out);  free(out);  // Print to text, print it, release the string.
            if (value) lightSET();
        }break;
        case 0: { //init
            mylight.sat=iid;
        }break;
        case 2: { //update
        }break;
        default: {            //print an error?
        }break;
    }
}

void lightB(int aid, int iid, cJSON *value, int mode)  //Brightness
{
    switch (mode) {
        case 1: { //changed by gui
            char *out; out=cJSON_Print(value);  os_printf("bri %s\n",out);  free(out);  // Print to text, print it, release the string.
            if (value) lightSET();
        }break;
        case 0: { //init
            mylight.bri=iid;
            lightB(aid,iid,value,1);  //send the value to be implemented (since Brightness is the last item)
        }break;
        case 2: { //update
        }break;
        default: {            //print an error?
        }break;
    }
}

void identify_task(void *arg)
{
    int i,original;

    os_printf("identify_task started\n");
    while(1) {
        while(!xQueueReceive(identifyQueue,NULL,10));//wait for a queue item
        for (i=0;i<5;i++) {
            mjpwm_send_duty(4095,    0,    0,    0);
            vTaskDelay(30); //0.3 sec
            mjpwm_send_duty(   0, 4095,    0,    0);
            vTaskDelay(30); //0.3 sec
            mjpwm_send_duty(   0,    0, 4095,    0);
            vTaskDelay(30); //0.3 sec
        }
        lightSET();
    }
}

void identify(int aid, int iid, cJSON *value, int mode)
{
    switch (mode) {
        case 1: { //changed by gui
            xQueueSend(identifyQueue,NULL,0);
        }break;
        case 0: { //init
        	mjpwm_cmd_t init_cmd = {  //This is a mandatory cha so this is where we init the light
	        	.scatter = MJPWM_CMD_SCATTER_APDM,
	        	.frequency = MJPWM_CMD_FREQUENCY_DIVIDE_1,
	        	.bit_width = MJPWM_CMD_BIT_WIDTH_12,
	        	.reaction = MJPWM_CMD_REACTION_FAST,
	        	.one_shot = MJPWM_CMD_ONE_SHOT_DISABLE,
	        	.resv = 0,
	        };
	        mjpwm_init(PIN_DI, PIN_DCKI, 1, init_cmd);
            
            identifyQueue = xQueueCreate( 1, 0 );
            xTaskCreate(identify_task,"identify",256,NULL,2,NULL);
        }break;
        case 2: { //update
        }break;
        default: {            //print an error?
        }break;
    }
}

extern  cJSON       *root;
void    hkc_user_init(char *accname)
{
    //do your init thing beyond the bear minimum
    //avoid doing it in user_init else no heap left for pairing
    cJSON *accs,*sers,*chas,*value;
    int aid=0,iid=0;

    accs=initAccessories();
    
    sers=addAccessory(accs,++aid);
    //service 0 describes the accessory
    chas=addService(      sers,++iid,APPLE,ACCESSORY_INFORMATION_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,accname,NULL);
    addCharacteristic(chas,aid,++iid,APPLE,MANUFACTURER_C,"HacK",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,MODEL_C,"Rev-1",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,SERIAL_NUMBER_C,"1",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,IDENTIFY_C,NULL,identify);
    //service 1
    chas=addService(      sers,++iid,APPLE,LIGHTBULB_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,  "light",  NULL);
    addCharacteristic(chas,aid,++iid,APPLE,ON_C,        "1",lightO);
    addCharacteristic(chas,aid,++iid,APPLE,HUE_C,       "0",lightH);
    addCharacteristic(chas,aid,++iid,APPLE,SATURATION_C,"0",lightS);
    addCharacteristic(chas,aid,++iid,APPLE,BRIGHTNESS_C,"100",lightB); //to support switching on by physical switch

    char *out;
    out=cJSON_Print(root);  os_printf("%s\n",out);  free(out);  // Print to text, print it, release the string.
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{   
    os_printf("start of user_init @ %d\n",system_get_time()/1000);
    
//use this block only once to set your favorite access point or put your own selection routine
/*    wifi_set_opmode(STATION_MODE); 
    struct station_config *sconfig = (struct station_config *)zalloc(sizeof(struct station_config));
    sprintf(sconfig->ssid, ""); //don't forget to set this if you use it
    sprintf(sconfig->password, ""); //don't forget to set this if you use it
    wifi_station_set_config(sconfig);
    free(sconfig);
    wifi_station_connect(); /**/
    
    //try to only do the bare minimum here and do the rest in hkc_user_init
    // if not you could easily run out of stack space during pairing-setup
    //hkc_init("AiLight"); //works with hkc version < 20171029-3
    hkc_init("AiLight",LIGHTBULB_CAT); //the Accessory Category requires hkc version > 20171029-3
    
    os_printf("end of user_init @ %d\n",system_get_time()/1000);
}

/***********************************************************************************
 * FunctionName : user_rf_cal_sector_set forced upon us by espressif since RTOS1.4.2
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal    B : rf init data    C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
***********************************************************************************/
uint32 user_rf_cal_sector_set(void) {
    extern char flashchip;
    SpiFlashChip *flash = (SpiFlashChip*)(&flashchip + 4);
    // We know that sector size is 4096
    //uint32_t sec_num = flash->chip_size / flash->sector_size;
    uint32_t sec_num = flash->chip_size >> 12;
    return sec_num - 5;
}
