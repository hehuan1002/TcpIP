// NonBlockingTcpClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Winsock2.h>
#include <string>
#include <iostream>
#pragma comment(lib, "WS2_32.lib")

int main()
{
	char buf[BUFSIZ];
	SOCKET sHost;  // �����������ͨ�ŵ�Socket
	int retVal;      // ���ø���Socket�����ķ���ֵ

	// ��ʼ��Socket��̬��
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed! \n");
		return -1;
	}

	// ����Socket
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)
	{
		printf("socket failed!\n");
		WSACleanup();
		return -1;
	}

	//����SocketΪ������ģʽ
	int iMode = 1;
	retVal = ioctlsocket(sHost, FIONBIO, (u_long FAR*)&iMode);
	if (retVal == SOCKET_ERROR)
	{
		printf("ioctlsocket failed!\n");
		WSACleanup();
		return -1;
	}

	// ���÷�������ַ
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(9990);
	int sServerAddLen = sizeof(servAddr);

	while (true)
	{
		// ���ӷ�����
		retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
		if (retVal == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			// �޷�������ɷ�����Socket�ϵĲ���
			if (err == WSAEWOULDBLOCK || err == WSAEINVAL)
			{
				continue;
			}
			else if (err == WSAEISCONN) // �ѽ�������
			{
				break;
			}
			else
			{
				printf("connect failed!\n");
				closesocket(sHost);
				WSACleanup();
				return -1;
			}
		}
	}

	//ѭ��������������ַ���������ʾ������Ϣ
	// ����"quit"��ʹ�����������˳���ͬʱ�ͻ��˳�������Ҳ���˳�
	while (TRUE)
	{
		printf("Please input a string to send: ");
		std::string str;
		// �������������
		std::getline(std::cin, str);
		// ���û���������ݸ��Ƶ�buf��
		ZeroMemory(buf, BUFSIZ);
		strcpy(buf, str.c_str());
		while (true)
		{
			//���������������
			retVal = send(sHost, buf, strlen(buf), 0);
			if (retVal == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK)
				{
					Sleep(500);
					continue;
				}
				else
				{
					printf("send failed !\n");
					closesocket(sHost);
					WSACleanup();
					return -1;
				}
			}
			break;
		}

		while (true)
		{
			ZeroMemory(buf, BUFSIZ);
			//���ܷ������ش�������
			retVal = recv(sHost, buf, sizeof(buf) + 1, 0);
			if (SOCKET_ERROR == retVal)
			{
				int err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK)
				{
					Sleep(100);
					printf("waiting back msg!\n");
					continue;
				}
				else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
				{
					printf("recv failed!\n");
					closesocket(sHost);
					WSACleanup();
					return -1;
				}
			}
			break;
		}
		
		printf("Recv From Server: %s\n", buf);

		//����յ�"quit"�����˳�
		if (strcmp(buf, "quit") == 0)
		{
			printf("quit!\n");
			closesocket(sHost);
			WSACleanup();
			break;
		}
	}
	return 0;
}


