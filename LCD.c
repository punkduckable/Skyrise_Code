task LCD() {
	bLCDBacklight = true;
	// Define variables
	string Batteries;
	string Encoders;
	//string Offset;
	//string Time;

	int L_Encode;
	int R_Encode;

	// Encoder + battery loop.
	while (true) {
		// Update variables
		L_Encode = SensorValue[L_Drive_Encoder];
		R_Encode = SensorValue[R_Drive_Encoder];

		// Update strings
		sprintf(Batteries,"M:%4.2fV, B:%4.2fV", nImmediateBatteryLevel/1000.0, BackupBatteryLevel/1000.0);
		sprintf(Encoders,"L:%4d R:%4d",L_Encode, R_Encode);
		//sprintf(L_Cor,"%4.0f",L_Drive_Correction);
		//sprintf(R_Cor,"%4.0f",R_Drive_Correction);
		//sprintf(Offset,"%4.0f",Drive_Offset);

		// 1st line
		displayLCDString(0,0,Encoders);
		// 2nd line
		displayLCDString(1,0,Batteries);

		wait1Msec(50);
	} // while (true)
} // task LCD()
