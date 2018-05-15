// Prototypes
void Drive(int Power, int Target_Value);
	Byte Drive_Enable = 0;

task Auto_Drive();
	PD_Control Drive_PD;
	#define Drive_Refresh_Time 50 					// in miliseconds
	#define P_Drive 1.25 							// needs to be large enough to get us to the target from any displacement
	#define D_Drive (sqrt(4*P_Drive*Robot_Mass)) 	// This is determined by solving 2nd order diff eq for critical damping. See notebook
	#define Drive_Power_Minimum 12					// Lowest power before breakout
	#define Drive_Break_Out_Counter_Limit 5 		// corresponds to ~.10 seconds
	#define Drive_Min_Velocity 20					// In ticks per second

task Drive_Assist();
	short Drive_Assist_Enable = 0;
	Byte L_Drive_Correction = 0;
	Byte R_Drive_Correction = 0;
	#define P_Drive_Assist 3.5
  #define D_Drive_Assist (sqrt(4*P_Drive_Assist*Robot_MOI))
  PD_Control Turn_PD;				// REMOVE ME ONCE AUTP_TURN IS UPDATED

const unsigned char Auto_Power_Array[128] = {
	0, 15, 15, 15, 15, 15, 15, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 17,
	17, 17, 17, 17, 17, 18, 18, 18, 18, 18,
	18, 19, 19, 19, 19, 20, 20, 20, 20, 20,
	21, 21, 21, 21, 22, 22, 22, 22, 23, 23,
	23, 24, 24, 24, 24, 25, 25, 26, 26, 26,
	26, 27, 27, 27, 28, 28, 28, 28, 29, 29,
	29, 30, 30, 31, 31, 31, 32, 32, 33, 33,
	34, 34, 35, 35, 35, 36, 37, 37, 38, 38,
	39, 40, 41, 42, 42, 43, 43, 44, 45, 46,
	47, 48, 49, 51, 52, 52, 54, 56, 56, 58,
	59, 61, 62, 65, 67, 70, 73, 75, 77, 79,
	84, 84, 85, 86, 86, 87, 88, 89
};

void Auto_Set_Drive(int L_Drive_Power, int R_Drive_Power);

Byte mod(Byte a, Byte b);


////////////////////////////////////////////////////////////////////////////////////
 // Functions, Tasks

void Drive(int Power, int Target_Value) {
	// Check that power is a positive number
	if (Power < 0) { return; }

	// Set drive global variables
	Drive_PD.Power = Power;
	Drive_PD.Target = Target_Value;
	Drive_PD.Target_Sign = (Drive_PD.Target > 0) - (Drive_PD.Target < 0);

	// Enable auto_Drive
	Drive_Enable = 1;
} // void Run_Drive(int Power, int Target_Value)

task Auto_Drive() {
	// Declare Local variables
	int Position;										// Drive position
	int Error[5];										// Error array
	int Velocity;										// Drive velocity
	Byte Error_Sign;									// Sign of error
	unsigned char Break_Out_Counter = 0;				// Conter to determine if we should stop
	int L_Drive_Power, R_Drive_Power;					// Power for left and right side of drive
	Byte i;												// For loop counters
	Byte k;												// Used to know which value of Error Array to update

	// Loop continuously, waiting to be enabled
	while(true) {
		// If enabled, set up drive stuff
		if(Drive_Enable) {
			// Drive setup
			SensorValue[R_Drive_Encoder] = 0;			// Reset encoders
			SensorValue[L_Drive_Encoder] = 0;
			Drive_Assist_Enable = 1;					// Turn on drive assist
			Break_Out_Counter = 0;						// Reset breakout counter

			// Set Drive Error
			for(i = 0; i < 5; i++) {
				Error[i] = Drive_PD.Target;
			} // for(i = 0; i < 5; i++) {
			Velocity = 0;

			// Reset Iteration Counter
			k = 0;

			// Loop while drive is enabled, breakout counter is below limit.
			while((Drive_Enable) && (Break_Out_Counter < Drive_Break_Out_Counter_Limit)) {
				// Update Position value
				Position = .5*(SensorValue[R_Drive_Encoder] + SensorValue[L_Drive_Encoder]) - Drive_PD.Offset;

				// Update Errors
				k++;					// Incremenet Error counter
				k = mod(k,5);
				Error[k] = Drive_PD.Target - Position;
				Error_Sign = (Error[k] > 0) - (Error[k] < 0);

				// Calculate Derivative
				/* 	We use a 5 point approximation to the derivative:
						f'(x) ~= (1/(12*h))( 25*f(x) - 48*f(x-h) + 36*f(x-2h) - 16*f(x-3h) + 3*f(x-4h))
					This approximation has O(h^4) error. In this case, the f(x)'s represent the previous errors.
					These values are all stored in the Error array. We update this array in a cyclic manner. We
					update the 0th element on one iteration, the 2nd on the next, then the 3rd, and 4th. We then
					loop back to the 0th. The k variable keeps track of which element is being updated. In this
					way, the kth element of Error always stores the current error, the k-1th stores the f(x-h)
					error and so on.

					However, we need to access the elements cyclically as well. For example, suppose that k
					is 0. In this case, the last error is stored in the 4th element. But, k-1 is -1. However,
					notice that (k-1) mod 5 = 4. In general, we can always access the correct element using the
					modulus operator. Unfortunetly, c's % operator doesn't work like a modulus operator. Thus,
					I defined my own mod function and call that.

					Finally, recall that the refresh time is in miliseconds. Thus, we have to convert it to
					regular seconds
				*/
				Velocity = (1000./(12.*Drive_Refresh_Time))*( 25*Error[k]
				                                            - 48*Error[mod(k-1,5)]
				                                            + 36*Error[mod(k-2,5)]
				                                            - 16*Error[mod(k-3,5)]
				                                            +  3*Error[mod(k-4,5)]);
				/* 	Check for stall (if velocity is below threshold).
					We only run this check if we've moved 100 ticks twoards the target. This is done
					because the robot starts off not moving. Thus, the robot may think it has stalled
					when really it is just starting up. Thus, we only run this check once the robot has
					started driving for a bit.
				*/
				if( abs(Error[k]) < (abs(Drive_PD.Target)-100) && abs(Velocity) < Drive_Min_Velocity) {
					Break_Out_Counter++;
				} // if( abs(Drive_Delta) < (abs(Drive_PD.Target)-100) && abs(Velocity) < Min_Velocity) {
				else { Break_Out_Counter = 0; }			// Otherwise, reset breakout counter

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Assign Left and Right Drive_Power

				L_Drive_Power = P_Drive*Error[k] + D_Drive*Velocity; // Assign L

				// Limit max velocity (before corrections) to specified power to drive power
				if (abs(L_Drive_Power) > abs(Drive_PD.Power)) {
					L_Drive_Power = (Error_Sign)*(Drive_PD.Power);
				} // if (abs(L_Drive_Power) > abs(Drive_PD.Power)) {

				R_Drive_Power = L_Drive_Power;				// Assign R

				// Print P,D correction terms to debug stream (if connected)... mostly for tuning
				writeDebugStreamLine("P: %i D: %i",P_Drive*Error[k],D_Drive*Velocity);

				// 	Now we add in the corrections. These corrections are set in the Drive_Assist task.
				L_Drive_Power += L_Drive_Correction;
				R_Drive_Power += R_Drive_Correction;
				//L_Drive_Power += L_Drive_Correction*((float)L_Drive_Power/(float)Drive_PD.Power);
				//R_Drive_Power += R_Drive_Correction*((float)R_Drive_Power/(float)Drive_PD.Power);

				// Now, set drive. If we're close
				Auto_Set_Drive(L_Drive_Power, R_Drive_Power);

				wait1Msec(Drive_Refresh_Time);
			} // while (Drive_Enable)

			//// shut down, reset global target values ////
			Auto_Set_Drive(0,0);
			Drive_PD.Offset = Error[k];
			Drive_PD.Power = 0;
			Drive_PD.Target = 0;
			Drive_PD.Target_Sign = 0;
			Break_Out_Counter = 0;
			Drive_Assist_Enable = 0;
			Drive_Enable = 0;
		}	// if(Drive_Enable)
		else {
			wait1Msec(Drive_Refresh_Time);
		} // else {
	} // while (true)
} // task Auto_Drive()


// This task makes sure the drive goes straight
task Drive_Assist(){
	// Declare Local variables
	float Angle;
	float Error[5];								// Note: Angles are measured as floating point numbers.
	float Angular_Velocity;
	Byte i;										// Counter variable (for loops)
	Byte k;										// Cycle variable to update Error Array

	//////// REMOVE ONCE AUTO_TURN IS DEFINE
	Turn_PD.Power = 0;
	Turn_PD.Target = 0;
 	Turn_PD.Target_Sign = 0;
 	Turn_PD.Offset = 0;

	// Loop continuously, waiting to be enabled
	while(true) {
		// If enabled, set up drive stuff
		if(Drive_Assist_Enable) {
			// Error, Velocity setup.
			for(i = 0; i < 5; i++) {
				Error[i] = Turn_PD.Offset;
			}
			Angular_Velocity = 0;

			// Loop while drive_assist is enabled.
			while(Drive_Assist_Enable) {
				// Update Drive position. Turn_PD.Offset contains the last Angle Error. The angle is
				// positive if the robot has rotated clockwise about the +z axis.
				Angle = DEG_PER_TICK*.5*(SensorValue[R_Drive_Encoder] - SensorValue[L_Drive_Encoder]) - Turn_PD.Offset;


				// Update Errors.
				k++;					// Incremenet Error counter
				k = mod(k,5);
				Error[k] = 0 - Angle;	// With Drive assist, the target angle is always 0

				// Calculate Derivative
				Angular_Velocity = (1000./(12.*Drive_Refresh_Time))*( 25*Error[k]
				                                                    - 48*Error[mod(k-1,5)]
				                                                    + 36*Error[mod(k-2,5)]
				                                                    - 16*Error[mod(k-3,5)]
				                                                    +  3*Error[mod(k-4,5)]);

				// Assign L, R correction
				R_Drive_Correction = P_Drive_Assist*Error[k] + D_Drive_Assist*Angular_Velocity;
				R_Drive_Correction = Drive_PD.Target_Sign*R_Drive_Correction; // So that the error is correct if the robot drives backwards
				L_Drive_Correction = -R_Drive_Correction;
				/* L correction is neg of R correction. To understand why this is, we need to consider
				   how this code works. If the Robot has rotated a clockwise angle, then the angle will
				   will be positive and the error will be negative. A clockwise error implies that the
				   right side of the drive has gotten ahead of the left. Thus, we need to slow down the
				   right and speed up the left. This means adding the correction terms onto the R and
				   subtracting them from the L. Therefore, the L correction is the negative of the R correction
				   The same is true, but with all the signs reversed, when the robot is moving backwards.
				*/
				wait1Msec(Drive_Refresh_Time);
			} // while (Drive_Assist_Enable)
			//// shut down, reset global target values ////
			Turn_PD.Offset = Error[k];
			L_Drive_Correction = 0;
			R_Drive_Correction = 0;
		}	// if(Drive_Enable)
		else {
			wait1Msec(Drive_Refresh_Time);
		} // else {
	} // while (true)
} // task Drive_Assist()

void Auto_Set_Drive(int L_Drive_Power, int R_Drive_Power) {
	Byte L_Drive_Sign;
	Byte R_Drive_Sign;
	int L_Drive_Speed;
	int R_Drive_Speed;

	// Find sign of inputs
	L_Drive_Sign = (L_Drive_Power > 0) - (L_Drive_Power < 0);
	R_Drive_Sign = (R_Drive_Power > 0) - (R_Drive_Power < 0);

	// Find magnitudes
	L_Drive_Speed = abs(L_Drive_Power);
	R_Drive_Speed = abs(R_Drive_Power);

	// Make sure magnitudes are less than 127
	L_Drive_Speed = (L_Drive_Speed > 127) ? 127 : L_Drive_Speed;
	R_Drive_Speed = (R_Drive_Speed > 127) ? 127 : R_Drive_Speed;

	motor[Back_R_Drive] = R_Drive_Sign*Auto_Power_Array[R_Drive_Speed];
	motor[Front_R_Drive] = R_Drive_Sign*Auto_Power_Array[R_Drive_Speed];
	motor[Back_L_Drive] = L_Drive_Sign*Auto_Power_Array[L_Drive_Speed];
	motor[Front_L_Drive] = L_Drive_Sign*Auto_Power_Array[L_Drive_Speed];
} // void Set_Drive(int Power)

Byte mod(Byte a, Byte b) {
	return (a%b + b)%b;
} // int mod(int a, int b) {
