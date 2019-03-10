
#define SIMULATION 0

#if SIMULATION
#include "../API/webots/webotsAPI.h"  
#else
#include "../API/epuck/epuckAPI.h"
#endif




#define TIME_STEP 64


#define K 100
#define T_I 1000000000
#define T_D 0
#define PID_WALL_FOLLOW_TARGET 0
// careful 200 is for real epuck, change this value for simulation to 2
#define PID_MAX_DS 2

double error = 0;
double deriv = 0;
double integ = 0;

int whichSide(short int prox_values[]);
void rightMode(short int prox_values[]);
void leftMode(short int prox_values[]);
void turnSpeed(double average);

double pid(double proxLR) {

  double prev_err = error;
  error = proxLR - PID_WALL_FOLLOW_TARGET;
  
  //deriv = (error - prev_err);
  deriv = (error - prev_err)*1000/TIME_STEP;
  //integ += error;
  integ += error*TIME_STEP/1000;
    
  return K * ( error + 1.0 / T_I * integ + T_D * deriv);
}

void robot_setup() {
    init_robot();
    init_sensors();
    calibrate_prox();
}

void robot_loop() {
	short int prox_values[PROX_SENSORS_COUNT];
	
	// open files for writing
    FILE *log = fopen("logPID.csv", "w");

    if (log == NULL) {
        printf("Error opening file!\n");
        return ;
    } 

    // write header in CSV file
    fprintf(log, "time,P,I,D,ds,left speed,right speed\n");

    int loop_counter = 0;

    while (robot_go_on()) {
    
        loop_counter++;

        get_prox_calibrated(prox_values);

        switch (whichSide(prox_values))
        {
            case 0:
                set_speed(6,6);
                break;
            case 1:
                rightMode(prox_values);
                break;
            case -1:
                leftMode(prox_values);
                break;
            default:
                break;
        }


        double a = 2;
        double b = 1.5;
        double c = 1;
        double d = 0.5;


        double prox_right = (a * prox_values[0] + b * prox_values[1] + c * prox_values[2] + d * prox_values[3]) / (a+b+c+d);
        double prox_left = (a * prox_values[7] + b * prox_values[6] + c * prox_values[5] + d * prox_values[4]) / (a+b+c+d);
                      
        //compute PID response according to IR sensor value
        double rds = pid(prox_right);
        double lds = pid(prox_left);      
      
        // adjust speed values to turn away from obstacle
        double speed_right = (abs(rds)>PID_MAX_DS ? 0 : NORM_SPEED) + rds;
        double speed_left  = (abs(rds)>PID_MAX_DS ? 0 : NORM_SPEED) + rds;          
            
        // write a line of data in log file
        fprintf(log, "%d,%f,%f,%f,%f,%f,%f\n", loop_counter, K*error, K*integ/T_I, K*deriv*T_D, rds, speed_left,speed_right);

        // set_speed(bounded_speed(speed_left), bounded_speed(speed_right));
    }
}

void rightMode(short int prox_values[]){
    double a = 1;
    double b = 1;
    double c = 0.5;
    double d = 0;

    double prox_right = (a * prox_values[0] + b * prox_values[1] + c * prox_values[2] + d * prox_values[3]) / (a+b+c+d);

    turnSpeed(prox_right);
}

void leftMode(short int prox_values[]){
    double a = 1;
    double b = 1;
    double c = 0.5;
    double d = 0;

    double prox_left = (a * prox_values[7] + b * prox_values[6] + c * prox_values[5] + d * prox_values[4]) / (a+b+c+d);

    turnSpeed(prox_left);
}

void turnSpeed(double average){
    if (average < 750){
        set_speed(-5, 5);
    } else if (average > 1250) {
        set_speed(5, -5);
    } else {
        set_speed(5,5);
    }
    
}

int whichSide(short int prox_values[]){
    double threshold = 200;
    int none = 0;
    int left = -1;
    int right = 1;

    double prox_right = (prox_values[0] + prox_values[1] + prox_values[2] + prox_values[3])/4;
    double prox_left = (prox_values[7] + prox_values[6] + prox_values[5] + prox_values[4])/4;
    double diff = prox_right - prox_left;

    if (prox_right < threshold && prox_left < threshold){
        return none;
    } else if (diff > 0){
        return right;
    } else {
        return left;
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




