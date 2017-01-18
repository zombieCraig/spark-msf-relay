spark-msf-relay
========================

Metasploit HWBridge API Relay Library

# HTTP server (REST & JSON) on [Spark core](https://www.spark.io/)
![alt text](https://s3.amazonaws.com/spark-website/spark.png "")
## EN : Local HTTP server (REST & JSON) for Sparkcore

This project implement a Http server on Sparkcore itself. Once compiled into the [Web IDE](https://www.spark.io/build), the server will be available at http://IP-SPARKCORE (port 8080 by default).

### Common files of projects :
- slre.h
- slre.cpp
- HttpResponse.h
- HttpResponse.cpp
- HttpRequest.h
- HttpRequest.cpp
- http_parser.h
- http_parser.cpp
- SparkTime.h
- SparkTime.cpp

Thanks to :
- [CaptainIgloo / spark-web-embd-rest-json](https://github.com/captainigloo/spark-web-embd-rest-json)
- [SLRE: Super Light Regular Expression library](https://github.com/cesanta/slre)
- [Marcussacapuces91 Github / SparkCore-RESTserver ](https://github.com/Marcussacapuces91/SparkCore-RESTserver)
- [Joyent Github/ http-parser](https://github.com/joyent/http-parser)

### Projet "MSF_Relay.ino"

TODO: Describe how to find the IP in console.  How to connect via Metasploit

### Installation
- [Connect the Spark Core to your wifi](https://www.spark.io/start)
- [Deploy the firmware using the Spark Cloud](https://www.spark.io/build/)
- How add files in project :

![Add file](http://domotique-info.fr/wp-content/uploads/2014/04/domotique-info-webideaddlib.gif)

### Howto add custom functions

TODO: Describe how to do custom methods
