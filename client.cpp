#include <iostream>
#include <WS2tcpip.h>
#include <Windows.h>
#include <mmsystem.h>

#define PORT 6868
#define SAMPLE_RATE 44100 // Adjust this to match your audio settings
#define BUFFER_SIZE 1024 // Adjust the buffer size as needed

using namespace std;

int main() {
    // Initialize winsock
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        cout << "Can't initialize winsock! Quitting" << endl;
        return 1;
    }

    // Create a socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << "Can't create a socket! Quitting" << endl;
        return 1;
    }

    // Bind the IP address and port to a sockaddr_in structure
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR) {
        cout << "Can't connect to server, err #" << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Initialize audio capture
    HWAVEIN hWaveIn;
    WAVEFORMATEX waveFormat;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 2; // Stereo audio
    waveFormat.nSamplesPerSec = SAMPLE_RATE;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    MMRESULT result = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL);

    if (result) {
        cout << "Failed to open waveform input device." << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    short waveInBuffer[BUFFER_SIZE]; // Audio buffer
    WAVEHDR waveHdr;

    ZeroMemory(&waveHdr, sizeof(WAVEHDR));
    waveHdr.lpData = (LPSTR)waveInBuffer;
    waveHdr.dwBufferLength = BUFFER_SIZE * sizeof(short);
    waveHdr.dwFlags = 0L;
    waveHdr.dwLoops = 0L;
    waveInPrepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInAddBuffer(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    waveInStart(hWaveIn);
    bool running = true;

    while (running) {
        // Capture audio data and send to server
        if (waveInUnprepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING) {
            continue;
        }
        int sendResult = send(sock, (char*)waveInBuffer, BUFFER_SIZE * sizeof(short), 0);
        if (sendResult == SOCKET_ERROR) {
            cout << "Failed to send data to server, err #" << WSAGetLastError() << endl;
            running = false;
            break;
        }

        waveInPrepareHeader(hWaveIn, &waveHdr, sizeof(WAVEHDR));
        waveInAddBuffer(hWaveIn, &waveHdr, sizeof(WAVEHDR));
    }

    // Close the audio capture
    waveInClose(hWaveIn);
    // Close the socket
    closesocket(sock);
    // Cleanup winsock
    WSACleanup();

    return 0;
}
