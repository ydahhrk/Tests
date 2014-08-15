# Quick

	n6 (1::16) <---> j (1::1, 192.0.2.1, [192.0.2.2]) <---> n4 (192.0.2.5)

## Pool6

1. Extender la duración de las sesiones de UDP: `jool --toUDP 900000`
2. Crear comunicación entre n6 y n4.
3. Quitar el prefijo con `--quick`: `jool -6rq --prefix 64:ff9b::/96`
4. Enviar un paquete de n4 a n6. Jool debe dejar de traducir en el paso 3.
5. Enviar un paquete de n6 a n4. Jool debe no atajar el paquete.
6. Volver a insertar el prefijo: `jool -6a --prefix 64:ff9b::/96`.
7. Verificar que la comunicación entre n6 y n4 se haya reestablecido, y que esta nueva comunicación no genere BIBs o sesiones nuevas.

## Pool4

1. Extender la duración de las sesiones de UDP: `jool --toUDP 900000`
2. Crear comunicación entre n6 y n4.
3. Quitar la dirección con `--quick`: `jool -4rq --address 192.0.2.2`
4. Enviar un paquete de n4 a n6. Jool debe no atajar el paquete.
5. Enviar un paquete de n6 a n4. El paquete debe pasar.
6. Volver a insertar la dirección: `jool -4a --address 192.0.2.2`.
7. Verificar que la comunicación entre n6 y n4 se haya reestablecido, y que esta nueva comunicación no genere BIBs o sesiones nuevas.

