# Ross' Can Server

The Can Server reads can bus data from a Tesla Model 3. 

It sends data via ESP32-NOW WiFi between two ESP-32 micro controllers from Espressif. The slave esp32 outputs data sent from the server for real time data such as Battery Voltage, Current, and Power. 

Additionally, for the other abundant messages on the Can Bus the Server serves as an access point for a Web Page where data such as Temperatures, and remaining battery life can be observed. Any mobile device can connect to the high speed wifi connection served by the Can Server.

#### Check out the official Can Server [GitHub](https://github.com/joshwardell) from Josh Wardell where you can find the  .dbc files and where to buy the Can Server and MicroDisplays offered.

## Parts used
[Can Server by Josh Wardell](http://www.jwardell.com/canserver/)

[Tesla Diag to OBD Connector](https://www.gpstrackingamerica.com/shop/hrn-ct20t11/)

[2 x ESP32 Micro Controllers](https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z/ref=sxts_sxwds-bia-wc-p13n1_0?cv_ct_cx=ESP32&dchild=1&keywords=ESP32&pd_rd_i=B0718T232Z&pd_rd_r=22ff69ed-a6ca-4364-b621-4202f3d1dbe1&pd_rd_w=uiGLo&pd_rd_wg=JaNsh&pf_rd_p=e7ea7987-56a0-4822-adda-f67db5e22b16&pf_rd_r=D7BF18B8Q7RE05KHQ097&psc=1&qid=1599963637&sr=1-1-791c2399-d602-4248-afbb-8a79de2d236f#customerReviews)

[5 x QuadAlphanumeric Segmented Displays](https://www.adafruit.com/product/3127)

[5 x LED Backpacks](https://www.adafruit.com/product/3088)

## Demo

// todo