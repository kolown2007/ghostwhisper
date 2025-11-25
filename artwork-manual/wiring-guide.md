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

### NOTE: Skip STEPS 1-2 if you have successfully followed firmware-upload.md

**1. Install the required software on your computer:**

   |  | MacOS | Windows |
   | ----- | ----- | ----- |
   | Visual Studio Code (VS Code) | [https://code.visualstudio.com/Download](https://code.visualstudio.com/Download) | [https://code.visualstudio.com/Download](https://code.visualstudio.com/Download) |
   | PlatformIO (installed within VSCode) | [https://platformio.org/platformio-ide](https://platformio.org/platformio-ide) |[https://platformio.org/platformio-ide](https://platformio.org/platformio-ide) |
   | CH340 Driver | https://github.com/WCHSoftGroup/ch34xser\_macos  | [https://www.wch-ic.com/downloads/CH343SER\_ZIP.html](https://www.wch-ic.com/downloads/CH343SER_ZIP.html) |


   Related links:

   How to install VSCode:
   https://www.youtube.com/watch?v=cu_ykIfBprI

   How to install the PlatformIO extension:
   https://youtu.be/5edPOlQQKmo?si=uzvZJf_vuab7KCFq

   How to install CH34x drivers:
   https://learn.adafruit.com/how-to-install-drivers-for-wch-usb-to-serial-chips-ch9102f-ch9102


**2. Upload code to ESP32 microcontroller (Requirements: computer with the installed software mentioned above & data cable):**  
     
   Clone the repository from [https://github.com/kolown2007/ghostwhisper](https://github.com/kolown2007/ghostwhisper) and open it in PlatformIO by clicking the PlatformIO icon on the left panel. Under "Quick Access" open "PIO Home" and click "Open", this will lead you to the PlatformIO homepage. Click "Import Arduino Project" and select **"NodeMCU-32S"** as your board. Click on the directory that contains the **ghostwhisper** folder downloaded from the github link above. Open the **ghostwhisper** folder and click **IMPORT**.
   
   On the second to the left panel of your workspace, under "GHOSTWHISPER", open the **"src"** folder and open **"main.cpp"**.
     
   Connect the ESP32 to your computer using the data cable (USB-C to USB-A) it came with and click the upload button in PlatformIO. Make sure to press the **BOOT** button of the ESP32 while uploading the code, do not release the button until the code has been fully uploaded. Once the upload is done, press the RST (reset) button of the ESP32 to refresh the program on the ESP32. 
   
   Keep the ESP32 plugged into your computer for the next step.

**3. Configure the ESP32 to access the WiFi. You may use your computer or phone for this step.**

   Open your WiFi Settings and connect to the SSID name **ghostwhisper**, this will redirect you to the **WiFiManager** landing page of ghostwhisper.

   Click on the **Configure WiFi** button and enter the WiFi credentials (SSID and Password). Once done this will save the WiFi credentials and connect the ESP32 to the network.

   Once connected to the WiFi, you will hear a sound indicating that the connection was successful. If the sound doesn't play, there might be a problem with the credentials entered–– troubleshoot this by reconfiguring the WiFi Settings.

   Unplug the ESP32 from your computer once the connection is successful.

**4. Start assembling the circuit:**

   Strip the end of the wires of speaker modules or solder wires to the anode and cathode pins of the speaker/s if it did not come with pre-soldered wires.

   ![step 2](images/2.gif)

**5. Connect the speaker wires to the amplifier module using the green terminal blocks on the amplifier module.**

   Unscrew the ground side of the green terminal block which is marked with a negative (-) symbol. Insert both ground wires (black wire) of the speaker into the conductor insertion point then tighten back the screw to secure the connection. Do the same method for the power wires (red wire) and connect them to the power side of the terminal block which is marked with a positive (+) symbol.

   ![step 3](images/3.gif)

**6. Prepare the connections:**

   Connect the female end of the male to female dupont wires to the LRC, BCLK, DIN, GND, and VCC pins of the amplifier module.

   ![step 4a](images/4a.gif) ![step 4b](images/4b.gif)

   Next, connect the female end of the male to female dupont wires to the GPIO25, GPIO26, GPIO27, GND, and 5V pins of the ESP32 microcontroller.

   ![step 4c ](images/4c.gif)

**7. Connect the amplifier module pins to the ESP32 using a 5-pin terminal block:**
     
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

**8. Power the ESP32 using a dedicated 5V Power Supply Unit by connecting the USB-C end of the power supply to the ESP32. Plug the power supply to a wall outlet.**
   ![step 6](images/6.gif)


**Full video instructions (no audio) can be accessed here: https://youtu.be/pFi20m0UNg8**

-------

## GhostWhisper Assembly Links:

**1. Assemble the Hardware**: *you are here*

**2. Install the Firmware**: [firmware installation](/artwork-manual/firmware-upload.md)

**3. Connect the ESP32 to WiFi**: [connecting to wifi](/artwork-manual/wifi-connect.md)
