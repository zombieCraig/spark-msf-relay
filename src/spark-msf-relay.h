/**
 * @file
 * @copyright Copyright © 2017 by Craig Smith
 * @author Craig Smith
 *
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MY_HTTP_RESPONSE_H__
#define __MY_HTTP_RESPONSE_H__

#include "SparkTime.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "http_parser.h"

#include <map>

#define MSF_HWBRIDGE_API_VERSION "0.0.1"

/**
 * Main Metasploit HWBridge Relay class
 */
class MSFRelay : public TCPServer {
public:
  MSFRelay();
  MSFRelay(const unsigned);
  ~MSFRelay();
  unsigned char asc2nibble(char);
  std::map <String, String>parseQuery(String);
  void incPacketCount();
  void setup();
  void loop();
  /* API */
  String get_api_status();
  String get_api_statistics();
  String get_api_datetime();
  String get_api_timezone();
  String get_api_ipconfig();
  /* Overrides */
  virtual String get_operational();
  virtual String get_hw_specialty();
  virtual String get_hw_capabilities();
  virtual String get_api_version();
  virtual String get_fw_version();
  virtual String get_hw_version();
  virtual String get_device_name();
  virtual String get_uptime();
  virtual String get_packets_sent();
  virtual String get_lasttime();
  virtual String get_voltage();
  virtual String get_datetime();
  virtual String get_timezone();
  virtual String get_ip_setup();
  virtual String get_ip4_addr();
  virtual String get_ip4_netmask();
  virtual String get_ip4_gateway();
  virtual String get_ip4_dns();
  virtual String get_custom_methods();
  virtual String not_supported();
  virtual bool on_request_uri(String);
  // Client for responses
  TCPClient client;
private:
  unsigned long startTime;
  unsigned long lastTime;
  SparkTime rtc;
  UDP UDPClient;
  unsigned long packets_sent;
};

#endif
