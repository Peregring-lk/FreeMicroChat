// ConnectionList.cpp

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

#include "ConnectionList.hpp"

ConnectionList::ConnectionList(const PtrService& service)
    : _service(service)
{}

void ConnectionList::newConnection(unsigned id)
{
    PtrSocket socket(new tcp::socket(*_service));
    PtrBuffer recvBuffer(new asio::streambuf);
    PtrBuffer sendBuffer(new asio::streambuf);

    pair<PtrBuffer, PtrBuffer> buffers(recvBuffer, sendBuffer);

    _connectionList[id] = pair<PtrSocket, pair<PtrBuffer, PtrBuffer>>(socket, buffers);
}

bool ConnectionList::hasConnection(unsigned id) const
{
    auto found = _connectionList.find(id);

    return found != _connectionList.end();
}

PtrSocket ConnectionList::getSocket(unsigned id) const
{
    if (hasConnection(id))
        return _connectionList.find(id)->second.first;
    else
        return PtrSocket();
}

PtrBuffer ConnectionList::getRecvBuffer(unsigned id) const
{
    if (hasConnection(id))
        return _connectionList.find(id)->second.second.first;
    else
        return PtrBuffer();
}

PtrBuffer ConnectionList::getSendBuffer(unsigned id) const
{
    if (hasConnection(id))
        return _connectionList.find(id)->second.second.second;
    else
        return PtrBuffer();
}

void ConnectionList::connect(unsigned id, EndPoint endPoint)
{
    if (hasConnection(id))
        getSocket(id)->connect(endPoint);
}

void ConnectionList::deleteConnection(unsigned id)
{
    if (hasConnection(id)) {
        system::error_code ec;

        PtrSocket socket = getSocket(id);

        socket->shutdown(tcp::socket::shutdown_both, ec);
        socket->close();

        _connectionList.erase(id);
    }
}
