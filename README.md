# SmartCart

## Installation

1. To upload the folder `data/public` in the ESP32, you need to go in Platform IO tools, in the section Platform, then you need to click on "Build Filesystem Image", then when it is done, you will need to click on "Upload Filesystem Image".
2. After that, you will need to connect the cart to your computer and open the `Serial Monitor`.
3. You will need to connect to the ESP32 Access Point, the SSID and Password will be shown in the `Serial Monitor` logs.
4. Then you will need to go at the setup page at `http://192.168.4.1/setup.html`.
5. You will need to enter the information of the Store Wifi SSID and Password, then put the backend information like the ip, port and the security code that you will need to generate from the dashboard.
6. Then when you submit, if the message is successful, you will need to restart the SmartCart and then it will connect to the WiFi and you will see the MAC address for the connection.
