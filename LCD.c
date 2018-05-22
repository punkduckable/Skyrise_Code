task LCD() {
	bLCDBacklight = true;
	// Define variables
	string Main_Battery;
	//string Backup_Battery;
	string Encoders;
	//string Offset;
	string Time;

	int L_Encode;
	int R_Encode;

	// Encoder + battery loop.
	while (true) {
		// Update variables
		L_Encode = SensorValue[L_Drive_Encoder];
		R_Encode = SensorValue[R_Drive_Encoder];

		// Update strings
		sprintf(Main_Battery,"M:%4.2fV",nImmediateBatteryLevel/1000.0);
		//sprintf(Backup_Battery,", B:%4.2fV", BackupBatteryLevel/1000.0);
		sprintf(Time, " %dms",Drive_Timer);
		sprintf(Encoders,"L:%4d R:%4d",L_Encode, R_Encode);
		//sprintf(L_Cor,"%4.0f",L_Drive_Correction);
		//sprintf(R_Cor,"%4.0f",R_Drive_Correction);
		//sprintf(Offset,"%4.0f",Drive_Offset);

		// 1st line
		displayLCDString(0,0,Encoders);
		// 2nd line
		displayLCDString(1,0,Main_Battery);
		displayNextLCDString(Time);

		wait1Msec(50);
	} // while (true)
} // task LCD()
