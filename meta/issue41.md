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
	- hipótesis (muy improbable): iptables4 no maneja paquetes completos.
		- pregunta: ¿cómo demonios mantiene track de conexiones en fragmentos?

# defrag6

- pregunta: ¿realmente podemos asumir que todo iptables6 ve paquetes separados, simplemente porque Netfilter6 lo hace?
	- hipótesis: todo iptables6 maneja paquetes separados.
		- pregunta: ¿cómo trackea iptables6 las conexiones de IPv6 para fragmentos cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` no está definido?
		- pregunta (probablemente es la misma): ¿por qué demonios `NET_SKBUFF_NF_DEFRAG_NEEDED` es opcional?
		- pregunta (probablemente es la misma): ¿cuál es la tarea de defrag6 cuando `NET_SKBUFF_NF_DEFRAG_NEEDED` está apagado?

