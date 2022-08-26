# Arduino-TempLogger

**TempLogger** is a C++ program meant to be run on Arduino WeMos D1 R1, but can also be any esp8266 compatible boards. This program does send every x seconds temperature and humidity to a google spreadsheet. Temperature and humidity is obtained with a simple DHT22 sensor.

## 🚊 Usage
> Make sure the DHT22 is plugged like in diagrams
> Power up the board and it will starts to send the data online.

> If the data does not updates, you will need to start the Arduino IDE on your computer and check the Serial logs through USB.

## ⚙️ Circuit
![image](https://user-images.githubusercontent.com/34723487/186924298-2fefd5a1-fc93-4472-86d2-d1d73dbb56c1.png)


## 🖼️ Pictures & Videos

## 👨‍💻 Modifying the program
In order to modify the program you will need to follow all those following steps. If you have any issue during this procedure, please open an issue [here](https://github.com/barbierb/Arduino-TempLogger/issues/)
 1. Install the [Arduino IDE](https://www.arduino.cc/en/software) on your computer
 2. Download all of this project's files by using the **CODE** button or clicking [here](https://github.com/barbierb/Arduino-TempLogger/archive/refs/heads/main.zip)
 3. Unzip the downloaded content on your computer
 4. With the Arduino IDE, open the **main.ino** file
 5. Plug the Arduino UNO on the computer using a USB cable, on the editor check the right port where the Arduino is connected to
 6. Modify the program
 7. Using the **Verify** button on the editor in order to check if you made error or not
 8. Click the Upload button to compile & send the program to the board
 9. The program should now be modified 😁
