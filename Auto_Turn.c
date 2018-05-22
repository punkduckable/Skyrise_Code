void Turn(Byte Power, int Angle) {
	// Check that power is a positive number
	if (Power < 0) { return; }

	// Set drive global variables
	Turn_PD.Power = Power;
	Turn_PD.Target = Angle;
	Turn_PD.Target_Sign = (Angle > 0) - (Angle < 0);

	// Enable auto_Drive
	Turn_Enable = 1;

} // void Turn(Byte Power, int Target_Value)


task Auto_Turn() {
	// Declare Local variables
	float Angle;
	float Error[5];
	float Angular_Velocity;
	float Error_Sign;
	unsigned char Break_Out_Counter = 0;				// Conter to determine if we should stop
	int L_Drive_Power, R_Drive_Power;					// Power for left and right side of drive
	Byte i;												// For loop counters
	Byte k;												// Used to know which value of Error Array to update

	// Setup Turn_PD.
	Turn_PD.Power = 0;
	Turn_PD.Target = 0;
	Turn_PD.Target_Sign = 0;
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
			for(i = 0; i < 5; i++) {
				Error[i] = Turn_PD.Target;
			} // for(i = 0; i < 5; i++) {
			Angular_Velocity = 0;

			// Reset Iteration Counter
			k = 0;

			// Loop while drive_assist is enabled.
			while(Turn_Enable && (Break_Out_Counter < Turn_Break_Out_Counter_Limit)) {
				// Update Drive position. Turn_PD.Offset contains the last Angle Error. The angle is
				// positive if the robot has rotated clockwise about the +z axis.
				Angle = DEG_PER_TICK*.5*(SensorValue[R_Drive_Encoder] - SensorValue[L_Drive_Encoder]) - Turn_PD.Offset;

				// Update Errors.
				k++;					// Incremenet Error counter
				k = mod(k,5);
				Error[k] = Turn_PD.Target - Angle;
				Error_Sign = (Error[k] > 0) - (Error[k] < 0);

				// Calculate Derivative
				Angular_Velocity = (1000./(12.*Turn_Refresh_Time))*( 25*Error[k]
				                                                    - 48*Error[mod(k-1,5)]
				                                                    + 36*Error[mod(k-2,5)]
				                                                    - 16*Error[mod(k-3,5)]
			                      	                             +  3*Error[mod(k-4,5)]);

				/* 	Check for stall (if velocity is below threshold).
				We only run this check if we've moved 10 degrees twoards the target. This is done
				because the robot starts off not moving. Thus, the robot may think it has stalled
				when really it is just starting up. Thus, we only run this check once the robot has
				started turning for a bit.
				*/
				if( abs(Error[k]) < (abs(Turn_PD.Target)-10) && abs(Angular_Velocity) < Turn_Min_Velocity) {
					Break_Out_Counter++;
				} // if( abs(Drive_Delta) < (abs(Drive_PD.Target)-100) && abs(Velocity) < Min_Velocity) {
				else { Break_Out_Counter = 0; }			// Otherwise, reset breakout counter

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Assign Left and Right Drive_Power

				R_Drive_Power = P_Turn*Error[k] + D_Turn*Angular_Velocity;                      // Assign R (For CCW rotation, Right side should go forward, L backwards)

				//R_Drive_Power = (abs(R_Drive_Power) < Turn_Power_Minimum) ? 0 : R_Drive_Power;	// Set L to 0 if we're very close to 0 power (prevent oscillations

				// Limit max velocity (without correction) to drive power
				if (abs(R_Drive_Power) > abs(Turn_PD.Power)) {
					R_Drive_Power = (Error_Sign)*(Turn_PD.Power);
				} // if (abs(R_Drive_Power) > abs(Turn_Power)) {

				L_Drive_Power = -R_Drive_Power;				// Assign L

				writeDebugStreamLine("P: %i D: %i",P_Turn*Error[k],D_Turn*Angular_Velocity);

				// Now, set drive
				Auto_Set_Drive(L_Drive_Power, R_Drive_Power);

				wait1Msec(Turn_Refresh_Time);
			} // while (Turn_Enable)
			//// shut down, reset global target values ////
			Auto_Set_Drive(0,0);
			Turn_PD.Offset = Error[k];
			Turn_PD.Power = 0;
			Turn_PD.Target = 0;
			Turn_PD.Target_Sign = 0;
			Break_Out_Counter = 0;
			Turn_Enable = 0;
		}	// if(Turn_Enable)
		else {
			wait1Msec(20);
		} // else {
	} // while (true)
} // task Auto_Turn()
