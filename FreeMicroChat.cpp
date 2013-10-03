// FreeMicroChat.cpp

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

#include <cstdlib>
#include <iostream>

#include "Server.hpp"
#include "Client.hpp"

using namespace std;

/*
  Explicación general:

  Cuando se ejecuta este programa, puede ejecutarse como cliente o como servidor.

  Como servidor:
      ./FreeMicroChat server port

  Como cliente:
      ./FreeMicroChat server_name port nick

  Evidentemente, hay que ejecutar al servidor antes de ejecutar a cualquier cliente.

  Servidor
  -----------------
  El servidor maneja varios clientes, y los comunica entre sí. Cuando se conecta
  un cliente, éste tiene que enviarle su nick. Una vez que el servidor ha recibido
  dicho nick, el servidor le envía al nuevo cliente la lista de contactos (ContactList).

  El servidor entonces 'escucha' a dicho cliente. Cada vez que dicho cliente le envía un
  mensaje, éste contendrá a su destinatario. Entonces el servidor envía el mensaje a dicho
  destinatario. A su vez, cada vez que un nuevo cliente se conecta o se desconecta, envía
  la lista de contactos actualizada a cada uno de sus clientes actualmente conectados.

  Cliente
  -----------------
  El cliente se conecta a un solo servidor. Cuando se conecta le envía su nick, y luego
  recibe la lista de contactos. Cada vez que quiere enviar un mensaje, indica quién es
  el destinatario. Y cuando el cliente recibe un mensaje del servidor, también recibe
  quién es el emisor.

  Desde el punto de vista del usuario potencial, es decir, lo que vé en su pantalla,
  lo que vé es un prompt tal que así: "Nick$>". Si escribe el nombre de otro
  cliente, el prompt cambia tal que así: "Nick->NickDestino$>". Esto significa que
  todo lo que escriba el usuario será dirigido a "NickDestino". Cada vez que el usuario
  envía un mensaje (cada vez que pulsa Intro), aparecerá en su pantalla todos los
  mensajes recibidos (en caso de que haya algunos). Estos mensajes tendrán el formato
  "NickEmisor->Nick$>Mensaje". De esta forma se indica siempre la dirección del mensaje.

  Si el cliente quiere ver la lista de contactos, puede escribir "$" y luego la tecla intro.
  Aparecerá entonces la lista de todos los usuarios conectados.

  Para salir, tendrá que introducir "exit".

  Diseño general del código.
  -----------------------------------
  Existen las clases Server, Client, Message, ContactList, ConnectionList. Por otro lado,
  todos estos ficheros contiene la cabecera Headers.hpp. Debido a que boost es un poco
  engorroso, en esta cabecera éstan todas las definiciones y typedefs que se necesitarán
  a lo largo del código.

  Las clase ConnectionList es una interfaz entre Server/Client y los sockets de
  asio. De esta forma, cuando se quieren enviar o recibir mensajes, Server/Client
  interactúan con ConnectionList, que tiene una interfaz bastante más amigable
  que asio.

  ** ContactList y Message **
  ContactList es una lista de "nicks". Cada nick viene señalado por un identificador
  (unsigned). Server y Client solo se envían dichos identificadores, mientras que
  el usuario ve en su pantalla solamente el nick asociado.

  Message es una clase que tiene tres atributos:
     - MessageType: MSG_NULL para indicar mensaje nulo.
                    MSG_EOF  para indicar fin de conexión.
                    MSG_CONTACT_LIST para indicar que se está enviando la lista
                             de contactos (solo lo envía Server).
                    MSG_NICK para indicar que se envía un nick (solo lo envía Client).
     - Message: El contenido en sí.
     - Target: Identificador del destinatario cuando se envía, y el emisor cuando se
               recibe.

   Los mensajes se envía serializandose (función Message::serialized()), y se deserializan
   con uno de sus constructores. Para enviar la lista de contactos, se envía un mensaje
   de tipo MSG_CONTACT_LIST cuyo mensaje es la serialización de la lista de contactos:
   ContactList::serialize(). La lista de contactos se deserializa con el método
   ContactList::deserialize().

   La clase ContactList tiene dos "grupos" de métodos. El primero es relacionado con lo
   que he decido en llamada: "PhantomContact". Son contactos que se han conectado al
   servidor pero todavía no han enviado su nick. Cuando el nick ha sido recibido,
   se "registra" dicho contacto fantasma en la verdadera lista de contactos, con la
   función ContactList::checkinPhantomContact, que recibe el identificador del contacto.

   ** ConnectionList **
   Esta clase es una lista de sockets y bufferes. Es decir, para cada identificador
   definido en la lista de contactos, se tiene un socket y dos bufferes: uno para
   los datos recibidos, y otro para enviar datos. Para entender esto mejor, véase
   más abajo.

   Esta clase es una interfaz por encima de boost::asio. Es decir, automatiza el
   envío y recepción de datos:
     -Recibir datos: para recibir datos, la función boost::asio::asyn_write(...)
                     necesita un buffer en donde escribir. Este buffer será el
                     buffer de recepción nombrado más arriba. Cuando éstos
                     éstan listos, ConnectionList encapsula los datos recibidos en
                     el buffer en un Message, que será devuelto al Server/Client
                     (a quién corresponda).
     -Enviar datos: Análogo a recibir datos. Una vez recibido el dato, ConnectionList
                    reenvia el Message enviado a Server/Cllient (a quién corresponda).
                    Si hay algún error (en particular, si la comunicación se ha cerrado),
                    se crea un mensaje MSG_EOF que se devuelve al Server/Client en vez
                    del mensaje original (es de observar que mensajes del tipo MSG_EOF
                    nunca se envían a través del socket, solamente en dirección
                    ConnectionList->Server/Client). Si hay un error recibiendo datos,
                    se procede como aquí: enviando un MSG_EOF a Server/Client.

   Si Server/Client recibe un mensaje de tipo MSG_EOF por parte de ConnectionList,
   le dice luego a ConnectionList que elimine la conexión (ConnectionList::deleteConection),
   lo que implica también cerrar el socket. En caso del Client, como solamente tiene
   una conexión (con el Server), cerrar la conexión implica abandonar el programa.

   ** Server **
   El funcionamiento de Server, aunque abstraido de la "sintaxis" y requerimientos
   de boost::asio gracias a la clase ConnectionList, si que tienen en común su forma
   de trabajar, ya que la librería así lo impone.

   --- Generalidades de boost::asio ---
   En el servidor, todo eso asíncrono. Es significa que las tareas de conexión/envío/recibo
   de mensajes se "mandan" al sistema operativo y el programa principal sigue trabajando.
   Cuando finalizan alguna de estas acciones, el sistema operativo lo indica (se dispara
   un evento). Para estas llamadas asíncronas, boost::asio necesita, por tanto, una función
   a la que llamar cuando se dispare dicho evento, llamada callback. Esa función será ejecutada
   en cualquiera de los hilos del programa que estén ejecutando a la función io_service->run().

   Esta función es una función que no acaba hasta que "no haya más trabajo que hacer".
   Es decir, mientras que haya eventos/callbacks por procesar, la función no returnara, y por
   lo tanto mantendrá al hilo ocupado.

   Server solo tiene un único hilo, el propio hilo del programa, así que él es el que ejecuta
   a la función run(). Justo antes de hacerlo, el servidor inicia al loop de conexiones: _connectionControl().
   Esto llama a async_connect() con Server::_newClient() como callback. Justo después de la llamada
   a async_connect() es cuando se ejecuta run(), que se mantendrá latente hasta que no se ejecute dicho
   callback, es decir, hasta que no llegue un nuevo cliente. Cuando llegue, la función _newClient()
   se ejecutará en el hilo actual (ya que run() se llamó aquí).

   A partir de aquí, todas las llamadas asíncronas se hacen a través de ConnectionList.
   La secuencia de callbacks completa es la siguiente:
     _connectionControl() -> _newClient() -> _getNick() -> _newMsg() -> _newMsg()
                                          -> _connectionControl()

   De esta forma, cada vez que llega una conexión, se espera a por otra, mientras se espera
   recibir el nick, y luego sucesivos mensajes. Esto crea una "linea" de callbacks por cada
   nuevo cliente llegado. De esta forma, la función run() al menos tiene siempre un callback
   pendiente: el de la llegada de un nuevo cliente, así que la función nunca returna.
   La única forma de detener al cliente es cerrandolo forzosamente desde fuera.

   ** Client **
   Una vez comprendido como funciona el servidor, el cliente es análogo, con la diferencia de que,
   como solamente hay una conexión (al Server), la conexión es síncrona (es decir,
   hasta que no se conecte no continúa).

   La secuencia de callbacks después de la conexión es la siguiente:
     _talkLoop() -> _sendedNick()
                 -> _recvContactList() -> _newMsg() -> _newMsg()

   Cuando se recibe la lista de contactos, se ejecuta un nuevo hilo
   con la función _readMsg(), que se encarga de leer los mensajes escritos
   por el usuario para enviárselos al servidor. La secuencia de callbacks
   es:
     _readMsg() -> _sendedMsg()
                -> _readMsg()

   Cuando el cliente recibe algún mensaje del tipo MSG_EOF, significa que el servidor se ha caido
   o ha cerrado la conexión, momento en el que Client llama a la función exit() y se termina
   el programa.
 */

void printUsage()
{
    cerr << "Incorrect call. Correct usage: ./FreeMicroChat server port" << endl;
    cerr << "                               ./FreeMicroChat client server_name port nick" << endl;
}

int main(int argc, char **argv)
{
    bool correctCall = true;

    if (argc < 3)
        correctCall = false;
    else if (!strcmp(argv[1], "server")) {
        Server server(atoi(argv[2]));

        server.up();
    }
    else if (!strcmp(argv[1], "client") && argc > 3) {
        string server(argv[2]);
        string nick(argv[4]);

        Client client(server, atoi(argv[3]), nick);

        client.start();
    }
    else
        correctCall = false;

    if (!correctCall)
        printUsage();
}
