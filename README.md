![Casa](https://raw.githubusercontent.com/cewpur/Casa/master/asset/banner.png)

Casa is a smart home system, written in C and powered by a Raspberry Pi, used to control a model home through a mobile app. The controller implements its own blockchain to record transactions between the client and whether or not they were authorised, using a custom permission system.

The app is built in React Native and driven almost entirely by the home system, using TCP sockets to communicate.

## Demo
![Home](https://raw.githubusercontent.com/cewpur/Casa/master/asset/home.png)
![Home](https://raw.githubusercontent.com/cewpur/Casa/master/asset/home-night.png)
![Terminal](https://raw.githubusercontent.com/cewpur/Casa/master/asset/terminal.png)

## About
This is the development part of an Hons project aiming to explore IoT and possibly combine the concept of blockchain. Due to the small scale, it wasn't possible to implement distribution among appliances for true competitive mining, but the logic for verifying a received block, serialising the chain into JSON and formulating the chain is all there. It records the details of each transaction and consults a separate permission/policy header in the system on whether to process it or not. Blocks are of fixed size but can be sealed prematurely, for example on a permission amendment.

The home can be equipped with sensors capable of responding to change from a client. For example, a daylight sensor (LDR) will inform the client of a change in daylight and the client will switch the colour scheme to a dark/light mode. Also in this change, exterior lights will come on in the dark and all lights will switch off when it is sufficiently bright.

Devices are identified through the DeviceCheck API (see [here](https://github.com/cewpur/react-native-device-check)) and this can be combined with S2S APIs to verify the request originated from the device, though this project was kept local. As there is only one device for testing, profiles are customizable on the app's configure screen and these titles are used instead to identify the user profile on the system.

If you'd just like to try out the app, you can tap 'Run as Demo' on the configure screen and it will simulate a connection to a home system. It currently functions only on iOS, the only natively bridged parts include the DeviceCheck API and the async socket lib (<5% of the app's code base).

Disclaimer: this is by no means a concrete implementation!

## License
GNU GPL v3

The following dependencies are included in this project:
* [facebook/react-native](https://github.com/facebook/react-native)
* [react-native-community/react-native-svg](https://github.com/react-native-community/react-native-svg)
* [react-native-community/react-native-linear-gradient](https://github.com/react-native-community/react-native-linear-gradient)
* [robbiehanson/CocoaAsyncSocket](https://github.com/robbiehanson/CocoaAsyncSocket)
* [WiringPi/WiringPi](https://github.com/WiringPi/WiringPi)
* [DaveGamble/cJSON](https://github.com/DaveGamble/cJSON)
* [ilvn/SHA256](https://github.com/ilvn/SHA256)
* [troydhanson/uthash](https://github.com/troydhanson/uthash)
