# Arduino Based RFID with Servo Parking Attendant Simulator

[![Open Source Love](https://badges.frapsoft.com/os/v1/open-source-175x29.png?v=103)](https://github.com/ellerbrock/open-source-badges/)
[![forthebadge](http://forthebadge.com/images/badges/built-with-love.svg)](http://forthebadge.com)
[![forthebadge](http://forthebadge.com/images/badges/contains-cat-gifs.svg)](http://forthebadge.com)
[![forthebadge](http://forthebadge.com/images/badges/60-percent-of-the-time-works-every-time.svg)](http://forthebadge.com)


## A Project for IT 443

- The project requires the library **[MFRC522](https://github.com/miguelbalboa/rfid)** for communicating with the RFID Reader
- Reference code used was from a gist [Sample Code](https://gist.github.com/anonymous/5ec489385bcf1ac6e6efa789321aee81)
- The project reference was from [DIY Smart Lock with Arduino and RFID](https://www.makeuseof.com/tag/diy-smart-lock-arduino-rfid/)

## Project Details
1. The project is inteneded to create a parking attendant simulation that has a maximum capacity of 10.
2. There must be 2 Cards that can be read.
3. 1st Card is for entrance and the 2nd one is for exit.
4. Once the entrance card was read the servo motor will lift up the stick as a sign that the car can enter.
5. The same goes with the exit card.
6. When the capacity reached 10 the the stick will not be lifted until an exit card was read.

> **Afterschool Creatives 2017**