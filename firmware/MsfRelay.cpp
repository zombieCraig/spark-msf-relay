/**
 * @file
 * @copyright Copyright Â© 2014 by Marc Sibert
 * @author Marc Sibert
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

#include "MsfRelay.h"

MSFRelay::MSFRelay() : TCPServer(8080) {
  rtc.begin(&UDPClient, "north-america.pool.ntp.org");
  rtc.setTimeZone(-8); // gmt offset (-8 for PST)
  startTime = rtc.nowEpoch();
  lastTime = 0UL;
  packets_sent = 0UL;
}

MSFRelay::MSFRelay(const unsigned port) : TCPServer(port) {
  MSFRelay();
}

MSFRelay::~MSFRelay() {

}

/* Does some setup.  These are optional.  Such as publish the
  LocalIP to the Spark console (PRIVATTE)
  and set the D7 (LED) pin to OUTPUT
  */
void MSFRelay::setup() {
  IPAddress myIP = WiFi.localIP();
  Spark.publish("LocalIP", String(myIP), PRIVATE);
  pinMode(D7, OUTPUT);
}

/* Helper function to convert asci to a nibble
 @param c [char] Hex Character to convert
 @return [unsigned char] decimal representation of hex
 */
unsigned char MSFRelay::asc2nibble(char c) {

        if ((c >= '0') && (c <= '9'))
                return c - '0';

        if ((c >= 'A') && (c <= 'F'))
                return c - 'A' + 10;

        if ((c >= 'a') && (c <= 'f'))
                return c - 'a' + 10;

        return 16; /* error */
};

/* parseQuery from URI
 *
 * @param url [String] URI string to parse
 * @return [map <String, String>] Map of key=value pairs
 */
std::map <String, String>MSFRelay::parseQuery(String url) {
    std::map <String, String>params;
    int index;
    String keypair;
    String value, key;
    if((index = url.indexOf('?')) > 0) {
        url = url.substring(index + 1);
        keypair = url;
        while(url.indexOf('&') > 0) {
            keypair = url.substring(0, url.indexOf('&'));
            url = url.substring(url.indexOf('&')+1);
            if(keypair.indexOf('=') > 0) {
                key = keypair.substring(0, keypair.indexOf('='));
                value = keypair.substring(keypair.indexOf('=') + 1);
                params[key] = value;
            }
        }
        if(url.length() > 0)
            keypair = url;
        if(keypair.indexOf('=') > 0) {
            key = keypair.substring(0, keypair.indexOf('='));
            value = keypair.substring(keypair.indexOf('=') + 1);
            params[key] = value;
        }
    }
    return params;
}

/* Helper function to increment packet sent and lastTime sent */
void MSFRelay::incPacketCount() {
    packets_sent++;
    lastTime = rtc.nowEpoch();
}

/* Main URL Loop handler.  Distributes call to all the core APIs.  Override
  on_request_uri to add custom URI handlers */
void MSFRelay::loop() {
  if (client = available()) {

      HttpRequest hr;
      while (int nb = client.available()) {
          for (int i = nb; i > 0; --i) {
              const char buf = client.read();
              hr.parse(buf);
          }
      }
      String uri = String(hr.URL());
// Check for a custom match -----------------------------------------------------------------------------
      if (on_request_uri(uri)) {
        // We do not need to respond since the overriding class handled it
//Status -----------------------------------------------------------------------------------------------
      } else if (uri.startsWith("/status")) {
          HttpResponseStatic resp(get_api_status(), strlen(get_api_status()));
          client << resp.status(200);
//Statistics ----------------------------------------------------------------------------------------------
      } else if (uri.startsWith("/statistics")) {
          HttpResponseStatic resp(get_api_statistics(), strlen(get_api_statistics()));
          client << resp.status(200);
//DateTime
      } else if (uri.startsWith("/settings/datetime")) {
          HttpResponseStatic resp(get_api_datetime(), strlen(get_api_datetime()));
          client << resp.status(200);
//Timezone
      } else if (uri.startsWith("/settings/timezone")) {
          HttpResponseStatic resp(get_api_timezone(), strlen(get_api_timezone()));
          client << resp.status(200);
// Get IP Information
      } else if (uri.startsWith("/settings/ip/config")) {
          HttpResponseStatic resp(get_api_ipconfig(), strlen(get_api_ipconfig()));
          client << resp.status(200);
// Custom Methods
      } else if (uri.startsWith("/custom_methods")) {
          HttpResponseStatic resp(get_custom_methods(), strlen(get_custom_methods()));
          client << resp.status(200);
//Default Response ----------------------------------------------------------------------------------------------
      } else {
          String msg;
          msg = "{ \"status\", \"not supported\" }";
          HttpResponseStatic resp(msg, strlen(msg));
          client << resp.status(404);
      }
      client.flush();
      delay(100);  // TODO: See if we truly need this delay
      client.stop();
  }
}

/***  API CALLS ****/
/* API Status Call */
String MSFRelay::get_api_status() {
    String resp;
    resp = "{ \"operational\": " + get_operational() + ", ";
    resp += " \"hw_specialty\": " + get_hw_specialty() + ", ";
    resp += " \"hw_capabilities\": " + get_hw_specialty() + ", ";
    resp += " \"api_verison\": \"" + get_api_version() + "\", ";
    resp += " \"fw_version\": \"" + get_fw_version() + "\", ";
    resp += " \"hw_version\": \"" + get_hw_version() + "\" ";
    if (get_device_name().length() > 0) {
      resp += ", \"device_name\": \"" + get_device_name() + "\" ";
    }
    resp += "}";
    return resp;
};

String MSFRelay::get_api_statistics() {
    String resp;
    resp = " { \"uptime\": ";
    resp += get_uptime();
    resp += ", ";
    resp += " \"packet_stats\": ";
    resp += get_packets_sent();
    resp += ", ";
    resp += " \"last_request\": ";
    resp += get_lasttime();
    resp += ", ";
    resp +=" \"voltage\":";
    resp += get_voltage();
    resp += " }";
    return resp;
};

String MSFRelay::get_api_datetime() {
  String resp;
  resp = "{ \"system_timezone\": ";
  resp += get_datetime();
  resp += " }";
  return resp;
}

String MSFRelay::get_api_timezone() {
  String resp;
  resp = "{ \"system_timezone\": ";
  resp += get_timezone();
  resp += " }";
  return resp;
}

String MSFRelay::get_api_ipconfig() {
    String resp;
    resp = " { \"setup\": \" " + get_ip_setup() + "\", ";
    resp += " \"ipv4_addr\": \"" + get_ip4_addr() + "\", ";
    resp += " \"ipv4_netmask\": \"" + get_ip4_netmask() + "\", ";
    resp += " \"ipv4_gw\": \"" + get_ip4_gateway() + "\", ";
    resp += " \"dns\": " + get_ip4_dns() + " }";
    return resp;
}

/*** API Overides ***/

/*** Status Overrides ***/

/* Returns operational state
 * @return [String] Operational state: 0=Unknown, 1=Yes, 2=No
 */
String MSFRelay::get_operational() {
  return String("0");
}

/* Returns the hardware specialty
 * @return [String] Example { "automotive": true }
 */
String MSFRelay::get_hw_specialty() {
  return String("{}");
}

/* Returns the hardware capabilities
 * @return [String] Example { "can": true }
 */
String MSFRelay::get_hw_capabilities() {
  return String("{ \"custom_methods\": true }");
}

/* Returns the supported HWBridge API Version
 * @return [String] Example: 0.0.1
 */
String MSFRelay::get_api_version() {
  return String(MSF_HWBRIDGE_API_VERSION);
}

/* Returns the firmware version of the device
 * @return [String] Example: 0.0.1
 */
String MSFRelay::get_fw_version() {
  return String("0.0.1");
}

/* Returns the hardware build version
 * @return [String] Example: 0.0.1
 */
String MSFRelay::get_hw_version() {
  return String("0.0.1");
}

/* Optional name of the device.  Only displayed if present
 * @return [String] Example: carloop
 */
String MSFRelay::get_device_name() {
  return String("");
}

/*** Statistic Overrides ***/

/* Defalts to return the the uptime of the device
 * @return [String] uptime in Epch
 */
String MSFRelay::get_uptime() {
  return String(rtc.nowEpoch() - startTime);
}

/* Gets the packet sent count
 @return [String] packet sent count
*/
String MSFRelay::get_packets_sent() {
  return String(packets_sent);
}

/* Gets the last time a packet was sent
 @return [String] last time packet sent in Epoch
*/
String MSFRelay::get_lasttime() {
  return String(lastTime);
}

/* Returns the devices current voltage usage
 @return [String] Voltage number
 */
String MSFRelay::get_voltage() {
  return String("0.0");
}

/*** DateTIme Overrides ***/
/* Returns the datetime as Epoch
 @return [String] datetime as Epoch
 */
String MSFRelay::get_datetime() {
  return String(rtc.nowEpoch());
}

/* Returns the timezone
 @return [String] timezone, example PST
 */
String MSFRelay::get_timezone() {
  return String("PST");
}

/*** IP Config Overrides ***/
/* Return if the IP was dynamically setup or statically
  @return [String] Either 'dynamic' or 'static'
  */
String MSFRelay::get_ip_setup() {
  return String("dynamic");
}

/* The ipv4 assigned to the Ethernet interface
 @return [String] ipv4 address
 */
String MSFRelay::get_ip4_addr() {
  return String(WiFi.localIP());
}

/* The ipv4 netmask
 @return [String] ipv4 netmask
 */
String MSFRelay::get_ip4_netmask() {
  return String(WiFi.subnetMask());
}

/* The ipv4 gateway
  @return [String] ipv4 gateway IP
  */
String MSFRelay::get_ip4_gateway() {
  return String(WiFi.gatewayIP());
}

/* Returns an array of ipv4 dns servers
 @return [String] JSON array of DNS servers
 */
String MSFRelay::get_ip4_dns() {
  return String("[ \"" + String(WiFi.dnsServerIP()) + "\" ]");
}

/*** Custom Method Override ***/
/* Returns the full hash of all the custom methods.  By default only the
    onboard LED is exposed.  This whole method can be overriden for any
    custom functionality
  @return [String] JSON Hash of custom methods supported
  */
String MSFRelay::get_custom_methods() {
    String resp;
    resp += "{ \"Methods\": [ ";
    resp += "{ \"method_name\": \"led\", ";
    resp +=" \"method_desc\": \"Changes the state of the LED\", ";
    resp += " \"args\": [ ";
    resp +="   { \"arg_name\": \"state\", \"arg_type\": \"string\", \"required\": true } ";
    resp +="  ], ";
    resp +=" \"return\": \"string\" } ";
    resp +=" ] }";
    return resp;
};

/* Generic status not supported message
  @return [String] { 'status': 'not supported' }
  */
String MSFRelay::not_supported() {
    return String("{ \"status\": \"not supported\" }");
};

/* Custom request handler.  Override this for any URI calls you wish to
intercept that is reported by get_custom_methods.  For example, the led method
is exposed by default so this method checks for URL calls to led?state= to
toggle the led state.
 @return [Bool] Return true if uri was intercepted
 */
bool MSFRelay::on_request_uri(String uri) {
  if (uri.startsWith("/led\?state=")) {
     String mesg;
     mesg = "{ \"success\": false, \"value\": \"ERROR\" }";
     std::map<String, String>params = parseQuery(uri);
     if(params.find("state") == params.end()) {
        HttpResponseStatic resp(not_supported(), strlen(not_supported()));
        client << resp.status(404);
     } else {
       if (!strcmp(params["state"], "on") || !strcmp(params["state"], "ON")) {
         digitalWrite(D7, HIGH);
         mesg = "{ \"value\": \"ON\" }";
       } else if (!strcmp(params["state"], "off") || strcmp(params["state"], "OFF")) {
       digitalWrite(D7, LOW);
         mesg = "{ \"value\": \"OFF\" }";
       }
       HttpResponseStatic resp(mesg, strlen(mesg));
       client << resp.status(200);
     }
     return true;
   }
   return false;
}
