### NAME

```
ping — send ICMP ECHO_REQUEST packets to network hosts
```

---

### SYNOPSIS

```
ping [option ...] host ...
```

---

### DESCRIPTION

```
ping uses the ICMP protocol's mandatory ECHO_REQUEST datagram to elicit
an ICMP ECHO_RESPONSE from a host or gateway.  ECHO_REQUEST datagrams
("pings") have an IP and ICMP header, followed by a “struct timeval” and
then an arbitrary number of "pad" bytes used to fill out the packet.
```

---

### OPTIONS

```
--address
        Send ICMP_ADDRESS packets (root only).

--mask
        Same as --address.

--echo
        Send ICMP_ECHO packets (default).

--timestamp
        Send ICMP_TIMESTAMP packets.

-t, --type type
        Send type packets.

-c, --count count
        Stop after sending (and receiving) count ECHO_RESPONSE packets.

-d, --debug
        Set the SO_DEBUG option on the socket being used.

-f, --flood
        Flood ping.  Outputs packets as fast as they come back or one
        hundred times per second, whichever is more.  For every
        ECHO_REQUEST sent a period "." is printed, while for every
        ECHO_REPLY received a backspace is printed.  This provides a
        rapid display of how many packets are being dropped.  Only the
        super-user may use this option.  This can be very hard on a net‐
        work and should be used with caution.

-i, --interval wait
        Wait wait seconds between sending each packet.  The default is to
        wait for one second between each packet.  This option is incom‐
        patible with the -f option.

-l, --preload preload
        If preload is specified, ping sends that many packets as fast as
        possible before falling into its normal mode of behavior.

-n, --numeric
        Numeric output only.  No attempt will be made to lookup symbolic
        names for host addresses.

-p, --pattern pattern
        You may specify up to 16 "pad" bytes to fill out the packet you
        send.  This is useful for diagnosing data-dependent problems in a
        network.  For example, “-p ff” will cause the sent packet to be
        filled with all ones.

-q, --quiet
        Quiet output.  Nothing is displayed except the summary lines at
        startup time and when finished.

-R, --route
        Record route.  Includes the RECORD_ROUTE option in the
        ECHO_REQUEST packet and displays the route buffer on returned
        packets.  Note that the IP header is only large enough for nine
        such routes.  Many hosts ignore or discard this option.

-r, --ignore-routing
        Bypass the normal routing tables and send directly to a host on
        an attached network.  If the host is not on a directly-attached
        network, an error is returned.  This option can be used to ping a
        local host through an interface that has no route through it
        (e.g., after the interface was dropped by routed(8)).

-s, --size packetsize
        Specifies the number of data bytes to be sent.  The default is
        56, which translates into 64 ICMP data bytes when combined with
        the 8 bytes of ICMP header data.

-v, --verbose
        Verbose output.  ICMP packets other than ECHO_RESPONSE that are
        received are listed.

--ttl N
        Set N as the packet time-to-live.

-T, --tos num
        Set num as the packet type of service (TOS).

-w, --timeout N
        Stop after N seconds of sending packets.

-W, --linger N
        Number of seconds to wait for response.

--ip-timestamp flag
        IP timestamp of type flag, which is one of "tsonly" and "tsaddr".

-?, --help
        Display a help list.

--usage
        Display a short usage message.

-V, --version
        Print the program version.
```

---

### ADDITIONAL DESCRIPTION

```
When using ping for fault isolation, it should first be run on the local
host, to verify that the local network interface is up and running.
Then, hosts and gateways further and further away should be "pinged".
Round-trip times and packet loss statistics are computed.  If duplicate
packets are received, they are not included in the packet loss calcula‐
tion, although the round trip time of these packets is used in calculat‐
ing the minimum/average/maximum round-trip time numbers.  When the speci‐
fied number of packets have been sent (and received) or if the program is
terminated with a SIGINT, a brief summary is displayed.

This program is intended for use in network testing, measurement and man‐
agement.  Because of the load it can impose on the network, it is unwise
to use ping during normal operations or from automated scripts.
```

---

### ICMP PACKET DETAILS

```
An IP header without options is 20 bytes.  An ICMP ECHO_REQUEST packet
contains an additional 8 bytes worth of ICMP header followed by an arbi‐
trary amount of data.  When a packetsize is given, this indicated the
size of this extra piece of data (the default is 56).  Thus the amount of
data received inside of an IP packet of type ICMP ECHO_REPLY will always
be 8 bytes more than the requested data space (the ICMP header).

If the data space is at least eight bytes large, ping uses the first
eight bytes of this space to include a timestamp which it uses in the
computation of round trip times.  If less than eight bytes of pad are
specified, no round trip times are given.
```

---

### DUPLICATE AND DAMAGED PACKETS

```
ping will report duplicate and damaged packets.  Duplicate packets should
never occur, and seem to be caused by inappropriate link-level retrans‐
missions.  Duplicates may occur in many situations and are rarely (if
ever) a good sign, although the presence of low levels of duplicates may
not always be cause for alarm.

Damaged packets are obviously serious cause for alarm and often indicate
broken hardware somewhere in the ping packet's path (in the network or in
the hosts).
```

---

### TRYING DIFFERENT DATA PATTERNS

```
The (inter)network layer should never treat packets differently depending
on the data contained in the data portion.  Unfortunately, data-dependent
problems have been known to sneak into networks and remain undetected for
long periods of time.  In many cases the particular pattern that will
have problems is something that doesn't have sufficient "transitions",
such as all ones or all zeros, or a pattern right at the edge, such as
almost all zeros.  It isn't necessarily enough to specify a data pattern
of all zeros (for example) on the command line because the pattern that
is of interest is at the data link level, and the relationship between
what you type and what the controllers transmit can be complicated.

This means that if you have a data-dependent problem you will probably
have to do a lot of testing to find it.  If you are lucky, you may manage
to find a file that either can't be sent across your network or that
takes much longer to transfer than other similar length files.  You can
then examine this file for repeated patterns that you can test using the
-p option of ping.
```

---

### TTL DETAILS

```
The TTL value of an IP packet represents the maximum number of IP routers
that the packet can go through before being thrown away.  In current
practice you can expect each router in the Internet to decrement the TTL
field by exactly one.

The TCP/IP specification states that the TTL field for TCP packets should
be set to 60, but many systems use smaller values (4.3 BSD uses 30, 4.2
used 15).

The maximum possible value of this field is 255, and most Unix systems
set the TTL field of ICMP ECHO_REQUEST packets to 255.  This is why you
will find you can "ping" some hosts, but not reach them with telnet(1) or
ftp(1).

In normal operation ping prints the ttl value from the packet it re‐
ceives.  When a remote system receives a ping packet, it can do one of
three things with the TTL field in its response:

•     Not change it; this is what Berkeley Unix systems did before the
      4.3BSD-Tahoe release.  In this case the TTL value in the received
      packet will be 255 minus the number of routers in the round-trip
      path.

•     Set it to 255; this is what current Berkeley Unix systems do.  In
      this case the TTL value in the received packet will be 255 minus the
      number of routers in the path from the remote system to the pinging
      host.

•     Set it to some other value.  Some machines use the same value for
      ICMP packets that they use for TCP packets, for example either 30 or
      60.  Others may use completely wild values.
```

---

### BUGS

```
Many Hosts and Gateways ignore the RECORD_ROUTE option.

The maximum IP header length is too small for options like RECORD_ROUTE
to be completely useful.  There's not much that that can be done about
this, however.

Flood pinging is not recommended in general, and flood pinging the broad‐
cast address should only be done under very controlled conditions.
```

---

### SEE ALSO

```
netstat(1), ifconfig(1), routed(8)
```

---

### HISTORY

```
The ping command appeared in 4.3BSD.
```

---

GNU Network Utilities	       February 9, 2019		 GNU Network Utilities

