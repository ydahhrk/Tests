# defrag4

- pregunta: ¿realmente podemos asumir que todo iptables4 ve paquetes completos, simplemente porque defrag junta skbs?
	- hipótesis (probable): todo iptables4 maneja paquetes completos.
		- pregunta: ver siempre paquetes completos, ¿es un problema para el RFC?
			- hipótesis: Debido a nuestra fragmentación paranoica en out 6, probablemente la respuesta es no *en el pipeline de salida*.
			- hipótesis: La respuesta probablemente es sí en el pipeline de entrada.
				- solución (pobre): analizar los tamaños antes de defrag, guardarlos, y después de conntrack fragmentar acordemente.
					- problema: si nos envían frags pequeños se nos acaba la memoria?
					- problema: qué hacer cuando iptables droppea el paquete?
						- solución: poner un timeout.
							- problema: demasiado enrevesado para lo que lo queremos.
		- pregunta: defrag4 siempre apaga DF cuando defragmenta. ¿Cómo afecta eso al RFC?
	- <del>hipótesis (muy improbable): iptables4 no maneja paquetes completos.</del>
		- <del>pregunta: ¿cómo demonios mantiene track de conexiones en fragmentos?</del>

Testing:

- Todo iptables4 maneja paquetes completos.
- De hecho, iptables4 ve paquetes completos incluso cuando defrag4 no está insertado... wtf?

# defrag6

- pregunta: ¿realmente podemos asumir que todo iptables6 ve paquetes separados, simplemente porque Netfilter6 lo hace?
	- hipótesis: todo iptables6 maneja paquetes separados.
		- pregunta: ¿cómo trackea iptables6 las conexiones de IPv6 para fragmentos cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` no está definido?
		- <del>pregunta: ¿Es posible poner un firewall de IPv6 sin defrag6?</del>
			- Estrategia: pon una regla que niegue cierto puerto, quitando defrag6. Fragmentos hacia ese puerto que no tienen header de capa 4, ¿son bloqueados?
		- <del>pregunta: ¿por qué demonios `NET_SKBUFF_NF_DEFRAG_NEEDED` es opcional?</del> Esto no importa mucho.
		- <del>pregunta: ¿cuál es la tarea de defrag6 cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` está apagado?</del>

Testing:

defrag6 funciona diferente en kernels bajos y kernels altos. Lo más probable es que la separación entre "bajos" y "altos" se da entre los kernels 3.12 y 3.13, porque ahí es donde desaparece `skb.nfct_reasm` y cambia la implementación de `ipv6_defrag()`.

Kernels bajos (testeado 3.2):

- Todo iptables6 maneja paquetes separados.
- Cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` está activado, la única tarea de defrag6 parece ser setear `skb->nfct_reasm`.
- Cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` no está activado, no es posible compilar defrag6.

Kernels altos (testeado 3.13):

- defrag6 se comporta igual que defrag4 cuando está insertado (los skbs literalmente se juntan).
- Si defrag6 no está insertado, los módulos ven paquetes separados.
- `skb->nfct_reasm` y `NET_SKBUFF_NF_DEFRAG_NEEDED` no existen.

Es posible poner un firewall de IPv6 sin defrag.
En Linux 3.2, si se niega cierto puerto y se escriben fragmentos hacia ese puerto, ip6tables solamente bloquea al primer fragmento. Esto es independiente de si defrag6 esté activo o no.

