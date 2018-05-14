task Tele_Lift()
{
	while(true)
	{
		int x = 0;
		if(vexRT[Btn5U] == 1)
		{
			x = 2;
			motor[Leftlift1] = -100;
			motor[Leftlift2] = -100;
			motor[Rightlift1] = -100;
			motor[Rightlift2] = -100;
		}
		else if(vexRT[Btn7R] == 1)
		{
			motor[Rightlift1] = -100;
			motor[Rightlift2] = -100;
		}
		else if(vexRT[Btn7L] == 1)
		{
			motor[Leftlift1] = -100;
			motor[Leftlift2] = -100;
		}
		else if(vexRT[Btn5D] == 1)
		{
			x = 2;
			motor[Rightlift1] = 100;
			motor[Rightlift2] = 100;
			motor[Leftlift1] = 100;
			motor[Leftlift2] = 100;
		}
		else if(vexRT[Btn5D] == 0 && vexRT[Btn5U] == 0)
		{
			if(x == 1)
			{
				motor[Rightlift1] = -10;
				motor[Rightlift2] = 10;
				motor[Leftlift1] = 10;
				motor[Leftlift2] = -10;
			}
			else if(x == 2)
			{
				motor[Rightlift1] = -17;
				motor[Rightlift2] = 5;
				motor[Leftlift1] = 5;
				motor[Leftlift2] = -17;
			}
			else
			{
				motor[Rightlift1] = -13;
				motor[Rightlift2] = 10;
				motor[Leftlift1] = 10;
				motor[Leftlift2] = -13;
			}
		}
	}
} // task Tele_Lift() {