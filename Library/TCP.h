/*
	Copyright(c) 2023 jvde.github@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <fstream>
#include <iostream>
#include <thread>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

#define SOCKET		 int
#define SOCKADDR	 struct sockaddr
#define SOCKET_ERROR -1

#define closesocket close

#endif

#ifdef __ANDROID__
#include <netinet/in.h>
#endif

namespace TCP {
	class Client {

		SOCKET sock = -1;
		int timeout = 2;

		struct addrinfo* address = NULL;

	public:
		bool connect(std::string host, std::string port);
		void disconnect();

		void setTimeout(int t) { timeout = t; }
		int read(void* data, int length, bool wait = false);
		int send(const char* msg, int len) { return ::send(sock, msg, len, 0); }
	};

	// will be combined with above....
	class ClientPersistent {
	public:
		ClientPersistent() {}
		~ClientPersistent() { disconnect(); }

		void disconnect();
		bool connect(std::string host, std::string port);

		int send(const void* data, int length);

	private:
		enum State { DISCONNECTED, CONNECTING, READY };

		std::string host;
		std::string port;

		int timeout = 2; // seconds

		int sock = -1;
		State state = DISCONNECTED;
		time_t stamp = 0;

		struct addrinfo* address = NULL;

		bool isConnected();

		bool reconnect() {
			disconnect();
			if (connect(host, port)) {
				std::cerr << "TCP feed: connected to " << host << ":" << port << std::endl;
				return true;
			}
			return false;
		}
	};


}
