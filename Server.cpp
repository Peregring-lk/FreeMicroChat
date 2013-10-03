// Server.cpp

/*
  Copyright (C) 2012 Aarón Bueno Villares <abv150ci@gmail.com>
  Copyright (C) 2012 Juan Antonio Tejero Fernández <juanantonio.tejerofernandez@alum.uca.es>
  Copyright (C) 2012 María Inmaculada Labrador del Río <inmaculada.labrador@uca.es>

  This file is part of FreeMicroChat.

  FreeMicroChat is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FreeMicroChat is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FreeMicroChat.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Server.hpp"
#include "Message.hpp"

Server::Server(unsigned short portNum)
    : _service(new io_service), _acceptor(*_service), _portNum(portNum), _connectionList(_service)
{}

void Server::up()
{
    _acceptor.open(tcp::v4());
    _acceptor.set_option(socket_base::reuse_address(true));
    _acceptor.bind(tcp::endpoint(tcp::v4(), _portNum));
    _acceptor.listen(socket_base::max_connections);

    _connectionControl();

    _service->run();
}

void Server::_connectionControl()
{
    unsigned id = _contactList.addPhantomContact();

    _connectionList.newConnection(id);

    _acceptor.async_accept(*_connectionList.getSocket(id), boost::bind(&Server::_newClient, this, id, _1));
}

void Server::_newClient(unsigned id, const error_code& ec)
{
    // TODO: ¿error_code?
    _connectionList.recvMsg(id, &Server::_getNick, this);

    _connectionControl();
}

void Server::_getNick(unsigned id, Message msg)
{
    if (msg.messageType() == Message::MSG_EOF) {
        _contactList.releasePhantomContact(id);
        _connectionList.deleteConnection(id);
    }
    else {
        if (msg.messageType() != Message::MSG_NICK || msg.emptyMsg())
            _connectionList.recvMsg(id, &Server::_getNick, this);
        else {
            _contactList.checkinPhantomContact(id, msg.message());

            _broadcastContactList();

            _connectionList.recvMsg(id, &Server::_newMsg, this);
        }
    }
}

void Server::_newMsg(unsigned id, Message msg)
{
    if (msg.messageType() == Message::MSG_EOF) {
        _contactList.releaseContact(id);
        _connectionList.deleteConnection(id);
        _broadcastContactList();
    }
    else {
        if (msg.messageType() == Message::MSG_NORMAL) {
            // Recibimos un mensaje de un cliente, que contiene al destinatario,
            // msg.target(). Por tanto, se envía a 'target' el mismo mensaje.
            unsigned idTarget = msg.target();

            if (_contactList.hasContact(idTarget)) {
                Message newMsg(Message::MSG_NORMAL, msg.message(), id);

                _connectionList.sendMsg(idTarget, &Server::_sentMsg, this, newMsg);
            }
        }

        _connectionList.recvMsg(id, &Server::_newMsg, this);
    }
}

void Server::_sentMsg(unsigned id, Message msg)
{}

void Server::_broadcastContactList()
{
    Message newMsg(Message::MSG_CONTACT_LIST, _contactList.serialize());

    auto f = &ConnectionList::sendMsg<decltype(&Server::_newMsg), Server*>;

    _contactList.broadcastMessage(bind(f, &_connectionList, _1, &Server::_sentMsg, this, _2), newMsg);
}
