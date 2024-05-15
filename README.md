# AudioStream | Real-Time Audio Streaming over TCP/IP

## Overview
A client-server application enabling real-time audio streaming over TCP/IP using C++ and Winsock. Utilizes the Windows Multimedia System for audio capturing and playback.

## Features
- **Real-Time Audio Streaming**: Captures audio from the client's microphone, encodes it, and transmits it to the server, which decodes and plays it back.
- **Reliable Communication**: Uses TCP/IP for reliable data transmission, ensuring audio data integrity and proper sequencing.
- **Low Latency**: Optimized for minimal latency to facilitate real-time audio communication.

## Components
- **Client and Server Applications**: Developed in C++ using Winsock for network communication.
- **Windows Multimedia System**: Used for capturing and playing back audio with high precision.

## Hardware Setup
The hardware setup includes a microphone for audio input on the client side and speakers for audio output on the server side.

## Software Development
The system is programmed in C++ using Winsock for network communication and the Windows Multimedia System for audio handling. The main functionalities include audio capture, encoding, transmission, decoding, and playback.

## Code Explanation
The repository includes the complete source code with comments explaining the functionality of each segment. The code is structured around a main loop that continuously captures and transmits audio data from the client to the server.

### Key Functions
- `initTimer0()`: Configures the timer used for audio capture timing.
- `startTimer0()`: Starts the timer for echo pulse measurement.
- `stopTimer0()`: Stops the timer and calculates the echo pulse duration.
- `delayUS()`: Provides precise microsecond delays.
- `controlAudioStream()`: Manages the audio streaming process, including encoding and transmission.
- `playAudio()`: Decodes and plays the received audio data on the server side.

## Installation
1. Clone the repository to your local machine.
2. Open the project in your preferred C++ development environment.
3. Connect your microphone and speakers to the client and server systems, respectively.
4. Compile the code and run the client and server applications.

Feel free to reach out if you have any questions or suggestions!

- **LinkedIn**: [linkedin.com/in/avij07](https://www.linkedin.com/in/avij07)
- **Email**: avijaiswaljsr@outlook.com
