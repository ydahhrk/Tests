# Fragment Header to Flag DF

## prueba 1

	IPv6 -> jool -> IPv4

1. Enviar paquete pequeño sin FH
2. Debera traducir a un paquete IPv4 con bandera DF true (1)
	Utilizar un ping request: `ping6 [64:ff9b::ipv4.address]`

## prueba 2

	IPv6 -> jool -> IPv4

1. Enviar paquete no fragmentado con FH
2. Debera traducir a un paquete IPv4 con bandera DF false (0)
	Utilizar packet Sender con IPV6_MF.pkt (ip6 = 1::16  ip4 = 192.0.2.7)
	
## prueba 3* (inconclusa en more_revision)

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
		
## prueba 4

	IPv6 -> jool -> IPv4
	| Host6 |--- MTU1 ---( X )--- MTU2 ---( Jool )--- mtu3 ---( X )--- MTU4 ---| Host4 |

MTU 1 = 1000
MTU 2 = 1500
MTU 3 = 1000
MTU 4 = 1500
	
Cuando el paquete IPv6 llege a jool, jool debera defragmentarlo, y cuando salga hacia ipv4 el kernel
 de jool debe fragmentarlo (???)
 
## prueba 5
    IPv6 <-- jool <-- IPv4

Paquete IPv4 debe tener **DF** true (1)
Paquete IPv6 no debera contener **Fragment Header**

H4 = Host IPv4 
H6 = Host IPv6

1. En Jool se crea bib estatica correspondiente para poder comunicarnos hacia ipv6.
2. H4 envia un paquete a H6 a traves de jool
 - Las caracteristicas del paquete que se envia a H6 es que la bandera de **DF** debe ser 1
3. Jool (NAT64) Recibe paquete IPv4 y lo convierte a IPv6, por medio de la BIB establece la comunicacion.
4. H6 Recibe paquete IPv6 de Jool (NAT64), se revisa que el paquete traducido no tenga un **Fragment Header**

`jool -ba --bib4=192.0.2.1#8081 --bib6=1::16#80` 
`sudo nping --icmp 192.0.2.1 --icmp-id 8081 -df -c 1 --data-length 500` 
`sudo nping --tcp 192.0.2.1 -p 8081  -df -c 1 --data-length 400` 


## prueba 6

	IPv6 <- jool <- IPv4

Paquete IPv4 debe tener DF false (0)
Paquete IPv6 debera contener Fragment Header

1. Crear bib estatica correspondiente para poder comunicarnos hacia ipv6
2. Enviar paquete hacia ipv6

`jool -ba --bib4=192.0.2.1#8081 --bib6=1::16#80`
`sudo nping --icmp 192.0.2.1 --icmp-id 8081 -c 1 --data-length 500`
`sudo nping --tcp 192.0.2.1 -p 8081 -c 1 --data-length 400`

