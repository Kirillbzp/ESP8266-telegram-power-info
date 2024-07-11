# ESP8266-telegram-power-info
This project submits messages into a Telegram chat when power goes out or is back on

## Description and infrastructure requirements
The project is based on ESP8266 board.
WiFi connectivity is required in a moment of status changes to be able to send the messages,
so some kind of UPS is required for your WiFi router. 
ESP8266 also requires backup power to send the message, there are two ways to achieve this:

### Approach 1. Using LiIon battery for ESP8266
This could be done using a regular LiIon 3.7v battery and charging board for example TC4056, please see this
[video](https://www.youtube.com/watch?v=Lk__xTxLlY0) for details. 
To reduce supply power to 3.3V a voltage regulator like MCP1700-3302E could be used, see this [video](https://www.youtube.com/watch?v=Z-36HflHotU)
for details. To power ESP8266, two MCP1700-3302E could be used in parallel. Also you can use DC-DC module based on AMS1117. But in this case out voltage could be less than 3V and ESP8266 could work unstable.
Short circuit of battery outputs never ends well so please be careful when working with the battery!
The single 5v power adapter could be used both to charge the esp8266 battery and to detect the power outage.
So the full list of materials will be:
- ESP8266 board
- TC4056/TP4056/TP4057 charging module with protec![alt text](image.png)tion (there are TP4056 without protection in the market)
- Two MCP1700-3302E voltage regulators or DC-DC module based on AMS1117
- Two resistors: R1 = 1K, R2 = 2K (or other values with the same ratio R2 = 2 * R1 or two R1 connected in series)
- Electrolytic capacitor about ~1000uF (mind the polarity) (in case of MCP1700-3302E used)
- Ceramic capacitor about ~100nF, any will work (in case of MCP1700-3302E used)
- Phone USB charged and MicroUSB/Type-C cable
![diagram](ESP8266-12.png)  

### Approach 2. Using two power supplies in case of ESP8266 with USB input used
It is possible to use two 5v power supplies: one connected directly to detect power loss,
and another one connected to UPS to power esp32. In this case, grounds of both supplies should be connected.

Please note, that the backup power is required only to send the messages (even 1 minute is enough) and is NOT required for the
whole period of power outage.

![schematic](schematic.png)

The voltage divider circuit (two resistors, 2*R1 = R2 ) is used to reduce power probe voltage to 3.3v, please do not apply 5v to an input pin directly.

### ESP8266 without USB
#To use ESP8266 without USB some pines always should be connected with GND or VCC using resistors
1. Connect GND and GPIO15 using 10kOm resistor
2. Connect VCC and CH_PD using 10kOm resistor
3. Connect VCC and REST using 10kOm resistor
#Before firmware ESP8266:
1. Connect VCC and GPIO0 using 10kOm resistor
2. Connect GND and GPIO0 using button (firmware button)
3. Connect VCC and REST using button (reset button)
4. Connect VCC and 3.3v out from USB UART converter
5. Connect GND and GND out from USB UART converter
6. Connect TXD (GPIO0) and RXT out from USB UART converter
7. Connect RXD (GPIO3) and TXD out from USB UART converter
#To firmware ESP8266
1. Connect USB UART converter to the USB out from computer
2. Push and hold Reset button
3. Push and hold Firmware button
4. Release Reset button
5. Release Firmware button
4. Upload firmware from VSCode

![diagram](ESP8266_firmware.png) 

### Telegram settings
In Telegram, you need to create a bot that will submit the messages using @BotFather.
It is also required to add the bot to the chat.

### Bot settings
In Constants.h file all setting available. There are places to set up WiFi, Telegram bot key and other settings.

### Servise messages
/start - Will send information about the bot and available commands
/showstatus - It will show the current status if there is light
Next commands available only for admin (admin chat_id сonfigured in the Constants.h file)
/showalldata - Shows all saved data
/clearallsubscribers - Deletes all subscribers
/removesubscriber {chat_id} - Delete an individual subscriber
/replacesubscribers {Data_Content} - Replaces the entire data in the file
/setthread {chat_id} {thread_id} - Sets the thread_id for the group. In case of that the group is divided into topics and the bot needs to write not in General, but in a separate topic (thread)

### ESP32
This project based on ESP32 [telegram-power-info](https://github.com/anha1/telegram-power-info)
