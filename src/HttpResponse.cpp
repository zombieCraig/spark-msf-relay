/**
 * @file
 * @copyright Copyright Â© 2014 by Marc Sibert
 * @author Marc Sibert
 * @author Craig Smith
 * 
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file or http://www.wtfpl.net/
 * for more details.
 */ 
 
#include "HttpResponse.h"

const char* const HttpResponse::DEFAULT_CONTENT_TYPE = "Content-Type: application/json";

void operator<<(Stream& aStream, const HttpResponse& aResponse) {
    aStream.print("HTTP/1.1 ");
    aStream.print(aResponse.fStatus);
    switch (aResponse.fStatus) {
        case 200 : 
            aStream.print(" OK\r\n");
            break;
        case 404 : 
            aStream.print(" Not Found\r\n");
            break;
        default :
            aStream.print(" \r\n");
            break;
    }
    if (aResponse.fContentType) {
        aStream.print("Content-Type: text/html; application/json\r\n"); 
        aStream.print(aResponse.fContentType); 
        aStream.print("\r\n");
    }
    aStream.print("Connection: close\r\n");
    aStream.print("Server: Apache\r\n");
    if (aResponse.fContentLength >= 0) {
        aStream.print("Content-Length:"); 
        aStream.print(aResponse.fContentLength); 
        aStream.print("\r\n");
    }
    aStream.print("\r\n");
    
    aResponse.printBody(aStream);
}
