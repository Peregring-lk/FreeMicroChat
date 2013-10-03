// Header.hpp

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

#include <cstddef>

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <queue>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>

using namespace std;
using namespace boost;
using namespace asio;
using namespace ip;
using namespace serialization;
using namespace archive;
using namespace system;

using ip::tcp;

typedef boost::shared_ptr<io_service> PtrService;
typedef boost::shared_ptr<asio::streambuf> PtrBuffer;
typedef boost::shared_ptr<tcp::socket> PtrSocket;
typedef ip::basic_resolver_entry<tcp> EndPoint;
