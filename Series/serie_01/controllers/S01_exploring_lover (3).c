#include <unistd.h>
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif


void robot_setup() {
    init_robot();
    init_sensors();
    calibrate_prox();
}

//Tableau valeur des capteurs de prox
short int prox_values[8];
	
//0 = currently in lover, 1 = currently in explorer
int status = 0;
//In lover: 0 = forward currently, 1 = backward currently, afin d'implémenter un compteur pour la position d'équilibre	
int avant = 0; 
//counter jusqu'à 10 pour décider de la stabilisation du lover
int counter = 0;

void robot_lover();
void robot_explorer();


void robot_loop() {
		
    while (robot_go_on()) {
		
		if(status == 0){
			
			robot_lover();
			
		}else{

			robot_explorer();
		
		}
    }
    
    cleanup_robot();
}

void robot_lover(){
	
    get_prox_calibrated(prox_values);
    	
	double prox = (prox_values[7] + prox_values[6] + prox_values[0] + prox_values[1]) / 4.;
    double ds = (NORM_SPEED * prox) / MAX_PROX;
    double speed = bounded_speed(NORM_SPEED - ds);
	
	if(avant == 0 && speed < 0){
		
		counter++;
		avant = 1;
		
	} else if(avant == 1 && speed > 0){
		
		counter++;
		avant = 0;
		
	}
	
	//Si on a fait 15 aller-retour, on s'arrête, réinitialise le tout, et passe en mode Explorer
	if(counter > 10){
		
		counter = 0;
		avant = 0;
		status = 1;
		set_speed(0,0);
		
		toggle_led(0);
		toggle_led(1);
		toggle_led(2);
		toggle_led(3);
	
		
	} else {
		
		set_speed(speed,speed);
	
	}
}
	
void robot_explorer(){

    get_prox_calibrated(prox_values);
    	
	double proxL = (prox_values[7] + prox_values[6] + 0.7 * prox_values[5] + 0.2 * prox_values[4]) / 2.9;
    double proxR = (prox_values[0] + prox_values[1] + 0.7 * prox_values[2] + 0.2 * prox_values[3]) / 2.9;
    double dsL = (NORM_SPEED * proxL) / MAX_PROX;
    double dsR = (NORM_SPEED * proxR) / MAX_PROX;
    double speedL = bounded_speed(NORM_SPEED - 550 + dsL);
    double speedR = bounded_speed(NORM_SPEED - 550 + dsR);
		
	set_speed(speedL,speedR);
	
	if((prox_values[7] + prox_values[6] + 0.01 * prox_values[5] + prox_values[0] + prox_values[1] + 0.01 * prox_values[2]) < 50){
		
		status = 0;
		toggle_led(0);
		toggle_led(1);
		toggle_led(2);
		toggle_led(3);
		
	}
	
}

int main (int argc, char **argv) {
    #if SIMULATION
    #else
    ip = argv[1];
    #endif
    
    robot_setup();
    robot_loop();   
}




