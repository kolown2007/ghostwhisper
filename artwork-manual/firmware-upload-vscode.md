

note: We recommend to use the [firmware upload instructions](/artwork-manual/firmware-upload.md). It is less technical. This instructions if you want to help contribute in the development of GhostWhisper.

## Install the required software on your computer


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


## Upload code to ESP32 microcontroller (Requirements: computer with the installed software mentioned above & data cable)  
     
   Clone the repository from [https://github.com/kolown2007/ghostwhisper](https://github.com/kolown2007/ghostwhisper) and open it in PlatformIO by clicking the PlatformIO icon on the left panel. Under "Quick Access" open "PIO Home" and click "Open", this will lead you to the PlatformIO homepage. Click "Import Arduino Project" and select **"NodeMCU-32S"** as your board. Click on the directory that contains the **ghostwhisper** folder downloaded from the github link above. Open the **ghostwhisper** folder and click **IMPORT**.
   
   On the second to the left panel of your workspace, under "GHOSTWHISPER", open the **"src"** folder and open **"main.cpp"**.
     
   Connect the ESP32 to your computer using the data cable (USB-C to USB-A) it came with and click the upload button in PlatformIO. Make sure to press the **BOOT** button of the ESP32 while uploading the code, do not release the button until the code has been fully uploaded. Once the upload is done, press the RST (reset) button of the ESP32 to refresh the program on the ESP32. 
   
   Keep the ESP32 plugged into your computer for the next step.