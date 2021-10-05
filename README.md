# Electronic Purchase
# Code by Abigail Miranda
# PIC 18F4550, teclado matricial, conversor AD, leds y display LCD.

Funcionamiento:

1- Al encender el equipo, se muestra en el display LCD: 1U$S = 98$. Con TECLA1 se implementará con el pulsador BOT1.

2- Con las teclas 2 y 3 se puede navegar en el display LCD hacia arriba con TECLA2 y hacia abajo con TECLA3 y
se deberá elegir un metal con TECLA1 entre, Bronce, Cobre, Aluminio y Plomo. Entonces se deberá mostrar la 
letra correspondiente del metal y la cotización en pesos por kilogramo del día. Para el cobre ejemplo: 
C1530,25$

3- Para pesar el metal presionar TECLA1 nuevamente, operando de la siguiente forma:
Durante 5 segundos se abre la ventana de pesaje, un led parpadea durante este proceso. Se tomará el valor que entrega el convdersor AD
Al finalizar el tiempo, el led quedará encendido y se mostrará en el display el peso en Kgr. Por ejemplo 
(187,3K) (La balanza tiene una capacidad máxima de 999,9 Kilogramos y su resolución de 100 gr.)
Para tener máxima resolución en la medición se utilizará el ADC con palabra de 10 bits.

Una vez leído el peso se deberá presionar TECLA1. Se deberá mostrar en la primera línea, la letra 
correspondiente al metal, el costo por Kgr en pesos, el peso y en la línea de abajo el precio total a pagar. Para
evitar el cambio en monedas, el precio total se redondeará sin centavos. Por ejemplo; si la cuenta es 
$15149,76 el valor mostrado en el display será:

C$1530,25    009,9K

$15150.- 
