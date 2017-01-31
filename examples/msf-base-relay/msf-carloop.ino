// NOTE: You will need to include the carloop library to use this example
// This examples provides a fully functional Metasploit Relay with
// Automotive extensions designed to work with carloop
// Author: Craig Smith

// This #include statement was automatically added by the Particle IDE.
#include <carloop.h>

// This #include statement was automatically added by the Particle IDE.
#include <spark-msf-relay.h>

Carloop<CarloopRevision2> carloop;

class MSFCarloop : public MSFRelay {
public:
    String get_hw_specialty() { return "{ \"automotive\": true }"; }
    String get_hw_capabilities() { return "{ \"can\": true }"; }
    String get_device_name() { return "carloop"; }
    bool on_request_uri(String uri) {
        if(uri.startsWith("/automotive/supported_buses")) {
            HttpResponseStatic resp("[ { \"bus_name\": \"can0\" } ]", strlen("[ { \"bus_name\": \"can0\" } ]"));
            client << resp.status(200);
            return true;
        } else if(uri.startsWith("/automotive/can0/cansend")) {
            std::map <String, String>params = parseQuery(uri);
            if(params.find("id") == params.end() || params.find("data") == params.end()) {
                HttpResponseStatic resp(not_supported(), strlen(not_supported()));
                client << resp.status(404);    
            } else {
                String msg = cansend(params["id"], params["data"]);
                HttpResponseStatic resp(msg, strlen(msg));
                client << resp.status(200);
            }
            return true;
        } else if(uri.startsWith("/automotive/can0/isotpsend_and_wait")) {
            std::map<String, String>params = parseQuery(uri);
            if(params.find("srcid") == params.end() || params.find("dstid") == params.end() || params.find("data") == params.end()) {
                HttpResponseStatic resp(not_supported(), strlen(not_supported()));
                client << resp.status(404);
            } else {
                int timeout = 2000;
                int maxpkts = 3;
                if (params.find("timeout") != params.end()) timeout = params["timeout"].toInt();
                if (params.find("maxpkts") != params.end()) maxpkts = params["maxpkts"].toInt();
                String msg = isotp_send_and_wait(params["srcid"], params["dstid"], params["data"], timeout, maxpkts);
                HttpResponseStatic resp(msg, strlen(msg));
                client << resp.status(200);
            }
            return true;
        }
        return false;
    }
    String cansend(String id, String data) {
        bool success;
        unsigned char tmp;
        int i;
        String resp;
        success = false;
        CANMessage message;
        if (strlen(data) > 16 || strlen(data) == 0 || strlen(id) > 7)
            return String("{ \"status\": \"not supported\" }");
        if (strlen(id) < 9) {
            for(i = 0; i < strlen(id); i++) {
                if((tmp = asc2nibble(id[i])) > 0x0F) {
                    return String("{ \"status\": \"not supported\" }");
                }
                message.id |= (tmp << (2-i)*4);
            }
        }
        message.len = strlen(data) / 2;
        for(i=0; i< message.len; i++) {
            tmp = asc2nibble(data[i * 2]);
            if(tmp > 0x0F)
                return String("{ \"success\": false }");
            message.data[i] = (tmp << 4);
            tmp = asc2nibble(data[i * 2 +1]);
            if(tmp > 0x0F)
                return String("{ \"success\": false }");
            message.data[i] |= tmp;
        }
        success = carloop.can().transmit(message);
        resp ="{ \"success\": ";
        if(success) {
            incPacketCount();
            resp +="true";
        } else {
            resp += "false";
        }
        resp += " }";
        return resp;
    }
    String isotp_send_and_wait(String src, String dst, String data, int timeout, int maxpkts) {
        bool success = false;
        bool first = false;
        bool data_first = false;
        unsigned char tmp;
        unsigned int i, dst_id, current_pkts;
        unsigned long current_time;
        String resp;
        success = false;
        CANMessage message, answer;
        if (strlen(data) > 14 || strlen(data) == 0 || strlen(src) > 7 || strlen(dst) > 7)
            return String("{ \"status\": \"not supported\" }");
        if (strlen(src) < 9) {
            for(i = 0; i < strlen(src); i++) {
                if((tmp = asc2nibble(src[i])) > 0x0F) {
                    return String("{ \"status\": \"not supported\", \"reason\": \"srcid is invalid\" }");
                }
                message.id |= (tmp << (2-i)*4);
            }
        }
        tmp = 0;
        dst_id = 0;
        if (strlen(dst) < 9) {
            for(i = 0; i < strlen(dst); i++) {
                if((tmp = asc2nibble(dst[i])) > 0x0F) {
                    return String("{ \"status\": \"not supported\", \"reason\": \"dstid is invalid\" }");
                }
                dst_id |= (tmp << (2-i)*4);
            }
        }
        message.len = (strlen(data) / 2) + 1;
        message.data[0] = strlen(data) / 2;
        for(i=0; i< message.len - 1; i++) {
            tmp = asc2nibble(data[i * 2]);
            if(tmp > 0x0F)
                return String("{ \"success\": false, \"reason\": \"Byte " + String(data[i*2]) + " is invalid (" + String(i * 2)+ ")\" }");
            message.data[i+1] = (tmp << 4);
            tmp = asc2nibble(data[i * 2 +1]);
            if(tmp > 0x0F)
                return String("{ \"success\": false, \"reason\": \"Byte " + String(data[i*2]) + " is invalid (" + String(i * 2 + 1) + ")\" }");
            message.data[i+1] |= tmp;
        }
    
        carloop.can().clearFilters();
        carloop.can().addFilter(dst_id, 0x7FF);
        success = carloop.can().transmit(message);
        if(success) {
            incPacketCount();
            resp = "{ \"Packets\": [";
            first = true;
            current_pkts = 0;
            current_time = millis();
            while(current_pkts < maxpkts && millis() - current_time < timeout) {
                while(carloop.can().receive(answer)) {
                    if (answer.id == dst_id) {
                        if(first) {
                            resp += "{ ";
                        } else {
                            resp +=", {";
                        }
                        resp += " \"ID\": \"";
                        resp += String::format("%03x", answer.id);
                        resp += "\", ";
                        data_first = true;
                        resp += "\"DATA\": [";
                        for(i = 0; i < answer.len; i++) {
                            if(data_first) {
                                resp += "\"";
                            } else {
                                resp += ", \"";
                            }
                            resp += String::format("%02x", answer.data[i]);
                            resp += "\"";
                            data_first = false;
                        }
                        resp += " ] }";
                        first = false;
                        current_pkts++;
                    }
                }
            }
            resp += " ] }";
        } else {
            return String("{ \"success\": false, \"reaons\": \"Couldn't transmit packet, bus busy?\" }");
        }
        return resp;
    }
};

MSFCarloop msf;


void setup() {
  msf.setup();
  msf.begin();
  carloop.begin();
}

void loop() {
  msf.loop();
  carloop.update();
}
