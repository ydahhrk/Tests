# Pruebas

## ICMP

	 n6 (1::5) <---> j (1::1, 192.0.2.1, [192.0.2.2]) <---> n4 (192.0.2.5)

### Truncado de errores

La idea es probar que Jool trunque, no fragmente, ICMP errors. Solamente es de 6 a 4, porque me parece que de 4 a 6 nunca crecen.

1. Subir la siguiente BIB entry:
	1. `jool -bau --bib6 1::5#5000 --bib4 192.0.2.2#5000`
2. Enviar el siguiente paquete:
	1. IPv4 { 192.0.2.5 -> 192.0.2.2 }
	2. ICMP { 3, 3 } (Destination unreachable, Port unreachable)
	3. Payload[1472]
		1. IPv4 { 192.0.2.2 -> 192.0.2.5 }
		2. UDP { 5000 -> 6000 }
		3. Payload[1444] { garbage }
3. Se debe traducir en el siguiente paquete:
	1. IPv6 { 64:ff9b::192.0.2.5 -> 1::5 }
	2. ICMP { 1, 4 }
	3. Payload[1452]
		1. IPv6 { 1::5 -> 64:ff9b::192.0.2.5 }
		2. UDP { 5000 -> 6000 }
		3. Payload[1404] { mismo garbage, truncado }

## Simultaneous Open of TCP Connections

	 n6 (1::16) <---> j (1::1, 192.0.2.1, [192.0.2.2]) <---> n4 (192.0.2.5)

[x] es dirección de pool.

Hay tres paquetes en esta prueba.

1. SO4.pkt
	1. IPv4 { 192.0.2.5 -> 192.0.2.2 }
	2. TCP { 20000 -> 10000, SYN }
	3. Payload { }
2. SO6.pkt
	1. IPv6 { 1::16 -> 64:ff9b::192.0.2.5 }
	2. TCP { 10000 -> 20000, SYN }
	3. Payload { }
3. SOHP.pkt
	1. IPv6 { 1::16 -> 64:ff9b::192.0.2.2 }
	2. TCP { 20000 -> 10000, SYN }
	3. Payload { }
4. SO-overflow.pkt
	1. IPv4 { 192.0.2.5 -> 192.0.2.2 }
	2. TCP { 20001 -> 10001 , SYN }
	3. Payload { }

### Sin SO

1. Enviar SO4.pkt desde _n4_.
2. Después de 6 segundos se debe observar un port unreachable de _j_ hacia _n4_.

### Con SO

1. En `mod/pool4.c`, función pool4_register(), hacer que cada pool tenga un solo puerto por rango en TCP. El puerto del rango mayor debe ser 10000:
	1. `error = poolnum_init(&node->tcp_ports.low, 0, 0, 1);`
	2. `error = poolnum_init(&node->tcp_ports.high, 10000, 10000, 1);`
2. Recompilar y reinsertar.
3. Enviar SO4.pkt desde _n4_.
4. Antes de que transcurran 6 segundos, enviar SO6.pkt desde _n6_.
5. SO6 debe ser traducido, SO4 olvidado, y el port unreachable se debe cancelar.

### DB overflow

1. Bajar a 2 el número de paquetes storeables: `jool --maxStoredPkts 2`
2. Enviar SO4.pkt desde _n4_.
3. Antes de que transcurran 6 segundos, enviar SO4-overflow.pkt desde _n4_.
4. Inmediatamente al ocurrir el paso anterior debe observarse un port unreachable de _j_ a _n4_.
5. Transcurridos 6 segundos del paso 2 debe observarse otro port unreachable de _j_ a _n4_.

### SO & Hairpinning

1. Repetir paso 1 de [Con SO](#con-so) (ie. modificar `mod/pool4.c`).
2. Enviar SOHP desde _n6_.
3. Después de 6 segundos se debe observar un port unreachable de _j_ hacia _n6_.

## Quick

	n6 (1::16) <---> j (1::1, 192.0.2.1, [192.0.2.2]) <---> n4 (192.0.2.5)

### Pool6

1. Extender la duración de las sesiones de UDP: `jool --toUDP 900000`
2. Crear comunicación entre n6 y n4.
3. Quitar el prefijo con `--quick`: `jool -6rq --prefix 64:ff9b::/96`
4. Enviar un paquete de n4 a n6. Jool debe dejar de traducir en el paso 3.
5. Enviar un paquete de n6 a n4. Jool debe no atajar el paquete.
6. Volver a insertar el prefijo: `jool -6a --prefix 64:ff9b::/96`.
7. Verificar que la comunicación entre n6 y n4 se haya reestablecido, y que esta nueva comunicación no genere BIBs o sesiones nuevas.

### Pool4

1. Extender la duración de las sesiones de UDP: `jool --toUDP 900000`
2. Crear comunicación entre n6 y n4.
3. Quitar la dirección con `--quick`: `jool -4rq --address 192.0.2.2`
4. Enviar un paquete de n4 a n6. Jool debe no atajar el paquete.
5. Enviar un paquete de n6 a n4. El paquete debe pasar.
6. Volver a insertar la dirección: `jool -4a --address 192.0.2.2`.
7. Verificar que la comunicación entre n6 y n4 se haya reestablecido, y que esta nueva comunicación no genere BIBs o sesiones nuevas.

## Fragment Header to Flag DF

### prueba 1

	IPv6 -> jool -> IPv4

1. Enviar paquete pequeño sin FH
2. Debera traducir a un paquete IPv4 con bandera DF true (1)
	Utilizar un ping request: `ping6 [64:ff9b::ipv4.address]`

### prueba 2

	IPv6 -> jool -> IPv4

1. Enviar paquete no fragmentado con FH
2. Debera traducir a un paquete IPv4 con bandera DF false (0)
	Utilizar packet Sender con IPV6_MF.pkt (ip6 = 1::16  ip4 = 192.0.2.7)
	
### prueba 3* (inconclusa en more_revision)

	IPv6 -> jool -> IPv4
	| Host6 |--- MTU1 ---( X )--- MTU2 ---( Jool )--- mtu3 ---( X )--- MTU4 ---| Host4 |

MTU 1 = 1000
MTU 2 = 1500
MTU 3 = 1500
MTU 4 = 1000

1. Enviar dos fragmentos (1000 y 300).
2. DF debe estar apagado (0)
3. Jool debe enviar un solo paquete 

con nping:
`sudo ip link set vboxnet2 mtu 1280` 
`sudo nping -6 --tcp 64:ff9b::192.0.2.7 -c 1 --data-length 1400 -e vboxnet2`
		
### prueba 4

	IPv6 -> jool -> IPv4
	| Host6 |--- MTU1 ---( X )--- MTU2 ---( Jool )--- mtu3 ---( X )--- MTU4 ---| Host4 |

MTU 1 = 1000
MTU 2 = 1500
MTU 3 = 1000
MTU 4 = 1500
	
Cuando el paquete IPv6 llege a jool, jool debera defragmentarlo, y cuando salga hacia ipv4 el kernel
 de jool debe fragmentarlo (???)
 
### prueba 5

	IPv6 <- jool <- IPv4

Paquete IPv4 debe tener DF true (1)
Paquete IPv6 no debera contener Fragment Header

1. Crear bib estatica correspondiente para poder comunicarnos hacia ipv6
2. Enviar paquete hacia ipv6

`jool -ba --bib4=192.0.2.1#8081 --bib6=1::16#80`
`sudo nping --icmp 192.0.2.1 --icmp-id 8081 -df -c 1 --data-length 500`
`sudo nping --tcp 192.0.2.1 -p 8081  -df -c 1 --data-length 400`

### prueba 6

	IPv6 <- jool <- IPv4

Paquete IPv4 debe tener DF false (0)
Paquete IPv6 debera contener Fragment Header

1. Crear bib estatica correspondiente para poder comunicarnos hacia ipv6
2. Enviar paquete hacia ipv6

`jool -ba --bib4=192.0.2.1#8081 --bib6=1::16#80`
`sudo nping --icmp 192.0.2.1 --icmp-id 8081 -c 1 --data-length 500`
`sudo nping --tcp 192.0.2.1 -p 8081 -c 1 --data-length 400`

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

If an IPv4 packet is discarded, then the translator SHOULD send back a t3c13 (unless otherwise specified) ICMPv4 error, unless the discarded packet is itself an ICMPv4 message.

If an IPv6 packet is discarded, then the translator SHOULD send back a t1c1 (unless otherwise specified) ICMPv6 error, unless the discarded packet is itself an ICMPv6 message.
