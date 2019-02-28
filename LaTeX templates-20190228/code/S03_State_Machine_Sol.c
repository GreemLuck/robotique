/********************************************************************************************
State Machine

Description: The robot changes between two states: LOVER, EXPLORER. He starts out in LOVER.
    When he finds a wall, a counter is triggered. When the counter reaches a certain number,
    the robot enters EXPLORER and another counter is started. When the second counter
    reaches a certain number, the robot switches back to LOVER.
Author: Beat Hirsbrunner, Julien Nembrini, Simon Studer (University of Fribourg)
Version: 1.0 (2016-03-09)
********************************************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include <webots/differential_wheels.h>
#include <webots/distance_sensor.h>
#include <webots/led.h>
#include <webots/robot.h>

#define TIME_STEP 64
#define PROX_SENSORS_NUMBER 8
#define LED_NUMBER 8
#define NBR_CALIB 50

#define MAX_SPEED 1000.0
#define NORM_SPEED 400.0
#define THRESH_PROX 400.0
#define THRESH_SPEED 50

// states
#define LOVER 1
#define EXPLORER 2

#define COUNT_WALL 30
#define COUNT_EXPL 50

// global variables
const char *prox_sensors_names[PROX_SENSORS_NUMBER] =
  {"ps0", "ps1", "ps2", "ps3", "ps4", "ps5", "ps6", "ps7"};
WbDeviceTag prox_sensor_tags[PROX_SENSORS_NUMBER];
double prox_corr_vals[PROX_SENSORS_NUMBER];

const char *led_names[LED_NUMBER] =
  {"led0", "led1", "led2", "led3", "led4", "led5", "led6", "led7"};
WbDeviceTag led_tags[LED_NUMBER];

// get the correction values for prox sensors
void get_prox_corr_vals() {
  int i, j;
  
  // init array for calibration values  
  for (i=0; i<PROX_SENSORS_NUMBER; i++) {
    prox_corr_vals[i] = 0;
  }

  // get multiple readings for each sensor
  for (j=0; j<NBR_CALIB && wb_robot_step(TIME_STEP)!=-1; j++) {
    for (i=0; i<PROX_SENSORS_NUMBER; i++) {
      prox_corr_vals[i] += wb_distance_sensor_get_value(prox_sensor_tags[i]);
    }
  }
  
  // calculate average for each sensor
  for (i=0; i<PROX_SENSORS_NUMBER; i++) {
    prox_corr_vals[i] = prox_corr_vals[i] / NBR_CALIB;
  }
}
   
// constrain speed to +/- MAX_SPEED
double bounded_speed(double speed) { 
  if (speed > MAX_SPEED) return MAX_SPEED;
  else if (speed < -MAX_SPEED) return -MAX_SPEED;
  else return speed;
}

int main(int argc, char **argv) {
  double prox[PROX_SENSORS_NUMBER];
  int leds[LED_NUMBER];
  double speed_left, speed_right;
  double prox_left, prox_right;
  double ds_left, ds_right;
  int counter_wall, counter_expl;
  int state = LOVER;
  int i;
  
  wb_robot_init();
  
  speed_left = speed_right = 0;
  counter_wall = counter_expl = 0;
  
  // init prox sensors
  for (i=0;  i<PROX_SENSORS_NUMBER; i++) {
    prox_sensor_tags[i] = wb_robot_get_device(prox_sensors_names[i]);
    wb_distance_sensor_enable(prox_sensor_tags[i], 4*TIME_STEP);
  }
  
  // init leds
  for (i=0;  i<LED_NUMBER; i++) {
    led_tags[i] = wb_robot_get_device(led_names[i]);
    leds[i] = 0;
  }

  get_prox_corr_vals();

  while (wb_robot_step(TIME_STEP)!=-1) {
  
    // fetch prox sensor values and calibrate
    for (i=0; i<PROX_SENSORS_NUMBER; i++) {
      prox[i] = wb_distance_sensor_get_value(prox_sensor_tags[i])-prox_corr_vals[i];
    }
    
    
    // make sure speed values are legal
    speed_left = bounded_speed(speed_left);
    speed_right = bounded_speed(speed_right);
    
    wb_differential_wheels_set_speed(speed_left, speed_right);
    
    // set LEDs to value in 'leds' array
    for (i=0; i<LED_NUMBER; i++) {
      wb_led_set(led_tags[i], leds[i]);
    }
  };
  
  wb_robot_cleanup();
  return EXIT_SUCCESS;
}
