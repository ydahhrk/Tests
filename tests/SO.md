# Simultaneous Open of TCP Connections

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

## Sin SO

1. Enviar SO4.pkt desde _n4_.
2. Después de 6 segundos se debe observar un port unreachable de _j_ hacia _n4_.

## Con SO

1. En `mod/pool4.c`, función pool4_register(), hacer que cada pool tenga un solo puerto por rango en TCP. El puerto del rango mayor debe ser 10000:
	1. `error = poolnum_init(&node->tcp_ports.low, 0, 0, 1);`
	2. `error = poolnum_init(&node->tcp_ports.high, 10000, 10000, 1);`
2. Recompilar y reinsertar.
3. Enviar SO4.pkt desde _n4_.
4. Antes de que transcurran 6 segundos, enviar SO6.pkt desde _n6_.
5. SO6 debe ser traducido, SO4 olvidado, y el port unreachable se debe cancelar.

## DB overflow

1. Bajar a 2 el número de paquetes storeables: `jool --maxStoredPkts 2`
2. Enviar SO4.pkt desde _n4_.
3. Antes de que transcurran 6 segundos, enviar SO4-overflow.pkt desde _n4_.
4. Inmediatamente al ocurrir el paso anterior debe observarse un port unreachable de _j_ a _n4_.
5. Transcurridos 6 segundos del paso 2 debe observarse otro port unreachable de _j_ a _n4_.

## SO & Hairpinning

1. Repetir paso 1 de [Con SO](#con-so) (ie. modificar `mod/pool4.c`).
2. Enviar SOHP desde _n6_.
3. Después de 6 segundos se debe observar un port unreachable de _j_ hacia _n6_.
