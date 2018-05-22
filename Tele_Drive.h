#ifndef _TELE_DRIVE
#define _TELE_DRIVE

task Joystick_Velocity_Control();
	const char Joystick_Change_Threshold = 15;
	Byte L_Joystick;
	Byte R_Joystick;

task Tele_Drive();
	const char Drive_Threshold = 8;

void Tele_Set_Drive(Byte L_Drive_Power, Byte R_Drive_Power);

const unsigned char Tele_Power_Array[128] = {
	0,  26, 26, 27, 27, 27, 27, 27, 29, 29,
	29, 29, 29, 29, 29, 29, 29, 30, 30, 30,
	30, 30, 31, 31, 31, 31, 32, 32, 32, 32,
	32, 32, 32, 32, 33, 33, 34, 34, 34, 34,
	35, 35, 35, 36, 36, 36, 36, 37, 38, 38,
	38, 39, 40, 40, 40, 40, 40, 41, 41, 42,
	42, 43, 43, 43, 43, 44, 45, 45, 45, 45,
	46, 46, 47, 48, 48, 49, 49, 49, 49, 50,
	51, 51, 53, 53, 53, 54, 56, 56, 56, 58,
	58, 69, 69, 69, 69, 69, 69, 69, 69, 69,
	69, 73, 73, 78, 78, 78, 79, 79, 81, 81,
	81, 81, 87, 88, 88, 88, 89, 89, 89, 94,
	94, 94, 94, 110, 119, 127, 127, 127
};

#endif
