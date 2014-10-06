# Checksum

## Red

	n6 (1::16) <---> j (1::1, 192.0.2.1, [192.0.2.2]) <---> n4 (192.0.2.5)

[x] es dirección de pool.

## Resumen

UDP, TCP e ICMP infos:

- Checksums correctos se traducen en checksums correctos.
- Checksums incorrectos se traducen en checksums incorrectos.

En ICMP errors es cuando estamos obligados a validar:

- Checksums correctos se traducen en checksums correctos.
- En checksums incorrectos se tira el paquete.

Fragmentación es parte de la ecuación en el código, pero debe no afectar.

Por último: En IPv4-UDP, checksum cero es diferente a checksum incorrecto. Como los paquetes generados para las pruebas nunca tienen checksum natural cero, en la mayoría de las pruebas checksum cero = checksum incorrecto. En IPv4->IPv6 UDP, sin embargo, checksum cero y checksum incorrecto necesitan pruebas por separado.

TODO +0 es ilegal en checksums de IPv6. Agregar pruebas para eso.

## Pruebas

Antes, insertar BIB entries: `jool -ba --bib6 1::5#20000 --bib4 192.0.2.2#20000`.

Wireshark debe estar configurado para validar checksums. Por comodidad, también conviene que no analice números de secuencia en TCP.

| Enviar | Verificar traducción a | con checksum |
|--------|------------------------|--------------|
| csum-IPv6-UDP-full-good.pkt | IPv4-UDP | correcto |
| csum-IPv6-UDP-full-bad.pkt | IPv4-UDP | incorrecto |
| csum-IPv6-UDP-frag1-good.pkt y csum-IPv6-UDP-frag2.pkt | IPv4-UDP x2 | correcto |
| csum-IPv6-UDP-frag1-bad.pkt y csum-IPv6-UDP-frag2.pkt | IPv4-UDP x2 | incorrecto |
| csum-IPv6-TCP-full-good.pkt | IPv4-TCP | correcto |
| csum-IPv6-TCP-full-bad.pkt | IPv4-TCP | incorrecto |
| csum-IPv6-TCP-frag1-good.pkt y csum-IPv6-TCP-frag2.pkt | IPv4-TCP x2 | correcto |
| csum-IPv6-TCP-frag1-bad.pkt y csum-IPv6-TCP-frag2.pkt | IPv4-TCP x2 | incorrecto |
| csum-IPv6-ICMPinfo-full-good.pkt | IPv4-ICMPinfo | correcto |
| csum-IPv6-ICMPinfo-full-bad.pkt | IPv4-ICMPinfo | incorrecto |
| csum-IPv6-ICMPinfo-frag1-bad.pkt y csum-IPv6-ICMPinfo-frag2.pkt | IPv4-ICMPinfo x2 | incorrecto |
| csum-IPv6-ICMPinfo-frag1-good.pkt y csum-IPv6-ICMPinfo-frag2.pkt | IPv4-ICMPinfo x2 | correcto |
| csum-IPv6-UDP-full-good.pkt* y csum-IPv6-ICMPerr-full-good.pkt | IPv4-ICMPerror | correcto |
| csum-IPv6-UDP-full-good.pkt* y csum-IPv6-ICMPerr-full-bad.pkt | Nada (paquete se ignora) | - |
| csum-IPv4-UDP-full-good.pkt | IPv6-UDP | correcto |
| csum-IPv4-UDP-full-bad.pkt | IPv6-UDP | incorrecto |
| csum-IPv4-UDP-full-zero.pkt | IPv6-UDP | correcto |
| csum-IPv4-UDP-frag1-good.pkt y csum-IPv4-UDP-frag2.pkt | IPv6-UDP x2 | correcto |
| csum-IPv4-UDP-frag1-bad.pkt y csum-IPv4-UDP-frag2.pkt | IPv6-UDP x2 | incorrecto |
| csum-IPv4-UDP-frag1-zero.pkt y csum-IPv4-UDP-frag2.pkt | IPv6-UDP x2 | correcto |
| csum-IPv4-TCP-full-good.pkt | IPv6-TCP | correcto |
| csum-IPv4-TCP-full-bad.pkt | IPv6-TCP | incorrecto |
| csum-IPv4-TCP-frag1-good.pkt y csum-IPv4-TCP-frag2.pkt | IPv6-TCP x2 | correcto |
| csum-IPv4-TCP-frag1-bad.pkt y csum-IPv4-TCP-frag2.pkt | IPv6-TCP x2 | incorrecto |
| csum-IPv4-ICMPinfo-full-good.pkt | IPv6-ICMPinfo | correcto |
| csum-IPv4-ICMPinfo-full-bad.pkt | IPv6-ICMPinfo | incorrecto |
| csum-IPv4-ICMPinfo-frag1-good.pkt y csum-IPv4-ICMPinfo-frag2.pkt | IPv6-ICMPinfo x2 | correcto |
| csum-IPv4-ICMPinfo-frag1-bad.pkt y csum-IPv4-ICMPinfo-frag2.pkt | IPv6-ICMPinfo x2 | incorrecto |
| csum-IPv4-UDP-full-good.pkt* y csum-IPv4-ICMPerr-full-good.pkt | IPv6-ICMPerror (packet filtered) | correcto |
| csum-IPv4-UDP-full-good.pkt* y csum-IPv4-ICMPerr-full-bad.pkt | Nada (paquete se ignora) | - |

* La traducción de este paquete no es importante a estas alturas; el envío sirve para que Jool genere la sesión que va a usar para traducir el siguiente error.

