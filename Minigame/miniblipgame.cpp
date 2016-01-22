// Minijuego Miniblip
// El jugador es el punto blanco, que se mueve hacia abajo (o vuelve arriba del todo si esta en la ultima fila) 
// pulsando el boton. Los puntos rojos son obstaculos que muestran una X roja con un pitido al chocar, los puntos 
// verdes aumentan la puntuacion (que aparece como puntos verdes en la ultima fila) y los amarillos quitan un punto.
// Al llegar a 5 puntos se muestra un cuadrado verde con pitidos y despues se resetea la puntuacion.

//Silvia Barbero Rodriguez, 22 de enero de 2016

#include "mbed.h"
#include "neopixel.h"
#include "USBSerial.h"

// Matrix led output pin
#define MATRIX_PIN P0_9
#define NLEDS 25

unsigned int counter = 0;   
USBSerial serial;

DigitalIn pushbutton(P0_23); //Define pushbutton

neopixel::Pixel buffer[NLEDS];

neopixel::PixelArray array(MATRIX_PIN);

void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) {
  if(x < 0 || x > 4 || y < 0 || y > 4) return;
  int posicion=x+y*5;
  buffer[posicion].red=red;
  buffer[posicion].green=green;
  buffer[posicion].blue=blue;
}

void updateLEDs() {
    array.update(buffer, NLEDS);
}

void clearPixels() {
    for(int x=0; x<5; x++)
        for(int y=0; y<5; y++)
            setPixel(x,y, 0,0,0);
}

void obstacle(int x, int y){
    setPixel(x,y,20,0,0);
}

void coolthing(int x, int y){
    setPixel(x,y,0,20,0);    
}

void yellow(int x, int y){
    setPixel(x, y, 20,20,0);    
}

void win(){
    clearPixels();
    setPixel(1,1,0,20,0);
    setPixel(1,2,0,20,0);
    setPixel(1,3,0,20,0);
    setPixel(2,1,0,20,0);
    setPixel(2,3,0,20,0);
    setPixel(3,1,0,20,0);
    setPixel(3,2,0,20,0);
    setPixel(3,3,0,20,0);
    updateLEDs();
    wait(2);
    clearPixels();
}
    
void drawPoints(int p){
    int i;
    for(i=0;i<=p;i++){
        setPixel(i,4,0,20,0);   
    } 
}

void drawX(){
    clearPixels();
    setPixel(0,0,20,0,0);
    setPixel(1,1,20,0,0);
    setPixel(2,2,20,0,0);
    setPixel(3,3,20,0,0);
    setPixel(4,4,20,0,0);
    setPixel(0,4,20,0,0);
    setPixel(1,3,20,0,0);
    setPixel(3,1,20,0,0);
    setPixel(4,0,20,0,0); 
    updateLEDs();
    wait(1);
    clearPixels();   
}


PwmOut speaker(P0_8);

int main()
{
    // Apagar el zumbador
    speaker=0.0;
    
    // setPixel(x,y, r,g,b); // valor máximo RGB: 255
    
    // Poner todos los LED en blanco gradualmente
    for(int x=0; x<5; x++) {
        for(int y=0; y<5; y++) {
            setPixel(x,y, 20,20,20);
            updateLEDs();
            wait_ms(100);
        }
    }
    
    clearPixels(); // Poner a 0 la matriz actual
    
//Dibuja una estrella
    setPixel(2, 0, 0,0,20);
    setPixel(2,1, 0,0,20);
    setPixel(2,2, 0,0,20);
    setPixel(2,3, 0,0,20);
    setPixel(2,4, 0,0,20);
    setPixel(0,2, 0,0,20);
    setPixel(1,2, 0,0,20);
    setPixel(2,2, 0,0,20);
    setPixel(3,2, 0,0,20);
    setPixel(4,2, 0,0,20);
    setPixel(1,1, 20,20,20);
    setPixel(1,3, 20,20,20);
    setPixel(3,1, 20,20,20);
    setPixel(3,3, 20,20,20);
    updateLEDs();
    
    
srand (time(NULL));

int playery=2;
int y=2;
int x=0;
int i, flag, points=-1;
int cooly=3, coolx=2;
int yellowx=4, yellowy=0;
    while(1) {
        //En cuanto se pulsa el boton empieza el juego
        if (pushbutton) {
            clearPixels();
            while(1){
                flag=0;
                setPixel(3,playery, 20,20,20);
                obstacle(x,y);
                coolthing(coolx, cooly);
                yellow(yellowx,yellowy);
                drawPoints(points);
                updateLEDs();
                for(i=0;i<=5; i++){
                    if(pushbutton){
                        flag=1;
                    }
                    wait(0.1);
                }
                if(flag==1){
                    if(playery==3){
                        playery=0;    
                    }else{
                        playery+=1;    
                    }
                }
                if(y==playery && x==3){
                    speaker=1;
                    drawX();   
                    speaker=0;
                }
                if(cooly==playery && coolx==3){
                    points+=1;  
                }
                if(yellowy==playery && yellowx==3){
                    points-=1;    
                }
                if(points==4){
                    speaker=1;
                    wait(0.2);
                    speaker=0;
                    wait(0.2);
                    speaker=1;
                    wait(0.2);
                    speaker=0;
                    wait(0.2);
                    speaker=1;
                    win();  
                    speaker=0;
                    points=-1;  
                }
                clearPixels();
                x+=1;
                coolx+=1;
                yellowx+=1;
                if(x>4){
                    x=0;
                    y=rand() % 3;
                }
                if(coolx>4){
                    coolx=0;
                    cooly=rand() % 3;    
                }
                if(yellowx>4){
                    yellowx=0;
                    yellowy=rand() % 3;    
                }
            }
            
            
        }
        
    }
    
}