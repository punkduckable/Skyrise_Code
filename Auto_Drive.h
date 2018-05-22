#ifndef AUTO_DRIVE
#define AUTO_DRIVE

// Prototypes
void Drive(int Power, int Target_Value);
	Byte Drive_Enable = 0;

task Auto_Drive();
	PD_Control Drive_PD;
	#define Drive_Refresh_Time 50 								// in miliseconds
	#define P_Drive 1.25 										// needs to be large enough to get us to the target from any displacement
	#define D_Drive (sqrt(4*P_Drive*NATURAL_ROBOT_MASS)\
	                               -NATURAL_ROBOT_DAMPING) 		// This is determined by solving 2nd order diff eq for critical damping. See notebook
	#define Drive_Power_Minimum 12								// Lowest power before breakout
	#define Drive_Break_Out_Counter_Limit 5 					// corresponds to ~.10 seconds
	#define Drive_Min_Velocity 20								// In ticks per second

task Drive_Assist();
	short Drive_Assist_Enable = 0;
	Byte L_Drive_Correction = 0;
	Byte R_Drive_Correction = 0;
	#define P_Drive_Assist 3.5
 	#define D_Drive_Assist (sqrt(4*P_Drive_Assist*Robot_MOI))
 	PD_Control Turn_PD;											// REMOVE ME ONCE AUTP_TURN IS UPDATED

const unsigned char Auto_Power_Array[128] = {
	0, 26, 26, 27, 27, 27, 27, 27, 29, 29,
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

void Auto_Set_Drive(int L_Drive_Power, int R_Drive_Power);

Byte mod(Byte a, Byte b);

#endif
