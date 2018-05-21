#ifndef TELE_DRIVE
#define TELE_DRIVE

task Joystick_Velocity_Control();
	const char Joystick_Change_Threshold = 15;
	Byte L_Joystick;
	Byte R_Joystick;

task Tele_Drive();
	const char Drive_Threshold = 8;

void Tele_Set_Drive(Byte L_Drive_Power, Byte R_Drive_Power);

const unsigned char Tele_Power_Array[128] = {
	0, 0, 0, 30, 30, 30, 30, 31, 31, 31,
	31, 31, 31, 31, 31, 31, 31, 31, 31, 32,
	32, 32, 32, 32, 32, 33, 33, 33, 33, 33,
	33, 34, 34, 34, 34, 35, 35, 35, 35, 35,
	36, 36, 36, 36, 37, 37, 37, 37, 38, 38,
	38, 39, 39, 39, 39, 40, 40, 41, 41, 41,
	41, 42, 42, 42, 43, 43, 43, 43, 44, 44,
	44, 45, 45, 46, 46, 46, 47, 47, 48, 48,
	49, 49, 50, 50, 50, 51, 52, 52, 53, 53,
	54, 55, 56, 57, 57, 58, 58, 59, 60, 61,
	62, 63, 64, 66, 67, 67, 69, 71, 71, 73,
	74, 76, 77, 80, 82, 85, 88, 90, 92, 94,
	95, 95, 95, 95, 95, 95, 95, 95
};

#endif