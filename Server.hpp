// Server.hpp

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

#ifndef _SERVER_
#define _SERVER_

#include <cstddef>

#include "Headers.hpp"

#include "ContactList.hpp"
#include "ConnectionList.hpp"

// Class.
class Server
{
private:
    typedef boost::function<void(unsigned, Message)> ServerFuncObj;

public:
    Server(unsigned short portNum);

    void up();

private:
    PtrService _service;
    ip::tcp::acceptor _acceptor;

    unsigned short _portNum;

    ContactList _contactList;
    ConnectionList _connectionList;

private:
    void _connectionControl();
    void _newClient(unsigned id, const system::error_code& ec);
    void _getNick(unsigned id, Message msg);
    void _newMsg(unsigned id, Message msg);
    void _sentMsg(unsigned id, Message msg);
    void _broadcastContactList();
};

#endif // _SERVER_
