#include <iostream>
#include <WS2tcpip.h>
#include <Windows.h>
#include <mmsystem.h>

#define Message(msg) MessageBoxA(GetConsoleWindow(), msg, "Server Contact - Error", MB_OK)
#define PORT 6868
#define CLIENTS 2
#define SAMPLE_RATE 44100 // Adjust this to match your audio settings
#define BUFFER_SIZE 1024 // Adjust the buffer size as needed
using namespace std;

fd_set master;
MMRESULT result;

short ConnectedUsers() {
    return master.fd_count - 1;
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Initialize winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        Message("Can't initialize winsock! Quitting");
        cout << "Can't initialize winsock! Quitting" << endl << endl;
        return 1;
    }
    // Create a socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET) {
        Message("Can't create a socket! Quitting");
        cout << "Can't create a socket! Quitting" << endl << endl;
        return 1;
    }

    // Bind the socket to an IP address and port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.s_addr = INADDR_ANY; // Use INADDR_ANY to bind to all available network interfaces
    if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        Message("Can't bind socket! Quitting");
        cout << "Can't bind socket! Quitting" << endl << endl;
        closesocket(listening);
        WSACleanup();
        return 1;
    }
    // Tell winsock the socket is for listening
    listen(listening, SOMAXCONN);
    FD_ZERO(&master);
    FD_SET(listening, &master);

    sockaddr_in client;
    int clientSize = sizeof(client);

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "Waiting for incoming connection...\n" << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);

    short audioBuffer[BUFFER_SIZE]; // Audio buffer
    // Initialize audio playback
    WAVEFORMATEX playbackFormat;
    playbackFormat.wFormatTag = WAVE_FORMAT_PCM;
    playbackFormat.nChannels = 2; // Stereo audio
    playbackFormat.nSamplesPerSec = SAMPLE_RATE;
    playbackFormat.wBitsPerSample = 16;
    playbackFormat.nBlockAlign = (playbackFormat.nChannels * playbackFormat.wBitsPerSample) / 8;
    playbackFormat.nAvgBytesPerSec = playbackFormat.nSamplesPerSec * playbackFormat.nBlockAlign;

    HWAVEOUT hWaveOut;
    result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &playbackFormat, 0, 0, CALLBACK_NULL);
    if (result) {
        char fault[256];
        waveInGetErrorTextA(result, fault, 256);
        Message(fault);
        return 1;
    }

    bool running = true;
    while (running) {
        fd_set copy = master;
        // See who's talking to us
        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

        // Loop through all the current connections / potential connections
        for (int i = 0; i < socketCount; i++) {
            // Makes things easy for us doing this assignment
            SOCKET sock = copy.fd_array[i];
            // Is it an inbound communication?
            if (sock == listening) {
                // Accept a new connection
                SOCKET clientSocket = accept(listening, nullptr, nullptr);
                // Add the new connection to the list of connected clients
                FD_SET(clientSocket, &master);
                // Send a welcome message to the connected client
                string welcomeMsg = "Welcome to the Awesome Chat Server!";
                send(clientSocket, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
                cout << "Connected users - " << ConnectedUsers() << endl;
            } else {
                // It's an inbound audio data
                int bytesIn = recv(sock, (char*)audioBuffer, BUFFER_SIZE * sizeof(short), 0);
                if (bytesIn <= 0) {
                    // Drop the client
                    closesocket(sock);
                    FD_CLR(sock, &master);
                    cout << "Connected users - " << ConnectedUsers() << endl;
                } else {
                    // Process and play back the received audio data
                    if (bytesIn > 0) {
                        // Write the audio data to the playback buffer
                        WAVEHDR playbackHdr;
                        ZeroMemory(&playbackHdr, sizeof(WAVEHDR));
                        playbackHdr.lpData = (LPSTR)audioBuffer;
                        playbackHdr.dwBufferLength = bytesIn;
                        playbackHdr.dwFlags = 0L;
                        playbackHdr.dwLoops = 0L;
                        waveOutPrepareHeader(hWaveOut, &playbackHdr, sizeof(WAVEHDR));
                        waveOutWrite(hWaveOut, &playbackHdr, sizeof(WAVEHDR));
                        waveOutUnprepareHeader(hWaveOut, &playbackHdr, sizeof(WAVEHDR));
                    }
                }
            }
        }
    }

    // Close the audio playback
    waveOutClose(hWaveOut);
    FD_CLR(listening, &master);
    // Close the socket
    closesocket(listening);
    // Cleanup winsock
    WSACleanup();
    return 0;
}
