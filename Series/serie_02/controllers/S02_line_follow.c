
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

//Si le capteur central détecte du blanc, on passe en mode recherche de la ligne, jusqu'à ce qu'il la retrouve,
//Pour ensuite repasser en mode follow_line simple
#define FOLLOW 0
#define SEARCH 1

short int IR_ground[GROUND_SENSORS_COUNT];

//Status: 0 = en train de suivre une ligne / 1 = en train de chercher une ligne perdue
int status = 0;
//State, les droits états dans  la fonction follow. 0 = chercher vers la gauche / 1 = cherche vers la droite / 2 = cherche devant
int state = 0;
//Counter pour les états de follow
int counter = 0;


void follow_line();
void search_line();



void robot_setup() {

    init_robot();
    init_sensors();    
}



//Main loop
void robot_loop() {
  
	while (robot_go_on()) {
		
		if(status == FOLLOW){
			
			follow_line();
			
		}else{
			
			search_line();
			
		}
    }
  
  cleanup_robot();
}




//fonction qui recherche la ligne perdu
void search_line(){
	
	
	get_ground(IR_ground);
	
	//Tant que le capteur central ne capte rien, on tourne à 90° (prendre de la marge) d'un côté, (état = gauche), si recherches infructueuses, on passe en état droit
	//Puis on retourne de 180° vers l'autre côté (état droit). Si on trouve rien non plus, c'est quon'est au début, ou alors au bout d'une ligne (prendre la décision d'avancer jusqu'à une ligne. on passe en état final
	//Si toujours, rien avancer tout droit jusqu'à une ligne (état final)
	//Un schéma FSM sera dispo pour expliquer plus clairement
	
	if(IR_ground[GS_CENTER] > 600){
		
		switch(state){
			
			case 0:
			
				if(counter < 20){
					
					set_speed(-NORM_SPEED,NORM_SPEED);
					counter++;
					
				} else{
					
					counter = 0;
					state = 1;
					
				}
				break;
				
			case 1:
			
				if(counter < 40){
					
					set_speed(NORM_SPEED,-NORM_SPEED);
					counter++;
					
				}else{
					
					counter = 0;
					state = 2;
					
				}
				break;
				
			case 2:
			
				set_speed(NORM_SPEED,NORM_SPEED);
				break;
				
		}
		
	}else{
	
		//le capteur central à trouvé la ligne, on nettoie et passe en follow line
	
		state = 0;
		counter = 0;
		set_speed(0,0);		
		status = 0;
		
	}	
}

//fonction qui suit simplement une ligne droite, fonctionne pour l'octogone simple
void follow_line(){
	
	get_ground(IR_ground);
	
	//Si on voit que le capteur central est aussi sur du blanc, on s'arrête et passe en mode recherche
	
	if(IR_ground[GS_CENTER] > 600){
		
		set_speed(0,0);
		status = 1;
		
	}else{
		
		//Si on détecte du blanc sur le capteur de gauche, on doit décaler vers la droite gentiment, et inversément 		
		
		double gs_left = (1 * IR_ground[GS_LEFT]) / 1.0;
		double gs_right = (1 * IR_ground[GS_RIGHT]) / 1.0;

		double ds_left = (NORM_SPEED * gs_left) / 900;
		double ds_right = (NORM_SPEED * gs_right) / 900;
        
		double speed_left = NORM_SPEED - ds_right;
		double speed_right = NORM_SPEED - ds_left;

		speed_left = bounded_speed(speed_left);
		speed_right = bounded_speed(speed_right);

		set_speed(speed_left, speed_right);
		
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






