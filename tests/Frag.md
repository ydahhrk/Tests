# Fragment Header to Flag DF
	La mayoria de (si no es que todas) las pruebas utilizan las siguientes direcciones IPs
	Host6 IPv6 = 1::16/96
	Jool IPv6 = 1::1/96, IPv4 = 192.0.2.1/24, Prefijo = 64:ff9b::/96
	Host4 IPv4 = 192.0.2.7/24

## prueba 1

	IPv6 -> jool -> IPv4

1. Un *Host6* debe enviar paquete sin **FH (Fragment Header)** hacia un *Host 4* a través de *Jool (NAT64)*

A continuacion se muestran las posibles combinaciones  
Primero setear la bandera setDF false.

`jool --setDF 0`  
Utilizar un ping6: 

| Comando | pkt6->len | pkt4->len | DF flag |
|---------|-----------|-----------|---------|
|`ping6 [64:ff9b::192.0.2.7] -s 39 -c 1`| 87 | 67 | (1) true |
|`ping6 [64:ff9b::192.0.2.7] -s 40 -c 1`| 88 | 68 | (1) true |
|`ping6 [64:ff9b::192.0.2.7] -s 41 -c 1`| 89 | 69 | (0) false |
|`ping6 [64:ff9b::192.0.2.7] -s 1000 -c 1`| 1048 | 1028 | (0) false |
|`ping6 [64:ff9b::192.0.2.7] -s 1231 -c 1`| 1279 | 1259 | (0) false |
|`ping6 [64:ff9b::192.0.2.7] -s 1232 -c 1`| 1280 | 1260 | (0) false |
|`ping6 [64:ff9b::192.0.2.7] -s 1233 -c 1`| 1281 | 1261 | (1) true |
|`ping6 [64:ff9b::192.0.2.7] -s 1333 -c 1`| 1381 | 1361 | (1) true |

si `jool --setDF 1`  
y se utiliza la misma tabla de arriba todos los DF flag deben ser true a excepcion de la siguiente prueba (Prueba 2).

## prueba 2

	IPv6 -> jool -> IPv4

1. Un *Host6* debe enviar paquete no fragmentado con **FH (Fragment Header)** hacia un *Host4* a través de *Jool (NAT64)*
2. *Jool (NAT64)* debera traducir a un paquete IPv4 con bandera **DF (Don't Fragment)** false (0).
3. El *Host4* recibira un paquete IPv4 con bandera **DF (Don't Fragment)** false (0)  

Utilizar *PacketSender* con **IPV6_MF.pkt** *(ip6 = 1::16,  ip4 = 192.0.2.7)*
	
## prueba 3* (inconclusa en more_revision)

	IPv6 -> jool -> IPv4
	| Host6 |--- MTU1 ---( X )--- MTU2 ---( Jool )--- MTU3 ---( X )--- MTU4 ---| Host4 |
	
MTU 1 = 1000  
MTU 2 = 1500  
MTU 3 = 1500  
MTU 4 = 1000  

1. *Host6* debe enviar un paquete fragmentado *(dos fragmentos (1000 y 300))*.
2. *Jool* debera recibir un solo paquete (que es el resultado de desfragmentar el paquete que envio *Host6*)
3. *Jool* debe traducir el paquete desfragmentado de *IPv6* a *IPv4* con la bandera **DF** en false (0).
4. *Jool* debe enviar un solo paquete hacia *Host4*

con nping:  
`sudo ip link set vboxnet2 mtu 1280` [*Cambiamos el MTU del enlace para que nping fragmente*]  
`sudo nping -6 --tcp 64:ff9b::192.0.2.7 -c 1 --data-length 1400 -e [interfaceIP6]`
		
## prueba 4* (Esta prueba depende que la 3 sea exitosa para que pueda seguir con el paso numero 3)

	IPv6 -> jool -> IPv4
	| Host6 |--- MTU1 ---( X )--- MTU2 ---( Jool )--- MTU3 ---( X )--- MTU4 ---| Host4 |

MTU 1 = 1000  
MTU 2 = 1500  
MTU 3 = 1000  
MTU 4 = 1500  
	
1. *Host6* debe enviar un paquete fragmentado *(dos fragmentos (1000 y 300))*.
2. *Jool* debera recibir un solo paquete (que es el resultado de desfragmentar el paquete que envio *Host6*)
3. *Jool* debe enviar un solo paquete hacia *Host4* con un tamaño aproximado de 1300 bytes, como el MTU de la *Interface* de salida hacia *IPv4* es menor (msg.lenght > MTU), El kernel de linux (Donde se encuentra alojado *Jool (NAT64)*) debera fragmentar el paquete que va a salir hacia *Host4*.
 
## prueba 5
    IPv6 <-- jool <-- IPv4

Paquete IPv4 debe tener **DF (Don't Fragment)** true (1)
Paquete IPv6 no debera contener **Fragment Header**

H4 = Host IPv4 
H6 = Host IPv6

1. En *Jool (NAT64)* se crea bib estatica correspondiente para poder comunicarnos hacia ipv6.
2. *Host4* envia un paquete a *Host6* a traves de jool
 - Las caracteristicas del paquete que se envia a *Host6* es que la bandera de **DF (Don't Fragment)** debe ser true (1)
3. *Jool (NAT64)* Recibe paquete IPv4 y lo convierte a IPv6, por medio de la BIB establece la comunicacion.
4. *Host6* Recibe paquete IPv6 de *Jool (NAT64)*, se revisa que el paquete traducido no tenga un **FH (Fragment Header)**

`jool -ba --bib4=192.0.2.1#8081 --bib6=1::16#80`  
`sudo nping --icmp 192.0.2.1 --icmp-id 8081 -df -c 1 --data-length 500`  
`sudo nping --tcp 192.0.2.1 -p 8081  -df -c 1 --data-length 400`  

## prueba 6

	IPv6 <- jool <- IPv4

Paquete IPv4 debe tener **DF (Don't Fragment)** false (0)  
Paquete IPv6 debera contener **FH (Fragment Header)**

1. En *Jool (NAT64)* se crea bib estatica correspondiente para poder comunicarnos hacia IPv6 desde IPv4.
2. *Host4* envia un paquete a *Host6* a traves de jool
 - Las caracteristicas del paquete que se envia a *Host6* es que la bandera de **DF (Don't Fragment)** debe ser false (0)
3. *Jool (NAT64)* Recibe paquete IPv4 y lo convierte a IPv6, por medio de la BIB establece la comunicacion.
4. *Host6* Recibe paquete IPv6 de *Jool (NAT64)*, se revisa que el paquete traducido contenga un **FH (Fragment Header)**.

`jool -ba --bib4=192.0.2.1#8081 --bib6=1::16#80`  
`sudo nping --icmp 192.0.2.1 --icmp-id 8081 -c 1 --data-length 500`  
`sudo nping --tcp 192.0.2.1 -p 8081 -c 1 --data-length 400`  

## prueba 7

	IPv6 <- jool <- IPv4
	
Paquete IPv4 debe tener **DF (Dont Fragment)** true (1) y ser fragmento (debe enviar los fragmentos para que sea un paquete *"completo"*)  
Paquete IPv6 debera contener **FH (Fragment Header)**


1. En *Jool (NAT64)* se crea bib estatica correspondiente para poder comunicarnos hacia IPv6 desde IPv4.  
2. *Host4* envia fragmentos (paquete) a *Host6* a traves de *Jool (NAT64)*
 - Las caracteristicas de los fragmentos que se envia a *Host6* es que la bandera **DF (Dont Fragment)** debe ser true (1).
3. *Jool (NAT64)* Recibe fragmentos IPv4, enseguida los desfragmenta y lo convierte a IPv6, por medio de la BIB establece la comunicacion.
4. *Host6* recibe paquete IPv6 de *Jool (NAT64)*, se revisa que el paquete (no fragmentos) traducido contenga un **FH (Fragment Header)**

`jool -ba --bib4=192.0.2.1#8081 --bib6=1::16#80`  
Utilizar packet sender con: `imcpv4_frag1.pkt`, `imcpv4_frag2.pkt`, `imcpv4_frag3.pkt`


## prueba 8

	IPv6 <- Jool <- IPv4
	
1. En *Jool (NAT64)* se crea bib estatica correspondiente para poder comunicarnos hacia ipv6.
2. Configurar *Jool (NAT64)* desde la *Aplicacion de usuario* para setear el minMtu a 1280 (Minimo permitido).
3. *Host4* envia un paquete a *Host6* a traves de jool
 - Las caracteristicas del paquete que se envia a *Host6* es que la bandera de **DF (Don't Fragment)** debe ser false (0) y su payload sea mayor a 1280.
3. *Jool (NAT64)* Recibe paquete IPv4 y lo convierte a Fragmentos de IPv6, por medio de la BIB establece la comunicacion.
4. *Host6* Recibe fragmentos IPv6 de *Jool (NAT64)*, se revisa que los fragmentos tenga un **FH (Fragment Header)**

`jool -ba --bib4=192.0.2.1#8081 --bib6=1::16#80`  
`jool --minMTU 1280`  
`sudo nping --icmp 192.0.2.1 --icmp-id 8081 -c 1  --data-length 1300`
