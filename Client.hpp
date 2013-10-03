// Client.hpp

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

#ifndef _CLIENT_
#define _CLIENT_

#include "Headers.hpp"

#include "ContactList.hpp"
#include "ConnectionList.hpp"

class Client
{
public:
    Client(const string& serverName, unsigned short port, const string& nick);

    void start();

private:
    PtrService _service;

    string _serverName;
    unsigned short _port;

    string _nick;

    ContactList _contactList;
    ConnectionList _connectionList;

    unsigned _idConnection;
    unsigned _idNick;
    unsigned _lastEndNick;

    queue<string> _messageQueue;

    thread_group _threads;

    bool _connect;

private:
    void _talkLoop();
    void _sentNick(unsigned id, Message msg);
    void _recvContactList(unsigned id, Message msg);

    void _printPrompt(unsigned idSource, unsigned idTarget = ContactList::ID_NULL, string msg = "", ostream& os = cout);
    void _printQueueMsg();
    void _readMsg();

    void _newMsg(unsigned id, Message msg);
    void _sentMsg(unsigned id, Message msg);

    void _exit();
};

#endif // _CLIENT_
