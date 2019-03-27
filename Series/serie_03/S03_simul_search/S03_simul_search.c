
#define SIMULATION 1

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif

// states
#define LOVER 1
#define EXPLORER 2
#define STOP 3


#define COUNT_LOV 30
#define COUNT_EXPL 250
#define THRESH_SPEED 1

void robot_setup() {
    init_robot();
    init_sensors();
    init_communication();
    calibrate_prox();
}

void robot_loop() {
	short int prox_values[8];
	
	int counter_wall = 0;
	int counter_expl = 0;
    int state = LOVER;


    while (robot_go_on()) {

        get_prox_calibrated(prox_values);

        double prox_left = (4 * prox_values[7] + 2 * prox_values[6] + prox_values[5]) / 7.;
        double prox_right = (4 * prox_values[0] + 2 * prox_values[1] + prox_values[2]) / 7.;

        double ds_left = (NORM_SPEED * prox_left) / MAX_PROX;
        double ds_right = (NORM_SPEED * prox_right) / MAX_PROX;
        
        double speed_left = NORM_SPEED - ( state == EXPLORER ? ds_right : ds_left);;
        double speed_right = NORM_SPEED - ( state == EXPLORER ? ds_left : ds_right);;
        
        // state machine
        if ((state == EXPLORER) && (counter_expl > COUNT_EXPL)) {
              state = LOVER;
              counter_wall = 0;
              for (int i=0; i<LED_COUNT; i++)
                toggle_led(i);
        } else if ((state == LOVER) && (counter_wall > COUNT_LOV)) {
          counter_expl = 0;
          state = STOP; 

          for (int i=0; i<LED_COUNT; i++)
            toggle_led(i);

          char snd[6];
          int id = get_robot_ID();

          sprintf(snd, "READY");
          printf("robot %d sends %s\n", id , snd);
          send_msg(snd);

        } 
        
        if (state == STOP){

          speed_left = 0;
          speed_right = 0;

          char rcv[6];
          char tmp[6];
          int id = get_robot_ID();

          receive_msg(tmp);

          sprintf(rcv, "READY");
          if (strcmp(rcv, tmp)==0){
            printf("robot %d receives %s\n", id, tmp);
            state = EXPLORER;
          }
        }

        // increment counters
        if (state == LOVER) {
          if ((abs(speed_right) < THRESH_SPEED) && (abs(speed_left) < THRESH_SPEED))
            counter_wall++;
        } else if (state == EXPLORER){
            counter_expl++;
        } 

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




