Packet Hatchet
---------------

##Description
Packet Hatchet is a highly customizable tool similar to netcat that allows for simple communication over a variety of Internet protocols. It is an experiment in using the SOCK_RAW interface and spoofing packet contents.

Eventually, I hope this program (or code from this program) may be used as the foundation for exploring more complex spoofing attacks.

##How To Use
	-h, --help                prints the command glossary
	-m                        prints the external ip of the interface currently being used
	-b                        creates a bouncer to send the message received back to the sender
	-l                        creates a listener to print the messages received
	-p, --protocol=acronym    specify the protocol being manipulated
	-s, --source=x.x.x.x      specify the source IP
	-d, --dest=x.x.x.x        specify the destination IP
	--srcport=short           specify the source port if applicable
	--dstport=short           specify the destination port if applicable
	string                    message content as a string

Current supported protocols include: TCP, UDP, ICMP

##Examples
Identify IP of currently being used interface.

	./ph -m

Spoof sender of message.

	./ph -p UDP -s 10.10.10.10 -d 120.12.14.5 --srcport=1234 --dstport=1234 "Hello World!"


##TODO
* Fix external IP printing in case of IPv6
* Allow for standard input redirection
* DNS options

##Dependencies:
* libargtable2-dev
* libpcap-dev
