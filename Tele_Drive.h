// Prototypes
task JoyStick_Velocity_Control();	
	const char Joystick_Change_Threshold = 15;
	signed char L_Joystick;
	signed char R_Joystick;

task Tele_Drive();
	const char Drive_Threshold = 8;

task Tele_Set_Drive(signed char L_Drive_Power, signed char R_Drive_Power);
	signed char L_Drive_Sign;
	signed char R_Drive_Sign;
	int L_Drive_Speed;
	int R_Drive_Speed;

const char Tele_Power_Array[128] = {
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

// Joystick Velocity Control
task Joystick_Velocity_Control() {
	////////////////////////////////////////////////////////////////////////
	// Set up variables
	signed char Current_L_Joystick;
	signed char Old_L_Joystick;
	int Change_In_L_Joystick;
	signed char Sign_Change_In_L_Joystick;

	signed char Current_R_Joystick;
	signed char Old_R_Joystick;
	int Change_In_R_Joystick;
	signed char Sign_Change_In_R_Joystick;

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
	signed char L_Drive_Power;
	signed char R_Drive_Power;

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
void Tele_Set_Drive(signed char L_Drive_Power,signed char R_Drive_Power) {
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