// Prototypes, Global Variables
	char Skyrise_Value = 1;
	char Tail_Value = 1;

task Tele_Skyrise() {
	while(true) {
		//Skyrise Code//
		if(vexRT[Btn7U] == 1)
		{
			Skyrise_Value = (Skyrise_Value+1)%2;
			SensorValue[Skyrise] = Skyrise_Value;
			wait1Msec(200);
		} // if(vexRT[Btn7U] == 1)

		if(vexRT[Btn7D] == 1)
		{
			Tail_Value = (Tail_Value+1)%2;
			SensorValue[Tail] = Tail_Value;
			wait1Msec(200);
		} // if(vexRT[Btn7D] == 1)
	} // while (true) {
} // task Tele_Skyrise() {