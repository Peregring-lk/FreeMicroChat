// ConnectionList.hpp

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

#ifndef _CONNECTION_LIST_
#define _CONNECTION_LIST_

#include "Headers.hpp"

#include "Message.hpp"

class ConnectionList
{
public:
    ConnectionList(const PtrService& service);

    // Esta función no conecta al socket, solamente crea el sockets y los buffers.
    void newConnection(unsigned id);

    bool hasConnection(unsigned id) const;

    PtrSocket getSocket(unsigned id) const;
    PtrBuffer getRecvBuffer(unsigned id) const;
    PtrBuffer getSendBuffer(unsigned id) const;

    // Esta función solamente es usada en el módulo cliente, ya que el servidor
    // trabaja con un socket pasivo.
    void connect(unsigned id, EndPoint endPoint);

    // Esta función desconecta el socket, además de borrar la conexión
    // de la tabla.
    void deleteConnection(unsigned id);

private:
    PtrService _service;

    map<unsigned, pair<PtrSocket, pair<PtrBuffer, PtrBuffer>>> _connectionList;

    // Generic functions.
    /*
      Estas funciones deben ser implementadas aquí.
      En caso contrario el compilador no las encontraría
      por culpa del mecanismo de plantillas.
    */

    /*
      Debido a que Server, por simplicidad, trabaja con Message y ConnectionList, haciendo a la clase
      algo más "legible", la clase ConnectionList debe tragarse todo ese manejo de callbacks. Por ello,
      las funciones ConnectionList::recvMsg y ConnectionList::sendMsg usan como callbacks para boost::asio
      las funciones privadas ConnectionList::_recvMsg, ConnectionList::_sendMsg. De esta forma, sendMsg
      llamada a async_write (para enviar), con _sendMsg como callback; y recvMsg llama a async_read_until
      con _recvMsg como callback. Pero, por su parte, Server necesita también ser notificado cuando el
      mensaje se envíe o reciba. Por ello, sendMsg/recvMsg reciben como parámetro, a su vez, otro callback.
      Este será Server::_getNick cuando se reciba un nick, Server::_newMsg cuando se reciba un mensaje
      nuevo por parte de los clientes, y Server::_sendedMsg cuando se envíen. ConnectionList::recvMsg
    */
public:
    template<typename F, typename O>
    void recvMsg(unsigned id, F f, O o)
    {
        if (hasConnection(id))
            async_read_until(*getSocket(id), *getRecvBuffer(id), '\0',
                             bind(&ConnectionList::_recvMsg<F, O>, this, id, f, o, _1, _2));
    }

    template<typename F, typename O>
    void sendMsg(unsigned id, F f, O o, Message msg)
    {
        if (hasConnection(id)) {
            PtrBuffer buffer = getSendBuffer(id);
            ostream os(&(*buffer));

            os << msg.serialize() << '\0';

            async_write(*getSocket(id), *buffer,
                        bind(&ConnectionList::_sendMsg<F, O>, this, id, f, o, msg, _1, _2));
        }
    }

    // Callbacks.
private:
    template<typename F, typename O>
    void _recvMsg(unsigned id, F f, O o, const error_code& ec, size_t bytes)
    {
        if (ec == error::eof || bytes < 1)
            (o->*f)(id, Message(Message::MSG_EOF));
        else {
            if (hasConnection(id)) {
                PtrBuffer buffer = getRecvBuffer(id);

                Message msg(buffer);
                (o->*f)(id, msg);
            }
        }
    }

    template<typename F, typename O>
    void _sendMsg(unsigned id, F f, O o, Message msg, const error_code& ec, size_t bytes)
    {
        if (ec == error::eof)
            (o->*f)(id, Message(Message::MSG_EOF));
        else
            (o->*f)(id, msg);
    }
};

#endif // _CONNECTION_LIST_
