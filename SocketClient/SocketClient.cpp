#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    WSADATA wsaData; //инфу хранити о сокете
    ADDRINFO hints; 
    ADDRINFO* addrResult; //указатель на структуру, где результаты
    SOCKET ConnectSocket = INVALID_SOCKET;
    char recvBuffer[512];//для приема данных

    const char* sendBuffer1 = "Hello from client 1";//соббщ для сервера
    const char* sendBuffer2 = "Hello from client 2";

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);//вса - виндовск соект апи. с помощью нее инициализируем библиотеки
    //макеворд использование Winsock (2.2 - версия) для работы с сокетами
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); //обнуление
    hints.ai_family = AF_INET;//семейство
    hints.ai_socktype = SOCK_STREAM;//тип сокета
    hints.ai_protocol = IPPROTO_TCP;//протокол

    getaddrinfo("localhost", "666", &hints, &addrResult); //получение инфы об адресе для соединения с сервером
    if (result != 0) {
        cout << "getaddrinfo failed with error" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype,
        addrResult->ai_protocol); //создание сокета для соединения
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation with" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    result = connect(ConnectSocket, addrResult->ai_addr,
        (int)addrResult->ai_addrlen);//установка соединения с сервером
    if (result == SOCKET_ERROR) {
        cout << "Unable connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);//освобождение памяти для резултаттов оплучения адреса
        WSACleanup();
        return 1;
    }

    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    //отправляет данные на сервер
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error" << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Send:" << result << "bytes" << endl;

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error" << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Send:" << result << "bytes" << endl;

    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "shutdown failed, error" << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    do {
        ZeroMemory(recvBuffer, 512);//очистка буфера
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);

    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}