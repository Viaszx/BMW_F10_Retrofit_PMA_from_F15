# BMW F10 with 5DL(Surround View) PMA retrofit from F15
***
This simple example shows how you can use an STM32 microcontroller version [105/107][stm32f10x-url] to communicate between two [CAN buses][CAN-BUS].

The [BMW F10][F10] still has the **K-CAN** body bus which operates at **100 kBit/s** and **K-CAN2** which operates at **500 kBit/.**
While the [BMW F15][F15] has had the K-CAN body bus removed.
In order for the PMA module from F15 to work correctly on the F10, it is necessary to receive certain statuses on the K-CAN2 bus via ZGW.
The central gateway module (ZGM) links the main buses to one another. Examples of 'main buses' include FlexRay, MOST or K-CAN. 
This enables the Gateway function on the Central Gateway Module to exchange data between the various different bus systems.
**Note:** 
The central gateway module does not always link all the existing main buses to one another
Which main buses are linked by the central gateway module (ZGM) depends on the model year and the vehicle equipment.
![ZGW](https://github.com/Viaszx/Mazda-SkyActiv-EngineCoolantTemp/assets/78595419/127cc739-5102-4085-954a-874a6f9d7869)
**Development Environment:**
- CooCox CoIDE Version: 1.7.7

## Configuration
----------------
### CMSIS Startup Files for STM32F10x Microcontrollers
To use two hardware CAN buses on STM32F107 it is necessary to pay attention to the new file `startup_stm32f10x_cl.c` located in cmsis_boot/startup.
For STM32F103 controller `startup_stm32f10x_md.c` was used.

To set the CAN bus speed it is necessary to consider the clock frequency of the STM32 controller and set the appropriate parameters. 
**CAN_SJW (Synchronization Jump Width):**
Specifies the width of the jump for synchronization.
In this case, the value is set to `CAN_SJW_1tq`, indicating that the synchronization jump is 1 time quantum.
**CAN_BS1 (Bit Segment 1):**
Determines the duration of the first bit segment.
Here, the value is set to `CAN_BS1_3tq`, meaning that the first bit segment consists of 3 time quanta.
**CAN_BS2 (Bit Segment 2):**
Defines the duration of the second bit segment.
In this case, the value is set to `CAN_BS2_4tq`, signifying that the second bit segment comprises 4 time quanta.

_These parameters define the timing characteristics of the bit segment in the CAN bus. Bit Timing configuration is a crucial part of setting up the CAN controller and depends on the characteristics of the CAN network, such as data transmission speed and network length. Properly configuring these parameters ensures accurate synchronization and communication on the CAN network._

The following settings are used in the code in `can.c`:

Bit Timing configuration for CAN1:
```c
CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
CAN_Init(CAN1, &CAN_InitStructure);
```
Bit Timing configuration for CAN2:
```c
CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
CAN_Init(CAN2, &CAN_InitStructure);
```

In `can.h`, the bus speed is configured for K-CAN at 100 kBit:
```c
#define CAN1_SPEED_PRESCALE	45
```
and speed is configured for K-CAN2 at 500 kBit:
```c
#define CAN2_SPEED_PRESCALE 9
```
the values are calculated for a 72 MHz clock.

   [F10]: <https://en.wikipedia.org/wiki/BMW_5_Series_(F10)>
   [F15]: <https://en.wikipedia.org/wiki/BMW_X5_(F15)>
   [CAN-BUS]: <https://www.can-cia.org/can-knowledge/can/classical-can/>
   [stm32-url]: <https://www.st.com/en/microcontrollers-microprocessors/stm32f103c8.html>
   [stm32f10x-url]: <https://www.st.com/en/microcontrollers-microprocessors/stm32f105-107.html>
