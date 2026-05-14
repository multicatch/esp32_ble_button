# esp32_ble_button
A battery-powered button made with ESP32-C3 (tutorial included).

I use it as emergency pause during movies.

## Why? Again, what do you use it for?

Me and my wife like to watch and comment movies together. We pause the movies very often to just comment on anything, praising the movie or poiting out the things in the background or inconsistencies.
Or we just compare the themes or the movie to the other media. Or just talk about the plot.

That's why we usually need to pause the movie immediately, so we won't have to go back a few seconds to hear a dialogue again or to find the exact frame we wanted to talk about.

Unfortunately our current problems are
* the TV remote gets lost on the couch,
* we misclick (especially me, fat finger, little pause button),
* we double click and some players just interpret both the Play and Pause as Play/Pause, which resumes the movie.

To fix this issues, I've come up with my little invention - **the emergency stop button**.

![Emergency movie stop](./imgs/thebutton.jpeg)

I've bought an emergency stop button (NO/NC) for 4 EUR, used an ESP32-C3 super mini and a 2xAA battery box from some non-functioning light string and *voila*!

## How does it work?

The ESP32's GND, GPIO0 and GPIO1 is wired to the NO (normally open) and NC (normally closed) terminals of the emergency stop button. This way, when GPIO0 or GPIO1 is LOW, I wake up the ESP32 and broadcast a BLE advertisement for 1200ms, and then put the ESP32 back to deep sleep.

The batteries should last for some time (maybe a few years) because I don't maintain a constant Bluetooth/WiFi connection. I just fire and hope the message's delivered.

Every time I make a BLE advertisement, I generate a random number that is used during the broadcast to detect retransmissions (and prevent false presses).

The advertisements are detected by a [Homebridge plugin made by me](https://github.com/multicatch/homebridge-ble-adv) and exposed to Apple Home as wireless buttons.

I also use another [Homebridge plugin made by me to expose the TV buttons in Apple Home](https://github.com/multicatch/homebridge-philipstv-2020-ambilight). So it's just a matter of making an automation that just simulates "Play" and "pause" TV buttons when Apple Home is notified about the button press.

And that's how it works. 

![Configuration is apple home](./imgs/applehome.jpeg)

The latency is about 400-700ms, but the Mac hosting Homebridge is relatively far from the couch. And the Play/Pause commands are issued to the TV via HTTP.

I think it's a pretty good result given the button is easier to reach, so it's much faster to press and we have a bigger time margin for latency. 

And maybe if I used an IR transmitter wired to the ESP32 then the latency would be even lower, but I wanted to make the button more reliable. The IR transmitters have the disadvantage of only working when you point them in the direction of the receiver. The UX of the device would be worse. And with this setup, you can just mash the button no matter where it is or how it's pointed. Much more satistying to use.

## Demo

![Button demo](./imgs/demo.gif)

## Schematics and tutorial

Coming soon... 
