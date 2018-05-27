#ifndef _AUTO_DRIVE
#define _AUTO_DRIVE

// Prototypes
void Drive(Byte Power, int Target_Value);
	Byte Drive_Enable = 0;

task Auto_Drive();
	PD_Control Drive_PD;
	#define Drive_Refresh_Time 25                             // in miliseconds
	#define P_Drive 1.0                                        // needs to be large enough to get us to the target from any displacement
	#define D_Drive (sqrt(4*P_Drive*NATURAL_ROBOT_MASS)\
	                               -NATURAL_ROBOT_DAMPING)    // This is determined by solving 2nd order diff eq for critical damping. See notebook
	#define Drive_Power_Minimum 12                            // Lowest power before breakout
	#define Drive_Break_Out_Counter_Limit 5                   // corresponds to ~.10 seconds
	#define Drive_Min_Velocity 20                             // In ticks per second

task Drive_Assist();
	Byte Drive_Assist_Enable = 0;
	Byte L_Drive_Correction = 0;
	Byte R_Drive_Correction = 0;
	#define P_Drive_Assist 3.5
 	#define D_Drive_Assist (sqrt(4*P_Drive_Assist*Robot_MOI))

void Auto_Set_Drive(int L_Drive_Power, int R_Drive_Power);

Byte mod(Byte a, Byte b);

#endif
