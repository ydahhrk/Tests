# Issue 41 - planeación

	netfilter6 = Solamente los hooks de Netfilter que procesan paquetes IPv6.
	netfilter4 = Solamente los hooks de Netfilter que procesan paquetes IPv4.
	iptables6 = La parte del firewall de Linux que se encarga de filtrar paquetes IPv6.
	iptables4 = La parte del firewall de Linux que se encarga de filtrar paquetes IPv4.
	defrag6 = nf_defrag_ipv6 (`sudo modprobe nf_defrag_ipv4`)
	defrag4 = nf_defrag_ipv4
	Cuando digo "RFC", me refiero al 6146.

## defrag4

### ¿Realmente podemos asumir que todo iptables4 ve paquetes completos, simplemente porque defrag4 junta skbs?

- Todo iptables4 maneja paquetes completos.
- De hecho, iptables4 ve paquetes completos incluso cuando defrag4 no está insertado... wtf?

### Ver siempre paquetes completos, ¿es un problema para el RFC?

(TODO)

Debido a nuestra fragmentación paranoica en out 6, probablemente la respuesta es no *en el pipeline de salida*. La respuesta probablemente es sí en el pipeline de entrada (debido al overrideo de DF).

- solución (pobre): analizar los tamaños antes de defrag, guardarlos, y después de conntrack fragmentar acordemente.
	- problema: si nos envían frags pequeños se nos acaba la memoria?
		- solución: meh
	- problema: qué hacer cuando iptables droppea el paquete?
		- solución: poner un timeout.
			- problema: demasiado enrevesado para lo que lo queremos.

### defrag4 siempre apaga DF cuando defragmenta. ¿Cómo afecta eso al RFC?

(TODO)

### Notas

Por alguna razón, tenía la impresión de que un firewall stateful se construye encima de un firewall stateless. En realidad son independientes. La siguiente regla no requiere a defrag4:

	iptables -A INPUT -d 192.0.2.5 -j REJECT

La siguiente regla sí lo requiere:

	iptables -A OUTPUT -p tcp -m state --state NEW,ESTABLISHED -j ACCEPT

("state" está definido en `xt_state.c`. Lo único que hace es observar las banderas seteadas por conntrack. Ese conntrack está definido en `nf_conntrack_l3proto_ipv4.c` y contiene un `nf_defrag_ipv4_enable()`. Ver `ipv4_conntrack_in()` > `nf_conntrack_in` > `resolve_normal_ct` > `init_conntrack` > `__nf_conntrack_alloc`)

Lo cual puede significar que quizá podemos enlazar a la parte stateless de iptables4 a Jool sin problemas.

TODO Sucede que la parte "Filtering" del paso "Filtering and Updating" de la definición de stateful NAT64 es bastante similar a lo que hace un firewall stateful. Sin embargo, "iptables stateful" es capaz de más de lo que el RFC pide.

TODO Linux piensa que las direcciones de la pool de IPv4 le pertenecen. iptables va a armar un escándalo por esto?

## defrag6

### ¿Realmente podemos asumir que todo iptables6 ve paquetes separados, simplemente porque Netfilter6 lo hace?

defrag6 funciona diferente en kernels bajos y kernels altos. Lo más probable es que la separación entre "bajos" y "altos" se da entre los kernels 3.12 y 3.13, porque ahí es donde desaparece `skb.nfct_reasm` y cambia la implementación de `ipv6_defrag()`.

Kernels bajos (testeado 3.2):

- Todo iptables6 maneja paquetes separados.
- Cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` está activado, la única tarea de defrag6 parece ser setear `skb->nfct_reasm`.
- Cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` no está activado, no es posible compilar defrag6.

Kernels altos (testeado 3.13):

- defrag6 se comporta igual que defrag4 cuando está insertado (los skbs literalmente se juntan).
- Si defrag6 no está insertado, los módulos ven paquetes separados.
- `skb->nfct_reasm` y `NET_SKBUFF_NF_DEFRAG_NEEDED` no existen.

### ¿Cuál es la tarea de defrag6 cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` está apagado?

`NET_SKBUFF_NF_DEFRAG_NEEDED` dicta si defrag6 se va a compilar o no...

### ¿Cómo trackea iptables6 las conexiones de IPv6 para fragmentos cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` no está definido?

defrag6 es dependencia obligatoria de conntrack6. Por lo tanto, iptables6 no trackea conexiones de IPv6 cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` no está definido.

### ¿Es posible poner un firewall de IPv6 sin defrag6?

Es posible poner un firewall stateless de IPv6 sin defrag. Bloquear por direcciones IP funciona como es esperado. Si bloqueo por puerto y envío un paquete UDP fragmentado, esto es lo que se bloquea dadas las condiciones mostradas:

|            | Con defrag6   | Sin defrag6   |
|------------|---------------|---------------|
| Linux 3.2  | 1er fragmento | 1er fragmento |
| Linux 3.13 | Todo          | 1er fragmento |

El resto de los fragmentos pasa sin problemas.

No es posible poner un firewall _stateful_ sin defrag6.

### ¿Por qué demonios `NET_SKBUFF_NF_DEFRAG_NEEDED` es opcional?

Creo que esto ya no nos importa mucho.

### defrag6 en kernels altos elimina el fragment header. ¿Cómo afecta eso al RFC?

TODO

### Notas

TODO me falta considerar que Linux no tiene conocimiento del prefijo de IPv6. ¿Esto afecta? (Probablemente no, porque estamos antes de in routing y `ip6_route_input()` no está exportado.)
