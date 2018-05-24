void Drive(Byte Power, int Target_Value) {
	// Check that power is a positive number
	if (Power < 0) { return; }

	// Set drive global variables
	Drive_PD.Power = Power;
	Drive_PD.Target = Target_Value;

	// Enable auto_Drive
	Drive_Enable = 1;
} // void Run_Drive(Byte Power, int Target_Value)

task Auto_Drive() {
	// Declare Local variables
	int Position;										  // Drive position
	int Velocity;										  // Drive velocity
	Byte Error_Sign;									// Sign of error
	Byte Break_Out_Counter = 0;				// Conter to determine if we should stop
	int L_Drive_Power, R_Drive_Power;					// Power for left and right side of drive
	Byte i;												// For loop counters
	Byte k;												// Used to know which value of Error Array to update

	// Setup Drive_PD.
	Drive_PD.Power = 0;
	Drive_PD.Target = 0;
	Drive_PD.Offset = 0;

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
				Drive_PD.Error[i] = Drive_PD.Target;
			} // for(i = 0; i < 5; i++) {
			Velocity = 0;

			// Reset Iteration Counter
			k = 0;

			// Print first line of output (header). Start timer
			writeDebugStreamLine("Time   Error  Velocity");
			clearTimer(T1);

			// Loop while drive is enabled, breakout counter is below limit.
			while((Drive_Enable) && (Break_Out_Counter < Drive_Break_Out_Counter_Limit)) {
				// Update Position value
				Position = .5*(SensorValue[R_Drive_Encoder] + SensorValue[L_Drive_Encoder]) + Drive_PD.Offset;

				// Update Errors
				k++;					// Incremenet Error counter
				k = mod(k,5);
				Drive_PD.k = k;
				Drive_PD.Error[k] = Position - Drive_PD.Target;
				Error_Sign = (Drive_PD.Error[k] > 0) - (Drive_PD.Error[k] < 0);

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
				Velocity = (1000./(12.*Drive_Refresh_Time))*( 25*Drive_PD.Error[k]
				                                            - 48*Drive_PD.Error[mod(k-1,5)]
				                                            + 36*Drive_PD.Error[mod(k-2,5)]
				                                            - 16*Drive_PD.Error[mod(k-3,5)]
				                                            +  3*Drive_PD.Error[mod(k-4,5)]);
				/* 	Check for stall (if velocity is below threshold).
					We only run this check if we've moved 100 ticks twoards the target. This is done
					because the robot starts off not moving. Thus, the robot may think it has stalled
					when really it is just starting up. Thus, we only run this check once the robot has
					started driving for a bit.
				*/
				if( abs(Drive_PD.Error[k]) < (abs(Drive_PD.Target)-100) && abs(Velocity) < Drive_Min_Velocity) {
					Break_Out_Counter++;
				} // if( abs(Drive_Delta) < (abs(Drive_PD.Target)-100) && abs(Velocity) < Min_Velocity) {
				else { Break_Out_Counter = 0; }			// Otherwise, reset breakout counter

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Assign Left and Right Drive_Power

				/* Assign L.
				      If the error is positive then the robot must be past the target. We want
				      the robot to move backwards twoards the target (-kx).

				      If the robot's velocity is positive, then we need to damp it out (-d*x').
				*/
				L_Drive_Power = (signed char)(-P_Drive*Drive_PD.Error[k]); // - D_Drive*Velocity); // Assign L

				// Limit max velocity (before corrections) to specified power to drive power
				if (abs(L_Drive_Power) > abs(Drive_PD.Power)) {
					L_Drive_Power = -(Error_Sign)*(Drive_PD.Power);
				} // if (abs(L_Drive_Power) > abs(Drive_PD.Power)) {

				R_Drive_Power = L_Drive_Power;				// Assign R

				// Print P,D correction terms to debug stream (if connected)... mostly for tuning
				writeDebugStreamLine("%4i,  %4i,  %6.3f",time1(T1), Drive_PD.Error[k], Velocity);

				// 	Now we add in the corrections. These corrections are set in the Drive_Assist task.
				L_Drive_Power += L_Drive_Correction;
				R_Drive_Power += R_Drive_Correction;

				// Now, set drive. If we're close
				Auto_Set_Drive(L_Drive_Power, R_Drive_Power);

				wait1Msec(Drive_Refresh_Time);
			} // while (Drive_Enable)

			//// shut down, reset global target values ////
			Auto_Set_Drive(0,0);
			Drive_PD.Offset = Drive_PD.Error[k];
			Drive_PD.Power = 0;
			Drive_PD.Target = 0;
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
	float Angular_Velocity;
	Byte i;										// Counter variable (for loops)
	Byte k;										// Cycle variable to update Error Array

	// Loop continuously, waiting to be enabled
	while(true) {
		// If enabled, set up drive stuff
		if(Drive_Assist_Enable) {
			// Error, Velocity setup.
			for(i = 0; i < 5; i++) {
				Turn_PD.Error[i] = Turn_PD.Offset;
			}
			Angular_Velocity = 0;

			// Loop while drive_assist is enabled.
			while(Drive_Assist_Enable) {
				// Update Drive position. Turn_PD.Offset contains the last Angle Error. The angle is
				// positive if the robot has rotated clockwise about the +z axis.
				Angle = DEG_PER_TICK*.5*(SensorValue[R_Drive_Encoder] - SensorValue[L_Drive_Encoder]) + Turn_PD.Offset;

				// Update Errors.
				k++;					// Incremenet Error counter
				k = mod(k,5);
				Turn_PD.k = k;
				Turn_PD.Error[k] = Angle - 0;	// With Drive assist, the target angle is always 0

				// Calculate Derivative
				Angular_Velocity = (1000./(12.*Drive_Refresh_Time))*( 25*Turn_PD.Error[k]
				                                                    - 48*Turn_PD.Error[mod(k-1,5)]
				                                                    + 36*Turn_PD.Error[mod(k-2,5)]
				                                                    - 16*Turn_PD.Error[mod(k-3,5)]
				                                                    +  3*Turn_PD.Error[mod(k-4,5)]);

				/* Assign R correction
				      If the Error is positive, then the robot has rotated counter-clock-wise
				      relative to equilibrium. We need a negative torque to get back to
				      equilibrium (-kx). This means speeding up the Left and slowing down the right.

				      If the error is increasing (positive angular velocity), then the robot is
				      rotating in the ccw direction. We need to damp this out with a clockwise torque
				      (-d*x'). This again means speeding up the Left and slowing down the right.
				*/
				R_Drive_Correction = (signed char)(-P_Drive_Assist*Turn_PD.Error[k] - D_Drive_Assist*Angular_Velocity);

				L_Drive_Correction = -R_Drive_Correction;
				/* L correction is neg of R correction. To understand why this is, we need to consider
				   how this code works. If the Robot has rotated a clockwise angle, then the angle will
				   will be positive and the error will be negative. A clockwise error implies that the
				   right side of the drive has gotten ahead of the left. Thus, we need to slow down the
				   right and speed up the left. This means adding the correction terms onto the R and
				   subtracting them from the L. Therefore, the L correction is the negative of the R correction
				   The same is true, but with all the signs reversed, when the robot is moving backwards.
				*/

				// Print corrections to debug stream (if connected)... mostly for tuning
				//writeDebugStreamLine("L: %i R: %i",L_Drive_Correction, R_Drive_Correction);

				wait1Msec(Drive_Refresh_Time);
			} // while (Drive_Assist_Enable)
			//// shut down, reset global target values ////
			Turn_PD.Offset = Turn_PD.Error[k];
			L_Drive_Correction = 0;
			R_Drive_Correction = 0;
		}	// if(Drive_Assist_Enable)
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
