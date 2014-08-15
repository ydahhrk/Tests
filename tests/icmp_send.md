# IPv4

## 4. Errores

| Nombre | Tipo | Código |
|--------|------|--------|
| Protocol Unreachable | 3 | 2 |
| Dest Unreachable | 3 | |
| Filtered | 3 | 13 |
| Port Unreachable | 3 | 3 |
| Host Unreachable | 3 | 1 |
| Frag Needed | 3 | 4 |
| TTL Exceeded | 11 | 0 |
| Source Route Failed | 3 | 5 |

### 4. Determine in tuple

* Viene un paquete cuyo l4-proto no es TCP, UDP o ICMP -> Proto Unreachable
`IPv4_L4SCTP.pkt`


### 4. Filtering & updating

* No hay BIB para paquete UDP -> Dest Unreachable (el RFC no especifica código)
`nping --udp 192.0.2.1 -c 1`

* No hay BIB para paquete TCP -> Port unreachable después de 6 segundos
`nping --tcp-connect 192.0.2.1 -c 1`

* No hay BIB para ICMP info -> Host Unreachable
`ping 192.0.2.1 -c 1`

* Bloqueado por Address-Dependent Filtering (UDP, ICMP) -> Filtered
Crear bib estatica, setear address-dependent filtering, usar nping
`$JOOL -ba --bib4=192.0.2.1#8081 --bib6=1::16#80`
`$JOOL --dropAddr=true`
`nping --udp 192.0.2.1 -p 8081 -c 1`
`sudo nping --icmp 192.0.2.1 --icmp-id 8081 -c 1`

### 4. RFC 6145

* DF está activado y paquete no cabe -> Frag needed (con MTU)
`sudo nping --udp 192.0.2.1 -p 8081 --df --data-length 1400 -c 1`
	
* TTL es cero o uno -> TTL Exceeded
`sudo nping --udp 192.0.2.1 -p 8081 --ttl 0 -c 1`

* Paquete contiene una unexpired source route -> Source Route Failed
`$JOOL -ba --bib4=192.0.2.1#90 --bib6=1::16#90`
`IPv4_OPT_LooseRoute.pkt`

# IPv6

## 6. Errores

| Nombre | Tipo | Código |
|--------|------|--------|
| Port Unreachable | 1 | 4 |
| Address Unreachable | 1 | 3 |
| Hop Limit Exceeded | 3 | 0 |
| Erroneous header | 4 | 0 |
| Packet too Big | 2 | 0 |

### 6. Determine in tuple

* Viene un paquete cuyo l4-proto no es TCP, UDP o ICMP -> Port Unreachable
[Correcto* (nosotros enviamos PROTO unreacheable pero el RFC indica PORT UNREACHABLE)]
`IPv6_L4_SCTP.pkt`

### 6. Filtering & updating

* No es posible allocatear Pool4 addr o BIB (UDP, TCP) -> Address Unreachable
`jool -4f`
`nping -6 --udp 64:ff9b::1 -c 1`
`nping -6 --tcp-connect 64:ff9b::1 -c 1`

### 6. RFC 6145

* Hop Limit es cero o uno -> Hop Limit Exceeded
`IPv6_ttl1.pkt`
`IPv6_ttl0.pkt`

* Paquete tiene routing header con "a non-zero Segments Left field" -> Erroneous header. PTR debe 
indicar al primer byte del segments left.
`IPv6_ExtHdr_Routing.pkt`

* Paquete es demasiado grande -> Packet too Big (con MTU)
`IPv6_UDP_MTU1500.pkt con ethN de jool (ipv4) mtu 1400`
`IPv6_ICMP_MTU1300.pkt con ethN de jool (ipv4) mtu 1280`

# Jo..

"If an IPv4 packet is discarded, then the translator SHOULD send back a t3c13 (unless otherwise specified) ICMPv4 error, unless the discarded packet is itself an ICMPv4 message."

"If an IPv6 packet is discarded, then the translator SHOULD send back a t1c1 (unless otherwise specified) ICMPv6 error, unless the discarded packet is itself an ICMPv6 message."

