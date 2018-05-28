void Turn(Byte Power, int Angle) {
	// Check that power is a positive number
	if (Power < 0) { return; }

	// Set drive global variables
	Turn_PD.Power = Power;
	Turn_PD.Target = Angle;

	// Enable auto_Drive
	Turn_Enable = 1;

} // void Turn(Byte Power, int Target_Value)


task Auto_Turn() {
	// Declare Local variables
	float Angle;
	float Angular_Velocity;
	float Error_Sign;
	Byte Break_Out_Counter = 0;				// Conter to determine if we should stop
	int L_Drive_Power, R_Drive_Power;					// Power for left and right side of drive
	Byte k;												// Used to know which value of Error Array to update
	unsigned int Iter;

	// Setup Turn_PD.
	Turn_PD.Power = 0;
	Turn_PD.Target = 0;
	Turn_PD.Offset = 0;

	// Loop continuously, waiting to be enabled
	while(true) {
		// If enabled, set up drive stuff
		if(Turn_Enable) {
			// Drive setup
			SensorValue[R_Drive_Encoder] = 0;    // Reset encoders
			SensorValue[L_Drive_Encoder] = 0;
			Break_Out_Counter = 0;             	 // Reset breakout counter

			// Set Turn Error
			for(k = 0; k < 5; k++) {
				Turn_PD.Error[k] = Turn_PD.Target;
			} // for(i = 0; i < 5; i++) {
			Angular_Velocity = 0;

			// Reset Iteration Counter
			k = 0;
			Iter = 0;

			// Loop while drive_assist is enabled.
			while(Turn_Enable && (Break_Out_Counter < Turn_Break_Out_Counter_Limit)) {
				// Update Drive position. Turn_PD.Offset contains the last Angle Error. The angle is
				// positive if the robot has rotated clockwise about the +z axis.
				Angle = DEG_PER_TICK*.5*(SensorValue[R_Drive_Encoder] - SensorValue[L_Drive_Encoder]) + Turn_PD.Offset;

				// Update Errors.
				k++;					// Incremenet Error counter
				k = mod(k,5);
				Iter++;
				Turn_PD.k = k;
				Turn_PD.Error[k] = Angle - Turn_PD.Target;
				Error_Sign = (Turn_PD.Error[k] > 0) - (Turn_PD.Error[k] < 0);

				// Calculate Derivative
				if(Iter >= 5)
				  Angular_Velocity = (1000./(12.*Turn_Refresh_Time))*( 25*Turn_PD.Error[mod(k,5)]
				                                                     - 48*Turn_PD.Error[mod(k-1,5)]
				                                                     + 36*Turn_PD.Error[mod(k-2,5)]
				                                                     - 16*Turn_PD.Error[mod(k-3,5)]
			                      	                               +  3*Turn_PD.Error[mod(k-4,5)]);
			  else
			  	Angular_Velocity = 0;

				/* 	Check for stall (if velocity is below threshold).
				We only run this check if we've moved 10 degrees twoards the target. This is done
				because the robot starts off not moving. Thus, the robot may think it has stalled
				when really it is just starting up. Thus, we only run this check once the robot has
				started turning for a bit.
				*/
				if( abs(Turn_PD.Error[k]) < (abs(Turn_PD.Target)-10) && abs(Angular_Velocity) < Turn_Min_Velocity) {
					Break_Out_Counter++;
				} // if( abs(Drive_Delta) < (abs(Drive_PD.Target)-100) && abs(Velocity) < Min_Velocity) {
				else { Break_Out_Counter = 0; }			// Otherwise, reset breakout counter

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Assign Left and Right Drive_Power

				/* Assign R power.
				      If the error is positive, that implies that the robot has rotated past the target
				      in the CCW direction. To fix this, we need to apply a negative torque in the cw
				      direction (-kx). This implies speeding up the left and slowing down the right.

				      If the angular velocity is positive, that implies that the robot is rotating in the
				      ccw direction. We need to damp this out with a cw torque. This implies speeding up
				      the left side and slowing down the right.
				*/
				R_Drive_Power = (signed char)(-P_Turn*Turn_PD.Error[k] - D_Turn*Angular_Velocity);

				// Limit max velocity (without correction) to drive power
				if (abs(R_Drive_Power) > abs(Turn_PD.Power)) {
					R_Drive_Power = -(Error_Sign)*(Turn_PD.Power);
				} // if (abs(R_Drive_Power) > abs(Turn_Power)) {

				L_Drive_Power = -R_Drive_Power;				// Assign L

				// Print spring and damper terms to debug stream. Mostly for debugging.
				writeDebugStreamLine("-kx: %i -dx': %i",-P_Turn*Turn_PD.Error[k],-D_Turn*Angular_Velocity);

				// Now, set drive
				Auto_Set_Drive(L_Drive_Power, R_Drive_Power);

				wait1Msec(Turn_Refresh_Time);
			} // while (Turn_Enable)
			//// shut down, reset global target values ////
			Auto_Set_Drive(0,0);
			Turn_PD.Offset = Turn_PD.Error[k];
			Turn_PD.Power = 0;
			Turn_PD.Target = 0;
			Break_Out_Counter = 0;
			Turn_Enable = 0;
		}	// if(Turn_Enable)
		else {
			wait1Msec(Turn_Refresh_Time);
		} // else {
	} // while (true)
} // task Auto_Turn()
