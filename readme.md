# Cadenza
Open source code for the cadenza project. VERSION 0.8

## What
What is cadenza? Cadenza is a small ESP32 based I2S audio player that allows users to play music through a hosted webserver.

## Why
After applying for a lot of internships and getting rejected from them, I decided to try my hand at a personal project, it was also the perfect excuse to start working on an embedded systems project, something I've been wanting to do for a while now.

## CURRENT VERSION

Cadenza now allows the user to store a default radio! 

Cadenza has also transitioned to a slightly more asynchronous approach. Thus increasing performance.

Refactoring will take a little bit of time as I don't want the project to be an abstraction mess, it'll probably be one or two classes with little to no abstraction to them for simplicity's sake. Do not expect any building patterns, cadenza is a simple passion project and not an award winning competition project.

## NEXT VERSION

- Refactor connection code into its own library/class
- Comment the code

## Functionalities
Below is a list of promised functionalities with their expected version date. 

### VERSION 1.0

- Cadenza hosts a soft Access Point to allow the user to setup Wifi creditentials
- Cadenza allows the user to control the volume using an analoge input
- Cadenza allows the user to change the radio channel by inputting an web link pointing to a live audio broadcast of that radio channel
- Cadenza allows the user to save a radio channel as default
- Cadenza allows the user the ability to reset settings (saved channels) or configs (network settings)
- Code responsible for cadenza's connectivity is fully commented and built in a modular way

### VERSION 2.0

- Cadenza allows the user to switch music inputs (Radio/Youtube)
- Cadenza allows the user to input a Youtube link and plays the audio from said link
- Cadenza allows the user the ability to input multiple youtube links into a queue
- Cadenza automatically manages the queue as songs finish and feedback is provided to the user
- Users can remove songs from the queue
- Youtube integration is fully modular and commented, allowing for other types of audio to later be integrated

### VERSION 3.0

- Cadenza supports an extra audio source type (Spotify)
- Users can add Spotify songs and create spotify Queues
- General bug fixing and code refactoring where necessary
- Cadenza hosts a better codebase
- Cadenza's readme is fully refactored, featuring code snippits and tutorials on the libraries created for cadenza's usage

### VERSION 4.0

- Cadenza allows the user to mix audio source types into the same queue (Youtube and Spotify only)
- Cadenza is transferred onto a PCB that operates on rechargable power source
- Cadenza's web servers are fully refactored for a more pleasing visual experience
- Cadenza gains a OLED based GUI

