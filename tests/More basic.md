# Basic tests (informal)

	n6 (1::5) <---> j (1::1, 192.0.2.1, [192.0.2.2]) <---> n4 (192.0.2.5)

## Checksum in ICMP error

1. Compilar con Debug activado.
2. Insertar BIB entry: `sudo jool -bau --bib6 1::5#80 --bib4 192.0.2.2#80`
3. Enviar checksum-icmp6-good.pkt. Debe traducirse.
	1. IPv6 { 1::5 -> 64:ff9b::192.0.2.5 }
	2. ICMPv6 { 1, 4 }
	3. IPV6 { 64:ff9b::192.0.2.5 -> 1::5 }
	4. UDP { 1234 -> 80 }
	5. Payload { 0, 1, 2, 3 }
4. Enviar checksum-icmp6-bad.pkt. No debe traducirse y se debe loguear algo como "Checksum doesn't match".
	1. Igual a checksum-icmp6-good, excepto que IPv6hdr.checksum = 1234.
5. Enviar checksum-icmp4-good.pkt. Debe traducirse.
	1. IPv4 { 192.0.2.5 -> 192.0.2.2 }
	2. ICMPv4 { 3, 2 }
	3. IPv4 { 192.0.2.2 -> 192.0.2.5 }
	4. UDP { 80 -> 1234 }
	5. Payload { 0, 1, 2, 3 }
6. Enviar checksum-icmp4-bad.pkt. Debe traducirse.
	1. Igual a checksum-icmp6-good, excepto que IPv4hdr.checksum = 1234.

## Unknown nexthdr

1. Insertar BIB entry: `sudo jool -bau --bib6 1::5#80 --bib4 192.0.2.2#80`
2. Enviar checksum-icmp4-good.pkt.
3. Verificar que Wireshark

## Pkt sources

### checksum-icmp6-good.pkt

	-6






	1::5
	64:ff9b::192.0.2.5
	58
	1
	4
	
	/* Insertar checksum aqui si estas buildeando bad. */

	-6






	64:ff9b::192.0.2.5
	1::5
	17
	1234
	80


	0






	checksum-icmp6-good.pkt

### checksum-icmp4-good.pkt

	-4












	192.0.2.5
	192.0.2.2
	1
	3
	2

	/* Checksum aqui */

	-4












	192.0.2.2
	192.0.2.5
	17
	80
	1234


	0






	checksum-icmp4-good.pkt
