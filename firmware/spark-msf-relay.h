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
  String get_operational();
  String get_hw_specialty();
  String get_hw_capabilities();
  String get_api_version();
  String get_fw_version();
  String get_hw_version();
  String get_device_name();
  String get_uptime();
  String get_packets_sent();
  String get_lasttime();
  String get_voltage();
  String get_datetime();
  String get_timezone();
  String get_ip_setup();
  String get_ip4_addr();
  String get_ip4_netmask();
  String get_ip4_gateway();
  String get_ip4_dns();
  String get_custom_methods();
  String not_supported();
  bool on_request_uri(String);
private:
  unsigned long startTime;
  unsigned long lastTime;
  SparkTime rtc;
  UDP UDPClient;
  TCPClient client;
  unsigned long packets_sent;
};

#endif
