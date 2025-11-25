## Wiring guide (I2S)

Connect the I2S amplifier to the ESP32 using the following pins (example mapping):

```text
DOUT -> GPIO25
BCLK -> GPIO27
LRC  -> GPIO26
3.3V / 5V -> VCC (match amplifier voltage)
GND -> GND
```

Adjust pin numbers in code if you use different ESP32 boards or custom wiring.

## Usage / Instructions

### Artwork description
WIP

### Medium
- Speaker units, 
- amplifier, 
- ESP32 microcontroller, 
- WiFi network, 
- and custom software on a remote server.
## List of Components (PER MODULE) 
1 pc	\-	NodeMCU-32s ESP32 Microcontroller  
1 pc	\-	Max98357 Amplifier Module  
1 set	\- 	Speakers  
(OPTIONS: Two units of 4Ω3W full-range speakers or one unit of 8Ω15-20W horn speaker)  
1 pc	\- 	5V Power Supply Unit  
1 set	\- 	Male to Female Dupont Wires 40cm  
1 set \-	Female to Female Dupont Wires 40cm  
2 pcs \-	Terminal block connectors  
1 set \-	Heat-shrink tubing (black)

**Other important tools:**

* Small flathead screwdriver  
* Phillips screwdriver  
* Wire stripper or diagonal cutters  
* USB-C cable for uploading the code to the microcontroller  
* Computer installed with **VS Code** and **PlatformIO IDE**  
* Optional: Soldering tools (soldering iron and solder lead)

## Wiring Diagram (PER MODULE)

![Wiring Diagram](images/ghostwhisper-ThB25.png)

![Pinouts](images/pinouts.png)

## Assembly Instructions

   ![giftest](images/1.gif)


**1. Start assembling the circuit:**

   Strip the end of the wires of speaker modules or solder wires to the anode and cathode pins of the speaker/s if it did not come with pre-soldered wires.

   ![step 2](images/2.gif)

**2. Connect the speaker wires to the amplifier module using the green terminal blocks on the amplifier module.**

   Unscrew the ground side of the green terminal block which is marked with a negative (-) symbol. Insert both ground wires (black wire) of the speaker into the conductor insertion point then tighten back the screw to secure the connection. Do the same method for the power wires (red wire) and connect them to the power side of the terminal block which is marked with a positive (+) symbol.

   ![step 3](images/3.gif)

**3. Prepare the connections:**

   Connect the female end of the male to female dupont wires to the LRC, BCLK, DIN, GND, and VCC pins of the amplifier module.

   ![step 4a](images/4a.gif) ![step 4b](images/4b.gif)

   Next, connect the female end of the male to female dupont wires to the GPIO25, GPIO26, GPIO27, GND, and 5V pins of the ESP32 microcontroller.

   ![step 4c ](images/4c.gif)

**4. Connect the amplifier module pins to the ESP32 using a 5-pin terminal block:**
     
   Remove the protective lid of the terminal block. Unscrew the top and bottom screw using a phillips screwdriver. It is best to start with one column at a time to avoid mismatching the pins. Use the same color for each individual connection.

   Refer to the diagram below for the connections:

   | Max98357 Amplifier | NodeMCU-32s ESP32 microcontroller |
   | :---: | :---: |
   | LRC | GPIO26 |
   | BCLK | GPIO27 |
   | DIN | GPIO25 |
   | GND | GND |
   | VCC | 5V |

   Insert the male end of the dupont cable to the conductor insertion point of the terminal blocks and tighten screws. Attach the protective cover back to the terminal block.

   ![step 5a](images/5a.gif)

   ![step 5b](images/5b.gif)

**5. Power the ESP32 using a dedicated 5V Power Supply Unit by connecting the USB-C end of the power supply to the ESP32. Plug the power supply to a wall outlet.**
   ![step 6](images/6.gif)


**Full video instructions (no audio) can be accessed here: https://youtu.be/pFi20m0UNg8**

-------

## GhostWhisper Assembly Links:

**1. Assemble the Hardware**: *you are here*

**2. Install the Firmware**: [firmware installation](/artwork-manual/firmware-upload.md)

**3. Connect the ESP32 to WiFi**: [connecting to wifi](/artwork-manual/wifi-connect.md)
