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
		/* Pero y si ya venía fragmentado? */

Resumen:

	Si DF está activado, solamente incluir el FH si ya está fragmentado.
	Si DF está desactivado, solamente incluir el FH si van a resultar fragmentos (excepto si el admin configura que siempre se incluya).

(tienes que revisar que Daniel solamente use esa bandera si DF no está activado y paquete no se fragmentó.)

"the rules in Section 4.1 use the presence of an IPv6 Fragment Header to indicate that the sender might not be using path MTU discovery (i.e., the packet should not have the DF flag set should it later be translated back to IPv4)."

Insertar defrag4 aquí trae dos problemas:

1. Apagando DF, "Jool" rompe el contrato de DF fragmentando en cada ocasión. Las comillas son porque en realidad no es culpa de Jool. Quizá deberías postear comentario en algún lado que pregunte cómo no afecta eso a state.
	- Si viene un paquete no fragmentado, no afecta porque defrag4 hace nada.
	- Si viene un paquete fragmentado sin DF, no afecta porque no había contrato.
	- El problema es cuando viene paquete fragmentado con DF. Sucede que este caso no tiene mucho sentido y, como el resto del kernel lo ignora, probablemente deberíamos seguir el ejemplo.
		- TODO prueba que realmente el resto del kernel lo ignore.
			- Inserta a state.
			- Envía dos fragmentos de un paquete. Con DF activado. No necesian ser grandes.
			- ¿Tienen DF activado?
2. Se pierde la indicación de que el sender puede no estar usando Path MTU discovery. No usar PMTUD es raro, de modo que podríamos considerar esto relativamente irrelevante.
	- He estado rompiéndome la cabeza, y sigo sin entender la conexión entre que venga un fragment header y que el paquete debería tener DF apagado en el regreso. Cuando un nodo IPv6 recibe un paquete con fragment header, no necesariamente responde con un paquete con fragment header. Al menos el RFC de IPv6 dice nada al respecto.

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

