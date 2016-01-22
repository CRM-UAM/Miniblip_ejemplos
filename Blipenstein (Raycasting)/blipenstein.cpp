// Arcadio Garcia - @arcadio_g_s
// 22 - Jan - 2016 
// A Wolfsenstein-like raycasting engine for the Miniblip board
// Based on http://lodev.org/cgtutor/raycasting.html
 
#include "mbed.h"
#include "neopixel.h"
#include "USBSerial.h"
#include "math.h"
 
// Matrix led output pin
#define MATRIX_PIN P0_9
#define NLEDS 25
 
unsigned int counter = 0;   
USBSerial serial;
 
DigitalIn pushbutton(P0_23); //This button is used to goes forward
 
neopixel::Pixel buffer[NLEDS];
 
neopixel::PixelArray array(MATRIX_PIN);

AnalogIn   ain(P0_15); //These two buttons are used to turn left/right
AnalogIn   ain2(P0_14); 

 
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
            if(y==1){
                setPixel(x,y, 5,5,5); //The floow is white
            }else if(y==0){
                setPixel(x,y, 10,10,10);// And is whiter near the player 
            }else{
                setPixel(x,y, 0,0,0); // The sky is black
            }
}


int playerangle=0;


int wWidth=10, wHeight=10;


//This is the world map
int map[10][10]={ // 1-RED 2-GREEN 3-BLUE 4-YELLOW
  {1,2,2,2,2,2,2,2,2,2},  
  {1,0,0,0,0,0,0,4,0,1},  
  {1,0,0,0,0,0,0,4,0,1},  
  {1,0,0,0,0,0,0,0,0,1},  
  {1,0,3,0,0,0,0,0,0,1},  
  {1,0,3,0,0,0,0,0,0,1},  
  {1,0,3,0,0,0,0,0,0,1},  
  {1,0,0,0,0,0,0,0,0,1},  
  {1,0,0,0,0,0,0,0,0,1}, 
  {2,2,2,2,2,2,2,2,2,1} 
};


//Player coordinates
double playerX=5;
double playerY=5;

//This vector tells where is the player looking at
double playerDirX=1;
double playerDirY=1;

//This plane is perpendicular to playerDir and has the same length, so FOV=90º
double playerPlaneX=-1;
double playerPlaneY=1;



void paintFrame(){
            clearPixels(); // Clear this frame
            int x, y;
            for (x=0;x<5;x++){
                //Calculate ray position and direction
                double cameraX = 2 * x / double(5) - 1;
                double rayPosX = playerX;
                double rayPosY = playerY;
                double rayDirX = playerDirX + playerPlaneX * cameraX;
                double rayDirY = playerDirY + playerPlaneY * cameraX;
                
                //The current tile of the ray
                int mapX = int(rayPosX);
                int mapY = int(rayPosY);
                
                //Length of ray from current position to next x or y-side
                double sideDistX;
                double sideDistY;
                
                //How much the ray moves in each iteration in each axis
                double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
                double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
                double perpWallDist;
                
                int stepX;//Does the ray moves left (-1) or right (+1)
                int stepY;//Does the ray moves up (-1) or down (+1)

                int hit = 0; //Has the ray found a wall?
                int side; //Is it a horizontal or vertical wall?
                
                //calculate step and initial sideDist
                if (rayDirX < 0)
                {
                    stepX = -1;
                    sideDistX = (rayPosX - mapX) * deltaDistX;
                }
                else
                {
                    stepX = 1;
                    sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
                }
                if (rayDirY < 0)
                {
                    stepY = -1;
                    sideDistY = (rayPosY - mapY) * deltaDistY;
                }
                else
                {
                    stepY = 1;
                    sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
                }
                
                //Keep moving the ray until it hits somthing
                while (hit == 0)
                {
                    //jump to next map square, OR in x-direction, OR in y-direction
                    if (sideDistX < sideDistY)
                    {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                    }
                    else
                    {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                    }
                    //Check if ray has hit a wall
                    if (map[mapX][mapY] > 0) hit = 1;
                } 
                
                 //Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
                    if (side == 0)
                    perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
                    else
                    perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
                    
                    //Intensity of color = 5/d^4 (hardcoded so it looks nice for this map size)
                    double intensity= 5/(perpWallDist*perpWallDist*perpWallDist*perpWallDist);
                    //Calculate color depending on map value
                    int r,g,b;
                    switch(map[mapX][mapY]){
                        case 1: //R
                            r=255;
                            g=0;
                            b=0;
                            break;
                         case 2: //G
                            r=0;
                            g=255;
                            b=0;
                            break;
                         case 3: //B
                            r=0;
                            g=0;
                            b=255;
                            break;
                         case 4: //Y
                            r=255;
                            g=255;
                            b=0;
                            break;
                    }
                    
                    //The height of the walls depends on distance, to create perspective
                    int y;
                    int height=2;
                    if(perpWallDist>=3.2){ //This value is hardcoded based on the map size so it looks nice
                        height--;
                    }
                    if(perpWallDist>=2.5){//This value is hardcoded based on the map size so it looks nice
                        height--;
                    }
                    setPixel(x,2, r*intensity,g*intensity,b*intensity); //At least, walls are 1 px tall
                    
                    for(y=1;y<=height;y++){ //But they can be up to 5 px tall
                        setPixel(x,2+y, r*intensity,g*intensity,b*intensity);
                        setPixel(x,2-y, r*intensity,g*intensity,b*intensity);
                    }
                }
            
}
 
PwmOut speaker(P0_8); // The speaker will be used to notify of collisions


int round (float n){ //TIL: round is not part of ANSI C, had to implement it myself
    float f=floor(n);
    if(n-f>0.5){
        return (int)ceil(n);
    }else{
        return (int)floor(n);
    }
}
 
int main()
{
    // Mute the speaker
    speaker=0.0;
    
    while(1) { //This is the game loo`p
            paintFrame(); //Render the scene
            updateLEDs(); // Pain the scene
            wait_ms(50);  // Wait 50ms, so there are 20 fps
            
            double rotSpeed=0; //Angular speed
            speaker=0.0;//Mute the speaker each frame
            
            //These buttons allow to turn left/right
            if(ain2.read() < 0.1){
                rotSpeed= 0.05;
            }
            if(ain.read() < 0.1){
                rotSpeed= -0.05;
            }
            //If the clicky button is down, try to move forward
            if(pushbutton){
                if(map[round(playerX+playerDirX/50)][round(playerY+playerDirY/50)]==0){ //If the taarget coordinates are empty, move
                    playerX+=playerDirX/20;
                    playerY+=playerDirY/20;
                }else{
                    speaker=1; //If not, notify with a beep
                }
            }
            
            //Update the direction and plane vectors according to the angular speed
            double oldDirX = playerDirX;
            playerDirX = playerDirX * cos(-rotSpeed) - playerDirY * sin(-rotSpeed);
            playerDirY = oldDirX * sin(-rotSpeed) + playerDirY * cos(-rotSpeed);
            double oldPlaneX = playerPlaneX;
            playerPlaneX = playerPlaneX * cos(-rotSpeed) - playerPlaneY * sin(-rotSpeed);
            playerPlaneY = oldPlaneX * sin(-rotSpeed) + playerPlaneY * cos(-rotSpeed);
    }
}