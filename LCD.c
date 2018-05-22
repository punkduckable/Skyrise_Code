task LCD() {
	bLCDBacklight = true;
	// Define variables
	string Main_Battery;
	//string Backup_Battery;
	//string Encoders;
	string Offset;
	string Drive_Corrections;
	//string Time;

	float f = -1.23;
	//int L_Encode;
	//int R_Encode;

	// Encoder + battery loop.
	while (true) {
		// Update variables
		//L_Encode = SensorValue[L_Drive_Encoder];
		//R_Encode = SensorValue[R_Drive_Encoder];

		// Update strings
		sprintf(Main_Battery,"M:%4.2fV",f);//nImmediateBatteryLevel/1000.0);
		//sprintf(Backup_Battery,", B:%4.2fV", BackupBatteryLevel/1000.0);
		//sprintf(Time, " %dms",Drive_Timer);
		//sprintf(Encoders,"L:%4d R:%4d",L_Encode, R_Encode);
		sprintf(Drive_Corrections,"L:%3d R:%3d", L_Drive_Correction, R_Drive_Correction);
		//sprintf(Offset," %4.0f",Turn_PD.Offset);

		// 1st line
		displayLCDString(0,0,Main_Battery);
		//displayNextLCDString(Offset);
		displayLCDString(1,0,Drive_Corrections);

		wait1Msec(50);
	} // while (true)
} // task LCD()
