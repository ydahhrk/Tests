# 4 -> 6

	Si DF
		Si el paquete está fragmentado
			incluir FH
		else
			no incluir FH
	else
		Fragmentar si skb->len > minMTU6.
		Si se fragmentó
			incluir FH (obvio)
		else
			/* No me acuerdo de esta bandera. Creo que no la tenemos. */
			Si config.includeFH
				incluir FH
			else
				no incluir FH

"the rules in Section 4.1 use the presence of an IPv6 Fragment Header to indicate that the sender might not be using path MTU discovery (i.e., the packet should not have the DF flag set should it later be translated back to IPv4)."

# 6 -> 4

	Si FH
		DF = false
	Si no
		/* El RFC dice esto en la página 17. */
		// DF = true
		/* Luego dice esto en la página 25... */
		Si skb->len <= 1280
			DF = false
		else
			DF = true

No tengo idea de qué significa esto:
"To avoid the problems described in [RFC4963], it is RECOMMENDED that the translator maintain 3-tuple state for generating the IPv4 identification value."

