/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2016 Icinga Development Team (https://www.icinga.org/)  *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#include "base/udpsocket.hpp"
#include "base/logger.hpp"
#include "base/utility.hpp"
#include "base/exception.hpp"
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <iostream>

using namespace icinga;


/**
 * Creates a socket and connects to the specified node and service.
 *
 * @param node The node.
 * @param service The service.
 */
void UdpSocket::Connect(const String& node, const String& service)
{
	addrinfo hints;
	addrinfo *result;
	int error;
	const char *func;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	int rc = getaddrinfo(node.CStr(), service.CStr(), &hints, &result);

	if (rc != 0) {
		Log(LogCritical, "UdpSocket")
		    << "getaddrinfo() failed with error code " << rc << ", \"" << gai_strerror(rc) << "\"";

		BOOST_THROW_EXCEPTION(socket_error()
		    << boost::errinfo_api_function("getaddrinfo")
		    << errinfo_getaddrinfo_error(rc));
	}

	int fd = INVALID_SOCKET;

	for (addrinfo *info = result; info != NULL; info = info->ai_next) {
		fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

		if (fd == INVALID_SOCKET) {
#ifdef _WIN32
			error = WSAGetLastError();
#else /* _WIN32 */
			error = errno;
#endif /* _WIN32 */
			func = "socket";

			continue;
		}

		rc = connect(fd, info->ai_addr, info->ai_addrlen);

		if (rc < 0) {
#ifdef _WIN32
			error = WSAGetLastError();
#else /* _WIN32 */
			error = errno;
#endif /* _WIN32 */
			func = "connect";

			closesocket(fd);

			continue;
		}

		SetFD(fd);

		break;
	}

	freeaddrinfo(result);

	if (GetFD() == INVALID_SOCKET) {
		Log(LogCritical, "UdpSocket")
		    << "Invalid socket: " << Utility::FormatErrorNumber(error);

#ifndef _WIN32
		BOOST_THROW_EXCEPTION(socket_error()
		    << boost::errinfo_api_function(func)
		    << boost::errinfo_errno(error));
#else /* _WIN32 */
		BOOST_THROW_EXCEPTION(socket_error()
		    << boost::errinfo_api_function(func)
		    << errinfo_win32_error(error));
#endif /* _WIN32 */
	}
}

