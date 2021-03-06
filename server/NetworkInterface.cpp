/**
	file: NetworkInterface.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Friday, 11th May 2012
**/

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>

#include "exceptions.h"
#include "NetworkInterface.h"

NetworkInterface::NetworkInterface(int port, int backlog)
{
	// create a socket for listening
	this->listener = socket(AF_INET, SOCK_STREAM, 0);
	if (this->listener == -1)
	{ throw Exception::ErrnoError("failed to create listening socket", "socket"); }
	
	// generate listening socket address structure and bind
	struct sockaddr_in socket_address =
	{
		.sin_family = AF_INET,
		.sin_port = htons(port)
	};
	if (inet_pton(AF_INET, "127.0.0.1", &socket_address.sin_addr) == -1)
	{ throw Exception::ErrnoError("failed to generate network address structure", "inet_pton"); }
	
	// bind listener
	if (bind(this->listener, reinterpret_cast<sockaddr *>(&socket_address), sizeof(socket_address)) == -1)
	{ throw Exception::ErrnoError("failed to bind listening socket", "bind"); }
	
	// listen
	if (listen(this->listener, backlog) == -1)
	{ throw Exception::ErrnoError("failed to listen", "listen"); }
}

void NetworkInterface::add_message_handler(const NetworkMessageHandler handler)
{ message_handlers.push_front(handler); }

void NetworkInterface::remove_message_handler(const NetworkMessageHandler handler)
{ message_handlers.remove(handler); }

void NetworkInterface::run(int ipc_socket)
{
	bool ipc_required = false;

	while (!ipc_required)
	{
		// generate fd_set
		fd_set set;
		FD_ZERO(&set);
		FD_SET(this->listener, &set);
		FD_SET(ipc_socket, &set);

		int end = std::max(this->listener, this->clients.fill_fd_set(&set));
		end = std::max(end, ipc_socket);
		end += 1;

		// select
		int selected_amount = select(end, &set, 0, 0, 0);
		if (selected_amount == -1)
		{
			throw Exception::ErrnoError("select failed", "select");
		}

		// check for incoming client connections
		if (selected_amount == 0)
		{ continue; }

		if (FD_ISSET(this->listener, &set))
		{
			this->clients.accept_client(this->listener);
			--selected_amount;
			/* Daniel -> Max:
			 * I added this here, because obviously you don't
			 * want to check for messages on the listening socket
			 * right?
			 */
			FD_CLR(this->listener, &set);
		}

		if (FD_ISSET(ipc_socket, &set))
		{
			/* the ipc socket doesn't care about messages, hence reduce
			 * the amount of messages required
			 * also clear it from the set so it won't be accessed
			 * by the message things below
			 */
			--selected_amount;
			FD_CLR(ipc_socket, &set);
			ipc_required = true;
		}

		// receive messages
		MessageList messages(selected_amount);
		this->clients.get_messages_by_fd_set(&set, end, messages);

		// process received messages
		for (const Message &message: messages)
		{
			// skip if message is empty or invalid
			if (message.is_empty() || message.type == Message::TYPE_INVALID)
			{ continue; }

			// trigger events for all event handlers
			for (const NetworkMessageHandler &handler: message_handlers)
			{ handler(message); }
		}
	}
}
