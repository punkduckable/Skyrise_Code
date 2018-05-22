#ifndef _AUTO_TURN
#define _AUTO_TURN

void Turn(Byte Power, int Angle);
	Byte Turn_Enabled = 0;

task Auto_Turn();
	PD_Control Turn_PD;
	#define Turn_Refresh_Time 15                // in miliseconds
	#define P_Turn 5                            // needs to be large enough to get us to the target from any displacement
	#define D_Turn (sqrt(4*P_Turn*Robot_MPI))   // This is determined by soling 2nd order diff eq for critical damping. See notebook
	#define Turn_Power_Minimum 8
	#define Turn_BreakOut_Counter_Limit (ceil(100/Turn_Refresh_Time)) 		// corresponds to ~.10 seconds
	#define Turn_Min_Velocity 10

	#define DEG_PER_TICK 0.2088

#endif 