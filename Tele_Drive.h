#ifndef _TELE_DRIVE
#define _TELE_DRIVE

task Joystick_Velocity_Control();
	const char Joystick_Change_Threshold = 15;
	Byte L_Joystick;
	Byte R_Joystick;

task Tele_Drive();
	const char Drive_Threshold = 8;

void Tele_Set_Drive(Byte L_Drive_Power, Byte R_Drive_Power);

#endif
