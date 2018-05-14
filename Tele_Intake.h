task Tele_Intake() {
	while(true) {
		if(vexRT[Btn6U] == 1 && vexRT(Btn6D) == 0) {
			motor[intake1] = 100;
			motor[intake2] = 100;
		} // if(vexRT[Btn6U] == 1 && vexRT(Btn6D) == 0)

		else if(vexRT[Btn6D] == 1 && vexRT[Btn6U] == 0) {
			motor[intake1] = -80;
			motor[intake2] = -80;
		} // else if(vexRT[Btn6D] == 1 && vexRT[Btn6U] == 0)
		
		else {
			motor[intake1] = 10;
			motor[intake2] = 10;
		} // else {
	} // while(true) {
} // task Tele_Intake() {