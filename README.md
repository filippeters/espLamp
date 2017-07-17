# espLamp

esp8266 + string of ws2811/12 leds + mqtt 
little project to try out some nodemcu boards.

I've used a couple of nodemcu boards, connnected a bunch of ws2812 led strips to them.
This little sketch is meant to setup a wifi portal to connect them into the main wifi and also setup a connection to a mqtt broker.
Once that's done, mqtt can be setup on a raspberry pi and add homekit support.



## arduino setup ## 
* install arduino environment
* add support for the esp8266
	- in preferences add "http://arduino.esp8266.com/stable/package_esp8266com_index.json" to the additional boards manager urls.
	- in Tools/Board/Boards manager type esp8266 and install the "esp8266 by ESP8266 Comunity" package
	- in Tools/Board, choose the NodeMCU board.
	- in Sketch/Include Library/Manage Libraries... add WifiManager, ESP8266WebServer, DNSServer if not yet installed
* add support for json:
	- in Sketch/Include Library/Manage Libraries... add ArduinoJson if not yet installed
* add support for mqtt:	
	- in Sketch/Include Library/Manage Libraries... add PubSubClient if not yet installed
	
## raspberry pi setup ##
I prefer raspberry pi's without all the hdmi cable mess, hence set it up, hide it somewhere and connect with ssh.
Prepare an sd card with raspbian, mount it and set it up to connect with the home wifi. 
Preferably with a fixed ip address, this will allow to remember it for the mqtt address.

###mqtt on raspberry pi###

###homekit on raspberry pi###

###web interface###



	