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

TODO

## --plateaus, --boostMTU

1. Insertar BIB entry: `sudo jool -bau --bib6 1::5#12345 --bib4 192.0.2.2#12345`
2. Sube artificialmente el tamaño del link de IPv6 en j: `sudo ip link set eth0 mtu 2000`
3. Enviar plateaus-boostmtu-mtu1280. Debe traducirse en un Packet too Big con MTU 1300.
3. Enviar plateaus-boostmtu-mtu0.pkt. Debe traducirse en un Packet too Big con MTU 1280.
4. `sudo jool --boostMTU false`
5. Enviar plateaus-boostmtu-mtu0.pkt. Debe traducirse en un Packet too Big con MTU 1512 (porque debe agarrar el plateau 1492).

