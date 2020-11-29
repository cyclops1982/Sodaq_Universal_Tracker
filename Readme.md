# SODAQ - Universal Tracker

Note: to be able to compile this application you need to add the right board file to your Arduino IDE.

Go to File, Preferences and set the following URL for the additional board files:

http://downloads.sodaq.net/package_sodaq_samd_index.json

You should install (or update to) version 1.6.19 or higher.

## Configuration Menu

After compiling the source code and uploading it to the board you will be able to configure the board through a menu.

Just open the Arduino Serial Monitor (at 115200 baud, with line endings set to NL/CR) and you will get this menu:

[In the case of SARA AFF or SFF]

```Arduino
** SODAQ - Universal Tracker - 1.0.3 **

 -> CPU reset by Software [64]


Commands:
  Show IMEI                                  (SI):
  Show CCID                                  (SC):
  Show Module version                        (SV):
  Commit Settings                            (CS):

Settings:

GPS
  Fix Interval (min)         (fi=): 15
  Alt. Fix Interval (min)    (afi=): 0
  Alt. Fix From (HH)         (affh=): 0
  Alt. Fix From (MM)         (affm=): 0
  Alt. Fix To (HH)           (afth=): 0
  Alt. Fix To (MM)           (aftm=): 0
  GPS Fix Timeout (sec)      (gft=): 120
  Minimum sat count          (sat=): 4
  Num Coords to Upload       (num=): 1

On-the-move Functionality
  Acceleration% (100% = 8g)  (acc=): 0
  Acceleration Duration      (acd=): 0
  Fix Interval (min)         (acf=): 1
  Timeout (min)              (act=): 10

Cellular
  Network Type (N2xx NB-IoT = 2, R4xx NB-IoT = 3, R4xx LTE-M = 4, R412 2G = 5, 2G/3G = 6)  (ntype=): 0
  All Things Talk Token      (att=):
  APN                        (apn=): nb.inetd.gdsp
  Force Operator             (opr=): 20404
  CID                        (cid=): 1
  MNO Profile                (mno=): 1
  APN user                   (apnu=):
  APN password               (apnp=):
  NB-IoT Band                (bnd=): 20
  Target IP                  (ip=): 0.0.0.0
  Target port                (prt=): 1
  Response Timeout           (rxto=): 15

Misc
  Status LED (OFF=0 / ON=1)  (led=): 0
  Debug (OFF=0 / ON=1)       (dbg=): 0
Enter command:
```

[In the case of SODAQ ONE]

```Arduino
Commands:
  Reset DevAddr / DevEUI to the Hardware EUI (EUI): 
  Reset LoRa                                 (RESET): 
  Commit Settings                            (CS): 

Settings:

GPS                               
  Fix Interval (min)                 (fi=): 3
  Alt. Fix Interval (min)            (afi=): 0
  Alt. Fix From (HH)                 (affh=): 0
  Alt. Fix From (MM)                 (affm=): 0
  Alt. Fix To (HH)                   (afth=): 0
  Alt. Fix To (MM)                   (aftm=): 0
  GPS Fix Timeout (sec)              (gft=): 10
  GPS Postition Accuracy (m)         (gpa=): 0
  GPS Dynamic Model                  (gpm=): 0
  Minimum sat count                  (sat=): 3
  Num Coords to Upload               (num=): 1

On-the-move Functionality         
  Acceleration% (100% = 8g)          (acc=): 15
  Acceleration Duration              (acd=): 0
  Fix Interval (min)                 (acf=): 1
  Timeout (min)                      (act=): 1

Other                             
  Minimum time between Fixes (min)   (mfi=): 5

LoRa                              
  OTAA Mode (OFF=0 / ON=1)           (otaa=): 1
  Retry conn. (OFF=0 / ON=1)         (retry=): 1
  ADR (OFF=0 / ON=1)                 (adr=): 0
  ACK (OFF=0 / ON=1)                 (ack=): 0
  Spreading Factor                   (sf=): 12
  Output Index                       (pwr=): 1
  Lora Port                          (lprt=): 1
  DevAddr / DevEUI                   (dev=): 0000000000000000
  AppSKey / AppEUI                   (app=): 00000000000000000000000000000000
  NWSKey / AppKey                    (key=): 00000000000000000000000000000000
  Repeat Count                       (rep=): 0

Misc                              
  Cayenne LPP (OFF=0 / ON=1)         (cay=): 0
  Status LED (OFF=0 / ON=1)          (led=): 0
  Debug (OFF=0 / ON=1)               (dbg=): 1
```

Entering commands is just a matter of typing the command as given in brackets with the right value. For example:

fi=5

Will set the time between the GPS fixes to 5 minutes.
Setting fi=0 will disable the default fix interval.

### Sleep

After the startup the device by default willt be in deep sleep mode. In sleep it uses less than 50 uA. Using the RTC Timers it will wake up at the set intervals.

### Timers and Watchdog

The application is based on the RTCTimer library. At startup the application tries to obtain a GPS fix until timeout. If no fix can be obtained initially the location will be set to 0,0. Once the first fix is obtained the RTC will be set and that fix location will be kept until the next proper fix.

There is a system watchdog running in case the application hangs it will be restarted by the watchdog. (See library Sodaq\_wdt)

### GPS fixes

As seen in the configuration menu we allow for two different schedules for GPS fixes based on UTC time. The default could for instance be that we want a GPS fix every 30 minutes during the night, but during the day we want a fix every 5 minutes. In that case we configure the default to be 30 minutes, but the optional fix timer to be 5 minutes from 06:00 UTC to 18:00 UTC.

The RTC library allows for two of such timers. In case the second Fix interval is set to 0 the second timer is simply ignored.

After a GPS fix is obtained with at least `Minimum sat count` satellites (or the timeout reached) a LoRa packet will be sent. In case of timeout without obtaining enough satellites, the best GPS fix found (if any) is used.

The `GPS Dynamic Model` is used to indicate what movement is expected. These are the options:
```
0: portable
2: stationary
3: pedestrian
4: automotive
5: sea
6: airborne with <1g acceleration
7: airborne with <2g acceleration
8: airborne with <4g acceleration
9: wrist-worn watch (not supported inprotocol versions less than 18)
10: bike (supported in protocol versions 19.2)
```
More details about this can be found in the [ublox documentation](https://www.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_%28UBX-13003221%29.pdf) in section 32.10.19.1.

The `GPS Postition Accuracy` setting allows you to set the accuracy of the fix. The u-blox eva-m8 seems to have this default set to 100m. 0 keeps the default. A value above 3 must be provided as the u-blox eva-m8 cannot be more accurate than this. Use this to get a more accurate fix.

### LoRa Connection

The LoRa communication only starts if the keys are not 0 (0 is the default)
If 'Retry conn.' is on, then in case the connection to the network is not successful (useful for OTAA), the application will retry to connect the next time there is a pending transmission.

For redundancy we could configure a `repeat count`. The value of the repeat count tells us to send the Lora frame an additional number of times (default 0) for redundancy.

The Lora frame contains the following data. The minimum frame size is 21 bytes, the maximum frame size 51 bytes, depending on the number of coordinates we have configured to be sent.


### Temperature

The on-board accelerometer provides temperature delta with 1 degree Celsius resolution. It is not factory calibrated so the offset needs to be set in the application (command "temp" or it can be hardcoded in the code) for each board.

### On-the-move Functionality

The firmware supports, except for the default and alternative fix intervals, a third fix interval that is dependent to movement: if the acceleration on any axis goes over (or below in the case of a negative axis) the acceleration set in Acceleration% parameter for over the set duration, the on-the-move fix interval becomes active until "Timeout" minutes have passed since the last movement detected.

### Minimum time between Fixes
The setting `Minimum time between Fixes` allows you to combine the On-the-move functionality and the GPS fix functionality without getting duplicate messages being send. Getting a GPS fix, and sending out the LoRa packet is quite heavy on the battery consumption, and therefor limiting this can be useful for specific scenario's. This is best explained with an example.

Let's say you have a GPS Fix interval of 30 minutes. This would send an update *every* 30 minutes. Let's say you also set the on-the-move functionality with a Fix Interval of 15 minutes. Let's say the device is always moving, this means the device would send an update *every* 15 minutes.
With these settings, it would send out an update around 6 times over the course of an hour. Every 15 minutes because of movement, and every 30 minutes because of the GPS fix.

If you set `Minimum time between Fixes` to 15 (minutes), then you would only have received 4 updates.

Internally, this simply works by storing the date/time when the last GPS fix was *initiated* and comparing this to the current date/time minus the `Minimum time between Fixes` setting. If the calculated date/time is later than the last GPS fix, then the update will be send. Otherwise the GPS fix is aborted.

This can have a slightly negative effect when Fix Intervals are set to a somewhat longer time.
As an example, consider the GPS Fix Interval of 4 hours, and a One-the-move fix interval of 30 minutes. Let's say the device moves for 1.5 hours. So the One-the-move interval performs 3 GPS fixes. Just after the last on-the-move fix, the GPS Fix Interval is triggered. It will compare the last fix and realize that no GPS fix is needed because it has just send out a on-the-move fix. The device does not move anymore. This would mean that it will take another 4 hours before another fix is send out. The time in between receiving a packet is than more than 4 hours.

### Message Frame content

Note: all values are little endian.

| Description | Length |
| --- | --- |
| Epoch Timestamp | uint32 (4) |
| Battery voltage (between 3 and 4.5 V) | uint8 (1) |
| Board Temperature (degrees celcius) | int8 (1) |
| Lat | int32 (4) |
| Long | int32 (4) |
| Altitude (MSL in meters) | int16 (2) |
| Speed (SOG \* 100 km/h) | uint16 (2) |
| Course (COG) | uint8 (1) |
| Number of satellites | uint8 (1) |
| Time to fix (seconds, FF = no fix, in that case the above position is the last known) | uint8 (1) |
| Plus 0 - 3 of the following 10 bytes: |   |
| Previous fix (seconds ago, FFFF means longer than) | uint16 (2) |
| Lat | long (4) |
| Long | long(4) |

### Remote (over the air) re-configuration

You can send the following configuration parameters back to the device (as part as the LoRaWAN class A communication protocol)

| Description | Length |
| --- | --- |
| Fix Interval  default | uint16 (2) |
| Fix Interval alternate | uint16 (2) |
| From (EPOCH) | long (4) |
| To (EPOCH) | long (4) |
| GPS fix timeout in seconds | uint16 (2) |

## License

Copyright (c) 2018, SODAQ
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
