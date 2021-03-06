/**
	file: Message.cpp
	author: Maximilian Lasser [max.lasser@online.de]
	created: Tuesday, 22nd May 2012
**/

#include "Client.h"
#include "exceptions.h"
#include "Message.h"

Message::Message(void):
	length(0), id(0), position(0), source(NULL), status(STATUS_NOT_OK), type(TYPE_INVALID)
{}

void Message::receive_from(ClientSptr client)
{
	char buffer;

	// save source
	source = client;

	// get message type
	client->receive(&buffer, FIELD_SIZE_TYPE);
	type = static_cast<MessageType>(buffer);
	
	// get first data
	switch (type)
	{
		case TYPE_DOC_ACTIVATE:
		case TYPE_DOC_SAVE:
			client->receive(&id, FIELD_SIZE_ID);
			id = ntohl(id);
			break;
		case TYPE_DOC_CREATE:
		case TYPE_DOC_DELETE:
		case TYPE_DOC_OPEN:
			name.resize(FIELD_SIZE_DOC_NAME);
			client->receive(&name, FIELD_SIZE_DOC_NAME);
			break;
		case TYPE_SYNC_BYTE:
			bytes.resize(FIELD_SIZE_BYTE);
			client->receive(&bytes, FIELD_SIZE_BYTE);
			break;
		case TYPE_SYNC_CURSOR:
		case TYPE_SYNC_DELETION:
			client->receive(&position, FIELD_SIZE_SIZE);
			position = ntohl(position);
			break;
		case TYPE_SYNC_MULTIBYTE:
			client->receive(&length, FIELD_SIZE_SIZE);
			length = ntohl(length);
			break;
		case TYPE_USER_LOGIN:
			name.resize(FIELD_SIZE_USER_NAME);
			client->receive(&name, FIELD_SIZE_USER_NAME);
			break;
		case TYPE_USER_LOGOUT: break;
		default:
			throw Exception::InvalidMessageType("invalid message type", type, client->socket);
	}
	
	// get second data
	switch (type)
	{
		case TYPE_DOC_ACTIVATE:
		case TYPE_USER_LOGIN:
			hash.resize(FIELD_SIZE_HASH);
			client->receive(&hash, FIELD_SIZE_HASH);
			break;
		case TYPE_SYNC_DELETION:
			client->receive(&length, FIELD_SIZE_SIZE);
			length = ntohl(length);
			break;
		case TYPE_SYNC_MULTIBYTE:
			bytes.resize(length);
			client->receive(&bytes, length);
			break;
		default: break;
	}
}

std::vector<char> &Message::generate_bytestream(std::vector<char> &dest) const
{
	// append message type
	append_bytes(dest, static_cast<char>(type));

	// append first data
	switch (type)
	{
		case TYPE_DOC_ACTIVATE:
		case TYPE_DOC_CREATE:
		case TYPE_DOC_DELETE:
		case TYPE_DOC_OPEN:
		case TYPE_DOC_SAVE:
		case TYPE_USER_LOGIN:
		case TYPE_STATUS:
			append_bytes(dest, static_cast<char>(status));
			break;
		case TYPE_SYNC_BYTE:
		case TYPE_SYNC_DELETION:
		case TYPE_SYNC_MULTIBYTE:
			append_bytes(dest, htonl(position));
			break;
		case TYPE_USER_JOIN:
		case TYPE_USER_QUIT:
			append_bytes(dest, htonl(id));
			break;
		default:
			throw Exception::InvalidMessageType("invalid message type", type, 0);
	}

	// append second data
	switch (type)
	{
		case TYPE_DOC_ACTIVATE:
		case TYPE_DOC_OPEN:
		case TYPE_DOC_SAVE:
			append_bytes(dest, htonl(id));
			break;
		case TYPE_DOC_CREATE:
		case TYPE_DOC_DELETE:
			append_bytes(dest, &name, FIELD_SIZE_DOC_NAME);
			break;
		case TYPE_SYNC_BYTE:
			append_bytes(dest, &bytes, FIELD_SIZE_BYTE);
			break;
		case TYPE_SYNC_DELETION:
		case TYPE_SYNC_MULTIBYTE:
			append_bytes(dest, htonl(length));
			break;
		case TYPE_USER_JOIN:
			append_bytes(dest, &name, FIELD_SIZE_USER_NAME);
			break;
		default: break;
	}

	// append third data
	switch (type)
	{
		case TYPE_DOC_OPEN:
			append_bytes(dest, &name, FIELD_SIZE_DOC_NAME);
			break;
		case TYPE_SYNC_MULTIBYTE:
			append_bytes(dest, &bytes, length);
			break;
		default: break;
	}

	return dest;
}

void Message::send_to(Client &client) const
{
	// generate bytestream to send
	std::vector<char> bytestream;
	generate_bytestream(bytestream);

	// send
	client.send(bytestream);
}

void Message::send_to(ClientCollection &clients) const
{
	// generate bytestream to send
	std::vector<char> bytestream;
	generate_bytestream(bytestream);

	// send
	clients.broadcast(bytestream);
}
