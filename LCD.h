task LCD() {
	bLCDBacklight = true;
	// Define variables
	string Bat_Main;
	string Bat_Backup;
	string Gyroscope;
	string L_Enc, R_Enc;
	string Offset;
	string Time;
	string DriveD;

	int L_Encode, R_Encode;

	// Encoder + battery loop.
	while (true) {
		// Update variables
		L_Encode = SensorValue[L_Drive_Encoder];
		R_Enccode = SensorValue[R_Drive_Encoder];

		// Update strings
		sprintf(Bat_Main,"%1.2f",nImmediateBatteryLevel/1000.0,"V");
		sprintf(Bat_Backup,"%1.2f",BackupBatteryLevel/1000.0,"V");
		sprintf(L_Enc,"%4d",L_Encode);
		sprintf(R_Enc,"%4d",R_Encode);
		//sprintf(L_Cor,"%4.0f",L_Drive_Correction);
		//sprintf(R_Cor,"%4.0f",R_Drive_Correction);
		//sprintf(Offset,"%4.0f",Drive_Offset);

		// 1st line
		displayLCDString(0,0,"L-E:");
		displayNextLCDString(L_Enc);
		displayNextLCDString(" R-E:");
		displayNextLCDString(R_Enc);

		// 2nd line
		displayLCDString(1,0,"M: ");
		displayNextLCDString(Bat_Main);

		wait1Msec(50);
	} // while (true)
} // task LCD()