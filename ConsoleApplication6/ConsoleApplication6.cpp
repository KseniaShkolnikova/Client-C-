#define WIN32_LEAN_AND_MEAN //макрос

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
using namespace std;

int main()
{
	setlocale(LC_ALL, "RUS");

	WSADATA wasData;// служебная структура для хранения информации (например семейная структура)
	SOCKET ConnectSocket = INVALID_SOCKET; // сокет для подключения


	ADDRINFO hints; // инфо с полями о сокете
	ADDRINFO* addrResult; // адрес хоста
	const char* senBuffer = "Hello from client";
	const char* senBuffer2 = "Это мое 2 сообщение"; // сообщения, который будут отправляться на сервер
	char recvBuffer[512];// для получаемого сообщения

	int result = WSAStartup(MAKEWORD(2, 2), &wasData);// начало использования библиотеки сокетов (определение версии, структуры)
	if (result != 0) {
		cout <<" WSAStartup FAILED result" << endl;
		return 1;
	}

	ZeroMemory(&hints,sizeof(hints));// команда для зануления памяти (1. что 2. сколько)
	hints.ai_family = AF_INET;// присвоение 4Байтного эзернета
	hints.ai_socktype = SOCK_STREAM;// потоковый тип сокета
	hints.ai_protocol = IPPROTO_TCP;// протокол TCP

	// result хранит в себе адрес, порт, семейство структур и адрес сокета
	result = getaddrinfo("localhost", "666", &hints, &addrResult);
	if (result != 0) {
		cout << " getaddrinfo FAILED result" << endl;
		WSACleanup(); // очистка WSAStartup
		return 1;
	}

	ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);// создание нового сокета ( в скобках указаны адрес, тип сокета, протокол)
	if (ConnectSocket == INVALID_SOCKET) {
		cout << " ConnectSocket FAILED result" << endl;
		WSACleanup();
		FreeAddrInfo(addrResult);// освобождение памяти адреса
		return 1;
	}
	 // здесь ппроисходит установка соединения (в скобках указаны дескриптор сокета для подключения,адрес и размер адреса хоста)
	result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		cout << " Connection FAILED result" << endl;
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	// здесь происходит отправка сообщения (в скобказ указаны дескриптор сокета подключения, сообщение, количесвто байтов и флаг (который указывает, что данные должны быть отправлены немедленно))
	result = send(ConnectSocket, senBuffer, (int)strlen(senBuffer),0);
	if (result == SOCKET_ERROR) {
		cout << " send FAILED result" << endl;
		WSACleanup();
		return 1;
	}
	// в случае удачной отправки сообщения выводится количество отправленных байтов
	cout << "sent" << result << " bytes" << endl;

	// тот же блок кода, только для отправки 2 сообщения
	result = send(ConnectSocket, senBuffer2, (int)strlen(senBuffer2), 0);
	if (result == SOCKET_ERROR) {
		cout << " send FAILED result" << endl;
		WSACleanup();
		return 1;
	}
	cout << "sent" << result << " bytes" << endl;

	result = shutdown(ConnectSocket, SD_SEND);// здесь происходит закрытие соединения через сокет и запрет на дальнейшую отправку/прием сообщений
	// в скобках дескриптор сокета подключения и флаг на запрет отправки данных
	if (result == SOCKET_ERROR) {
		cout << " shutdown FAILED result" << endl;
		FreeAddrInfo(addrResult);
		WSACleanup();
		return 1;
	}
	// вывод сообщения
	do {
		ZeroMemory(recvBuffer, 512);// зануление буфера для каждого нового приема
		result = recv(ConnectSocket, recvBuffer, 512, 0);// получение данных с сокета ( в скобках дескриптор сокета подключения, буфер, в 4котором будут храниться полученные данные, размер буфера и флаг)
		if (result > 0) {
			cout << "Received " << result << "bytes" << endl;// вывод кол-ва байтов
			cout << "Received data " << recvBuffer << endl;// вывод сообщения

		}
		else if (result == 0)
			cout << "Connection closed" << endl;

		else
			cout << "recv failed with error" << endl;


	} while (result > 0);
	freeaddrinfo(addrResult);/// освобождение памяти для структуры addrResult
	WSACleanup();//доиницилизируем библиотеку
	return 0;

}

