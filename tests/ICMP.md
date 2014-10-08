# ICMP

	 n6 (1::5) <---> j (1::1, 192.0.2.1, [192.0.2.2]) <---> n4 (192.0.2.5)

[x] es dirección de pool.

## Truncado de errores

La idea es probar que Jool trunque, no fragmente, ICMP errors. Solamente es de 6 a 4, porque me parece que de 4 a 6 nunca crecen.

1. Subir la siguiente BIB entry (esto fuerza a que se use puerto 5000):
	1. `jool -bau --bib6 1::5#5000 --bib4 192.0.2.2#5000`
2. Enviar el paquete ICMPtruncatePrev.pkt (esto genera la sesión que va a usar el siguiente paquete).
3. Antes de que caduque la sesión, enviar el paquete ICMPtruncate.pkt.
	1. IPv4 { 192.0.2.5 -> 192.0.2.2 }
	2. ICMP { 3, 3 } (Destination unreachable, Port unreachable)
	3. Payload[1472]
		1. IPv4 { 192.0.2.2 -> 192.0.2.5 }
		2. UDP { 5000 -> 6000 }
		3. Payload[1444] { garbage }
4. Se debe traducir en el siguiente paquete:
	1. IPv6 { 64:ff9b::192.0.2.5 -> 1::5 }
	2. ICMP { 1, 4 }
	3. Payload[1452]
		1. IPv6 { 1::5 -> 64:ff9b::192.0.2.5 }
		2. UDP { 5000 -> 6000 }
		3. Payload[1404] { mismo garbage, truncado 40 bytes al final. }

