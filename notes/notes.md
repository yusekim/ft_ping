# ft_ping study note

## What is ICMP(internet control message protocol)?
- used by hosts and routers to communicate network-level information
	- error reporting: unreachable host, network, port, protocol
	- echo request/reply (ping)
- network-layer "above" IP
	- IMCP messages carried in IP datagrams, with protocol number 1
