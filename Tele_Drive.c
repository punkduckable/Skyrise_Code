// Joystick Velocity Control
task Joystick_Velocity_Control() {
	////////////////////////////////////////////////////////////////////////
	// Set up variables
	Byte Current_L_Joystick;
	Byte Old_L_Joystick;
	int Change_In_L_Joystick;
	Byte Sign_Change_In_L_Joystick;

	Byte Current_R_Joystick;
	Byte Old_R_Joystick;
	int Change_In_R_Joystick;
	Byte Sign_Change_In_R_Joystick;

	////////////////////////////////////////////////////////////////////////
	// Assign Old/Current values
	Current_L_Joystick = vexRT[Ch3];
	Old_L_Joystick = L_Joystick;

	Current_R_Joystick = vexRT[Ch2];
	Old_R_Joystick = R_Joystick;

	while(true) {

		////////////////////////////////////////////////////////////////////////
		// Update current values;
		Current_L_Joystick = vexRT[Ch3];
		Current_R_Joystick = vexRT[Ch2];

		////////////////////////////////////////////////////////////////////////
		// Update change values
		Change_In_L_Joystick = Current_L_Joystick - Old_L_Joystick;
		Sign_Change_In_L_Joystick = (Change_In_L_Joystick > 0) - (Change_In_L_Joystick < 0);

		Change_In_R_Joystick = Current_R_Joystick - Old_R_Joystick;
		Sign_Change_In_R_Joystick = (Change_In_R_Joystick > 0) - (Change_In_R_Joystick < 0);


		////////////////////////////////////////////////////////////////////////
		// Compare to previous value, change L/R joystick values if we're changing too quickly
		if (abs(Change_In_L_Joystick) > Joystick_Change_Threshold) {
			Current_L_Joystick = Old_L_Joystick + Sign_Change_In_L_Joystick*Joystick_Change_Threshold;
		}
		else {
			Current_L_Joystick = Current_L_Joystick;
		} // if (abs(L_Joystick_Change) > Joystick_Change_Threshold)

		if (abs(Change_In_R_Joystick) > Joystick_Change_Threshold) {
			Current_R_Joystick = Old_R_Joystick + Sign_Change_In_R_Joystick*Joystick_Change_Threshold;
		}
		else {
			Current_R_Joystick = Current_R_Joystick;
		} // if (abs(R_Joystick_Change) > Joystick_Change_Threshold)


		////////////////////////////////////////////////////////////////////////
		// Set global joystick values, loop
		L_Joystick = Current_L_Joystick;
		R_Joystick = Current_R_Joystick;

		////////////////////////////////////////////////////////////////////////
		// Update Old joystick values, wait, loop
		Old_L_Joystick = Current_L_Joystick;
		Old_R_Joystick = Current_R_Joystick;

		wait1Msec(20);
		continue;
	} // while (true)
} // task Joystick_Velocity_Control()

// Drive Task
task Tele_Drive() {
	// Declare local variables
	Byte L_Drive_Power;
	Byte R_Drive_Power;

	while(true) {
		// Update drive powers
		L_Drive_Power = (abs(vexRT[Ch3]) > Drive_Threshold) ? L_Joystick : 0;
		R_Drive_Power = (abs(vexRT[Ch2]) > Drive_Threshold) ? R_Joystick : 0;

		// Set drive
		Tele_Set_Drive(L_Drive_Power, R_Drive_Power);

		// Delay and loop
		wait1Msec(10);
		continue;
	} // while (true)
} // task Tele_Drive()


// Set drive function
void Tele_Set_Drive(Byte L_Drive_Power,Byte R_Drive_Power) {
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

	motor[Front_R_Drive] = R_Drive_Sign*Tele_Power_Array[R_Drive_Speed];
	motor[Back_R_Drive] = R_Drive_Sign*Tele_Power_Array[R_Drive_Speed];
	motor[Front_L_Drive] = L_Drive_Sign*Tele_Power_Array[L_Drive_Speed];
	motor[Back_L_Drive] = L_Drive_Sign*Tele_Power_Array[L_Drive_Speed];
} // void Tele_Set_Drive(int Power)