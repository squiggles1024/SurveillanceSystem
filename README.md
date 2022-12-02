# SurveillanceSystem

This project is a MQTT Based 'surveillance system'. It uses the B-U585I-IOT02A STM32 Discovery board which comes equipped with several sensors and a 30 pin ZIF connector used to interface with a camera (OV5640). The camera is part of a COTs module B-CAMS-OMV, which is also from ST Micro. It comes with an EMW3080B chip programmed using ST's X-WIFI-EMW3080B firmware package. The EMW3080 forms the 802.11 network interface.  

This project is "Part 1" of a larger project I am working on: A Quadcopter/Drone. It establishes most of the infrastructure necessary to write the application code to control a drone (i.e. reading the gyro, accelerometer, pressure sensor, magnetometer, and receive/send data over Wi-Fi). The transmitted data can be viewed using the python script located in the git Repo as seen below.
![PythonScript](images/DesktopInterface.PNG)
