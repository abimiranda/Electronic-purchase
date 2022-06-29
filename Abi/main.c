/**************************
 * File:   main.c                                                           *
 * Author: Abigail Miranda                                                    *
 **************************/

#include <xc.h>
#include "confbits.h"       //no se debe de modificar este archivo para
//garantizar el funcionamiento del BOOTLOADER

#include "Tecnica1.h"       //segun el shield habilita distintas librerias

#include "ConfShield2_1.h" //cambiar para el teclado // esta viene x defecto
//el teclado utiliza los mismos pines que el entrenador pero la
//incializaci´on esta disponible en el Shield1.3 no en el 2.1.
#include "ConfShield1_3.h"
#include "lcd.h"
#include "teclado.h"
#include "EncoderIncremental.h"


#define LED1  LATDbits.LD1 // DEFINO A LOS ACTUADORES COMO SALIDAS PERO LE CAMBIO LOS NOMBRE
#define LED2  LATDbits.LD0

long int Cmil, Ccen, Cdec, Cuni; // guarda variable valor PESOS ARS COBRE HOY
long int Amil, Acen, Adec, Auni;// guarda variable valor PESOS ARS ALUMINIO HOY
long int Bmil, Bcen, Bdec, Buni;// guarda variable valor PESOS ARS BRONCE HOY
long int Pmil, Pcen, Pdec, Puni;// guarda variable valor PESOS ARS PLOMO HOY

long int dolar_hoy=98;

float cobre=10.40, aluminio=5.90, bronce=8.20, plomo=1.30; //precio USD de los metales (por Kgr))
long int cobre_hoy, aluminio_hoy, bronce_hoy, plomo_hoy; // variable para guardar el precio de los metales es AR$ (por Kgr)
long int casosMenu=0; // la mef funcionará en base al estado casosMenu. Declaración para la mef del menu

long int tiempo_ventana_peso=9, activador_tiempo_ventana_peso=0; //variables definida en segundos.
long int tiempo_parpadeo_on=1, activador_on=0; //tiempo de encendido=1seg. activador es el flag
long int tiempo_parpadeo_off=1, activador_off=0;
long int casos_parpadeo=0; //switch en base a parpadeo del led. Comienza en caso 0

float guarda_ADC, kilos, kgrs_pesado, cen_kg, dec_kg, uni_kg, gramos ; //variable para read adc
long int readADC(void);

long int cobrePrecio(void); //proptotipo funcion para guardar el valor en ARS del COBRE por Kgr
long int aluminioPrecio(void); //proptotipo funcion para guardar el valor en ARS del ALUMINIO por Kgr
long int broncePrecio(void);//proptotipo funcion para guardar el valor en ARS del BRONCE por Kgr
long int plomoPrecio(void);//proptotipo funcion para guardar el valor en ARS del PLOMO por Kgr

void descomposicion(void);
void parpadeoLed(void); //prototipo funcion para el parpadeo del led
void ventanaPesaje(void); //funcion mef para para la ventana de pesaje. Parpadeo, peso, ¿¿registro de valores.??
void mefMenu (void); //maquina de estado para navegar por el menu para elegir los metales

void main(void) {

    PicIni21(); //iniciliza entrenador del LCD

    TRISDbits.RD1 = 0; //defino que los leds son salidas
    TRISDbits.RD0 = 0;

    TIMER0_INI(); //llamo a la funcion tmimer 0
    TMR0ON = 1; //lo prendo
    TIMER1_INI();
    TMR1ON = 1;
    ei(); //habilito las interrupciones
    tecladoIni13(); //inicializo teclado y conexiones etc
    LCD_init(); //inicializo el lcd;   

    while (1) {
        
            mefMenu();
       
    }
}

void __interrupt myISR(void) {
    
    //TIMMER 0 PARA SEGUNDOS acá van todas las interrupciones en segundos.
    if (TMR0IF == 1) { //termino termino de contar el timer?
        TMR0IF = 0; //bajamos el flag
        TMR0L = 0xD2; //el timer contará 47 fosc/4 * 256 = 12032 * 0,0833us
        TMR0H = 0x48; //en total aprox 1.0022ms  casi 1ms
          //    des =~ des;        

      
         if(activador_tiempo_ventana_peso==1){ //si el flag (activador) se activó, comienzo a decrementar el tiempo
            if(tiempo_ventana_peso!=0){
                tiempo_ventana_peso--;
            }
        }
        
         if(activador_on == 1){                //si el flag (activador) se activó, comienzo a decrementar el tiempo
            if(tiempo_parpadeo_on != 0){
                tiempo_parpadeo_on--;
            }
        }
        
         if(activador_off==1){                 //si el flag (activador) se activó, comienzo a decrementar el tiempo
            if(tiempo_parpadeo_off !=0){
                tiempo_parpadeo_off--;
            }
        }
    }

    if (TMR1IF == 1) { //TIMER 1 PARA ms 
        TMR1IF = 0; //bajamos el flag
        TMR1L = 0x1B; //el timer contará 1200 fosc/4 * 1 = 12032 * 0,0833us
        TMR1H = 0xD1; //en total aprox 99.96us  casi 100us
        tic_teclado();
        tic_LCD();
        if (RBIF == 1) { //si RBIF es 1
            marca_tecla(); //llamo a la funcion marca_tecla
        }
        RBIF = 0;
        

        
    }

}

void mefMenu() {//declaro la función para la MEF corespondiente al menu. Eleccion de metal.
    switch (casosMenu) { //switch respecto a casosMenu
        case 0://primer caso(elige COBRE)
        {   
            set_CURSOR(0x00);
            msg2LCD("C $");
            
            Cmil = cobrePrecio() / 1000;
            Ccen = (cobrePrecio() % 1000) / 100;
            Cdec = ((cobrePrecio() % 1000) % 100) / 10;
            Cuni = ((cobrePrecio() % 1000) % 100) % 10;

            char2LCD(Cmil + 48);
            char2LCD(Ccen + 48);
            char2LCD(Cdec + 48);
            char2LCD(Cuni + 48);
            
            set_CURSOR(0x09);
            msg2LCD("<");
            set_CURSOR(0x40);
            msg2LCD("A $");
            
            Amil = aluminioPrecio() / 1000;
            Acen = (aluminioPrecio() % 1000) / 100;
            Adec = ((aluminioPrecio() % 1000) % 100) / 10;
            Auni = ((aluminioPrecio() % 1000) % 100) % 10;

            char2LCD(Amil + 48);
            char2LCD(Acen + 48);
            char2LCD(Adec + 48);
            char2LCD(Auni + 48);
                        
            if(tecla()==2){
                casosMenu = 1; //paso al sguiente caso
               clear_LCD();
            } 
            
            
            if (tecla() == 3) {
                casosMenu = 3;
                clear_LCD();
           }
        }
            break;

        case 1://segundo caso(elige ALUMINIO)
        {
            set_CURSOR(0x00);
            msg2LCD("C $");
            
            Cmil = cobrePrecio() / 1000;
            Ccen = (cobrePrecio() % 1000) / 100;
            Cdec = ((cobrePrecio() % 1000) % 100) / 10;
            Cuni = ((cobrePrecio() % 1000) % 100) % 10;

            char2LCD(Cmil + 48);
            char2LCD(Ccen + 48);
            char2LCD(Cdec + 48);
            char2LCD(Cuni + 48);
            
            set_CURSOR(0x40);
            msg2LCD("A $");
            
            Amil = aluminioPrecio() / 1000;
            Acen = (aluminioPrecio() % 1000) / 100;
            Adec = ((aluminioPrecio() % 1000) % 100) / 10;
            Auni = ((aluminioPrecio() % 1000) % 100) % 10;

            char2LCD(Amil + 48);
            char2LCD(Acen + 48);
            char2LCD(Adec + 48);
            char2LCD(Auni + 48);
            
             set_CURSOR(0x49);
            msg2LCD("<");
            if(tecla()==2){
                clear_LCD();
                casosMenu = 2; //paso al sguiente caso
            }
                       if (tecla() == 3) {
                           clear_LCD();
                            casosMenu = 2; //vuelvo al caso 2
                        }
           
        }
            break;
            
        case 2:
        {
            set_CURSOR(0x00);
            msg2LCD("A $");
            
            Amil = aluminioPrecio() / 1000;
            Acen = (aluminioPrecio() % 1000) / 100;
            Adec = ((aluminioPrecio() % 1000) % 100) / 10;
            Auni = ((aluminioPrecio() % 1000) % 100) % 10;

            char2LCD(Amil + 48);
            char2LCD(Acen + 48);
            char2LCD(Adec + 48);
            char2LCD(Auni + 48);
            
            set_CURSOR(0x40);
            msg2LCD("B $");
            
            Bmil = broncePrecio() / 1000;
            Bcen = (broncePrecio() % 1000) / 100;
            Bdec = ((broncePrecio() % 1000) % 100) / 10;
            Buni = ((broncePrecio() % 1000) % 100) % 10;

            char2LCD(Bmil + 48);
            char2LCD(Bcen + 48);
            char2LCD(Bdec + 48);
            char2LCD(Buni + 48);
            
            set_CURSOR(0x49);
            msg2LCD("<");
            if(tecla()==2){
                clear_LCD();
                casosMenu = 3; //paso al sguiente caso
            }
          if (tecla() == 3) {
                clear_LCD();
               casosMenu = 2; //vuelvo al caso 2
            }

            
        }
            break;
            
        case 3:
        {
            parpadeoLed(); //una vez q termina el timpo total tmb le puefdo agregar q haga el ultimo punto(llamando a la funcion)

            
                if(tecla()==1){ //si ya me encuentro en este caso y decido pesar plomo, toco la tecla 1
                                parpadeoLed(); // y hago el parpadeo
                            }
            
//            set_CURSOR(0x00);
//            msg2LCD("B $");
//            
//            Bmil = broncePrecio() / 1000;
//            Bcen = (broncePrecio() % 1000) / 100;
//            Bdec = ((broncePrecio() % 1000) % 100) / 10;
//            Buni = ((broncePrecio() % 1000) % 100) % 10;
//
//            char2LCD(Bmil + 48);
//            char2LCD(Bcen + 48);
//            char2LCD(Bdec + 48);
//            char2LCD(Buni + 48);
//            
//            set_CURSOR(0x40);
//            msg2LCD("P $");
//            
//            Pmil = plomoPrecio() / 1000;
//            Pcen = (plomoPrecio() % 1000) / 100;
//            Pdec = ((plomoPrecio() % 1000) % 100) / 10;
//            Puni = ((plomoPrecio() % 1000) % 100) % 10;
//
//            char2LCD(Pmil + 48);
//            char2LCD(Pcen + 48);
//            char2LCD(Pdec + 48);
//            char2LCD(Puni + 48);
//            
//            set_CURSOR(0x49);
//            msg2LCD("<");
            if (tecla() == 2) {
                clear_LCD();
                casosMenu = 0; //vuelvo al caso 0
            }
//            if (tecla() == 3) {
//                clear_LCD();
//                casosMenu = 2; //vuelvo al caso 2
//            }
            ventanaPesaje();
            
        }
        break;
        case 4:{
             set_CURSOR(0x00);
            msg2LCD("P $");
            
            Pmil = plomoPrecio() / 1000;
            Pcen = (plomoPrecio() % 1000) / 100;
            Pdec = ((plomoPrecio() % 1000) % 100) / 10;
            Puni = ((plomoPrecio() % 1000) % 100) % 10;

            char2LCD(Pmil + 48);
            char2LCD(Pcen + 48);
            char2LCD(Pdec + 48);
            char2LCD(Puni + 48);
            
            descomposicion();
            
            set_CURSOR(0x40);
            msg2LCD("$");
            Pmil = plomoPrecio() / 1000;
            Pcen = (plomoPrecio() % 1000) / 100;
            Pdec = ((plomoPrecio() % 1000) % 100) / 10;
            Puni = ((plomoPrecio() % 1000) % 100) % 10;

            char2LCD(Pmil + 48);
            char2LCD(Pcen + 48);
            char2LCD(Pdec + 48);
            char2LCD(Puni + 48);
            msg2LCD(".-");
            if(tecla()==1){
                casosMenu=0;
            }
        }
    }
}

long int cobrePrecio(){                 //funcion para guardar el valor del COBRE por kgr en AR$
    cobre_hoy = cobre * dolar_hoy;
    return cobre_hoy;
}

long int aluminioPrecio(){              //funcion para guardar el valor del ALUMINIO por kgr en AR$
    aluminio_hoy = aluminio * dolar_hoy;
    return aluminio_hoy;
}

long int broncePrecio(){                //funcion para guardar el valor del BRONCE por kgr en AR$
    bronce_hoy = bronce * dolar_hoy;
    return bronce_hoy;
}

long int plomoPrecio(){                 //funcion para guardar el valor del PLOMO por kgr en AR$
    plomo_hoy = plomo * dolar_hoy;
    return plomo_hoy;
}

long int readADC() { // en esta funcion guardo el valor deladc en kilos.
    guarda_ADC = obtener_ADC10(); //acá llamo a la dfuncion y guardo el valor en una variable
    kilos = (guarda_ADC * 999.9) / 1023; // para esto hago la regla de 3simples.
    //5v son 999900gr, o sea, 999.9kilos.
    //entonces si 999.9kg son 1023bits (pq ahora el conversor está en 10 bits tengo 1023 posiciones))
    // (x*999.9kg)/1023= valor de bits 
    return kilos;
}


void parpadeoLed() {//declaro la función parpadeoLed
    descomposicion();
    if (tiempo_ventana_peso != 0) { // si el tiempo no llegó a 0
        activador_tiempo_ventana_peso = 1; // activo el flag para que ahi comience el temporizador a decrementar
        switch (casos_parpadeo) { //switch respecto a casos parpadeo
            case 0://primer caso
            {
                if (tiempo_parpadeo_on != 0) { // si el tiempo de encendido del LED1 no llegó a 0

                    activador_on = 1; // activo el flag para qu recien ahi comience a contar el temporizador de ese led
                    LED1 = 1; // activo el led
                    
                }
                if (tiempo_parpadeo_on == 0) { // si llegó a 0
                    activador_on = 0; // pongo el flag en 0 para que deje de contar
                    clear_LCD(); // limpio pantalla lcd
                    casos_parpadeo = 1; // paso al siguiente caso
                }
                }
                break; 
            case 1://segundo caso
            {
                if (tiempo_parpadeo_on == 0) { // si el contador llego a 0
                    LED1 = 0; // apago el lec
                    clear_LCD(); //limpio pantalla
                    tiempo_parpadeo_on = 3; // cargo el contador a 1SEG paqra la proxima vez
                    casos_parpadeo=2; //paso al sig caso
                }
            }
                break;

            case 2:
            {
                if(tiempo_parpadeo_off!=0){ // si el temporizador del segundo led  no llego a 0
                    activador_off=1; // activo el contador de este led
                    LED2=1; // enciendo el led
           
                }
                if(tiempo_parpadeo_off==0){ // si el tiempo llegó a 0
                    activador_off=0; //apago el flag
                    casos_parpadeo=3; //paso al sig caso
                    clear_LCD(); // limpio pantalla
                }
            }
            break;

            case 3:
            {
                if (tiempo_parpadeo_off == 0) { // si el tiempo llegó a 0
                    LED2 = 0; // apago el led
                    clear_LCD(); //limpio pantalla
                    tiempo_parpadeo_off = 3; //cargo para la proxiima vez
                    casos_parpadeo=0; //vuelvo al primer caso
                }
            }
        }
    }
    if(tiempo_ventana_peso==0){ // si el contador principal de 5 seg llegó a 0
        activador_tiempo_ventana_peso=0; //entonces apago el flag para que no entre a la maquina de estados
        LED1=0; //apago los leds
        LED2=0;
        clear_LCD();
         casosMenu=4;     
    }
}

void descomposicion(){
      set_CURSOR(0xB);
            cen_kg = (readADC() % 1000) / 100;
            dec_kg = ((readADC() % 1000) % 100) / 10;
            uni_kg = ((readADC() % 1000) % 100) % 10;
                    
//            set_CURSOR(0x40);
            char2LCD(cen_kg + 48);
            char2LCD(dec_kg + 48);
            char2LCD(uni_kg + 48);
            
            msg2LCD("Kg");
   
}
