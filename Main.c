#pragma config(Sensor, dgtl1,  	R_Drive_Encoder,  	sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  	L_Drive_Encoder,   	sensorQuadEncoder)
#pragma config(Sensor, dgtl7,  	R_Lift_Encoder,  	sensorQuadEncoder)
#pragma config(Sensor, dgtl9,  	L_Lift_Encoder,   	sensorQuadEncoder)
#pragma config(Sensor, dgtl11, 	Tail,           	sensorDigitalOut)
#pragma config(Sensor, dgtl12, 	Skyrise,        	sensorDigitalOut)
#pragma config(Motor,  port1,           Front_R_Drive,    tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           Back_R_Drive,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           Rightlift1,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           Rightlift2,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           intake1,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           intake2,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           Leftlift1,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           Leftlift2,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           Back_L_Drive,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          Front_L_Drive,     tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)
#include "Vex_Competition_Includes.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic properties of robot

#define Base_Robot_Mass 6.7 // In Kg
#define Mass_Conversion_Factor .00043
float Robot_Mass = Base_Robot_Mass*Mass_Conversion_Factor;
#define Base_Mobile_Goal_Mass 1.7
#define Mobile_Goal_Mass (Base_Mobile_Goal_Mass*Mass_Conversion_Factor) // Kg

#define Base_Robot_MOI .17 // Kg-M^2
#define MOI_Conversion_Factor .05
float Robot_MOI = Base_Robot_MOI*MOI_Conversion_Factor;// + .08+0.06;

#define DEG_PER_TICK 0.40625


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type definitions

typedef struct {
	signed char Power;
	int Target;
	signed char Target_Sign;
	int Offset;
} PD_Control;

typedef signed char Byte;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include files
#include "LCD.h"
#include "Tele.c"
#include "Auto.c"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auto, Pre Auto
void pre_auton()
{
	bStopTasksBetweenModes = true;
}

task autonomous()
{
	startTask(Auto_Drive);
	startTask(Drive_Assist);

	Drive(100,500);
	while(Drive_Enable) {
		wait1Msec(50);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// User Control
task usercontrol()
{
	startTask(Joystick_Velocity_Control);
	startTask(Tele_Drive);
	startTask(Tele_Lift);
	startTask(Tele_Intake);
	startTask(Tele_Skyrise);
	startTask(LCD);

	while(true)
	{
		wait1Msec(50);
	} // while (true) {
} // task usercontrol() {
