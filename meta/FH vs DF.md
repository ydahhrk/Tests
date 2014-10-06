# 4 -> 6

	Si DF
		Si el paquete está fragmentado (Frag.md#prueba-1)
			incluir FH
		else
			no incluir FH (Frag.md#prueba-2)
	else
		Fragmentar si skb->len > minMTU6. (Frag.md#prueba-3)
		Si se fragmentó
			incluir FH (obvio)
		else (Frag.md#prueba-4)
			/* No me acuerdo de esta bandera. Creo que no la tenemos. */
			Si config.includeFH 
				incluir FH
			else
				no incluir FH

"the rules in Section 4.1 use the presence of an IPv6 Fragment Header to indicate that the sender might not be using path MTU discovery (i.e., the packet should not have the DF flag set should it later be translated back to IPv4)."

# 6 -> 4

	Si FH (Frag.md#prueba-5)
		DF = false
	Si no
		si --setDF True (Frag.md#prueba-6.1)
			DF = true
		else (Frag.md#prueba-6)
			Si skb->len <= 88 || skb->len > 1280
				DF = TRUE
			else
				DF = false

El RFC parece contradecirse en dos puntos:

- En la página 17 dice "the translator SHOULD set the IPv4 Don't Fragment bit", pero en la página 18 dice "the translator MAY provide a function as follows. If (...), it sets the DF flag to zero; otherwise, it sets the DF flag to one". Supongo que el SHOULD no descarta configuraciones alternativas, de modo que definir que --setDF sea True por defecto parece satisfacer ambos requerimientos.
- En la página 18 dice "If the packet size is greater than 88 bytes and less than or equal to 1280 bytes". En la página 25 dice "If the packet is less than or equal to 1280 bytes". De momento nos vamos a pegar a la primera versión de la condición, simplemente porque parece más específica. Sin embargo, necesitamos investigarlo más a fondo.

No tengo idea de qué significa esto:
"To avoid the problems described in [RFC4963], it is RECOMMENDED that the translator maintain 3-tuple state for generating the IPv4 identification value."

