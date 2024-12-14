MQTT protocol follows pub-sub communication model.
Node-red acts as a publisher,the Arduino Uno IDE is both the subscriber and publisher, and the data is controlled by the MQTT broker.
Arduino as a subscriber: 2 lights of 2 different rooms, simulated in PICSIMlab are controlled using switches designed in Node-red dashboard, over the MQTT protocol.
Arduino as publisher: the temperature reading obtained from temperature sensor placed in the room are displayed on the dashboard.
