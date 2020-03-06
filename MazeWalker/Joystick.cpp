
#include "main.h"



extern CCamera objCamera;

extern int joystickNum;


void Joystick::Initialize()
{
		JOYCAPS   joyCaps;
		
		if(joystickNum>=0)
		{
			joyGetDevCaps(JOYSTICKID1+joystickNum, &joyCaps, sizeof(joyCaps));
			JoyHasThrottle = (joyCaps.wCaps & JOYCAPS_HASZ);
		}
		else
			JoyHasThrottle=true;
}

float* Joystick::JoyStickMove()
{

	
	JOYINFOEX joyInfoEx;

	float angle_x  = 0.0f;	
	float angle_y  = 0.0f;				
	float angle_z  = 0.0f;
	float rot_x =0.0f;
	float rot_y =0.0f;
	float net_x =0.0f;
	float net_y =0.0f;
	float net_z =0.0f;
	float net_r =0.0f;

	float joyMoveSpeed=0;
	float joyStrafeSpeed=0;
	float running=0;
	float look=0;
	float upDown=0;

	float d_updown=0;
	float d_leftright=0;
	int jump=0;
	int interact=0;

	float* ret=new float[5];
	
	
	if(JoyHasThrottle)
	{
		

		if(joystickNum>=0)
		{
			joyInfoEx.dwSize = sizeof(joyInfoEx);
			joyInfoEx.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | JOY_RETURNPOV;
			joyGetPosEx(JOYSTICKID1+joystickNum, &joyInfoEx);	
		

			jump = joyInfoEx.dwButtons & jumpButton;
		
		
			if(jumpReleased&&jump>0.5)
			{
				jumpReleased=false;
			}
			else
			{
				jumpReleased=(jump<0.5);
				jump=0;
			}

			interact = joyInfoEx.dwButtons & interactButton;
		



			if (joyInfoEx.dwPOV==18000||joyInfoEx.dwPOV==13500||joyInfoEx.dwPOV==22500)
				d_updown=-1;
			else if(joyInfoEx.dwPOV==0||joyInfoEx.dwPOV==9000||joyInfoEx.dwPOV==31500)
				d_updown=1;

			if(joyInfoEx.dwPOV==9000||joyInfoEx.dwPOV==13500||joyInfoEx.dwPOV==4500)
				d_leftright=1;
			else if(joyInfoEx.dwPOV==22500||joyInfoEx.dwPOV==27000||joyInfoEx.dwPOV==31500)
				d_leftright=-1;

			d_updown*=plateau/2; //d-pad always at max
			d_leftright*=-plateau/2;

			// Get the direction from the mouse cursor, set a resonable maneuvering speed
			angle_x = ProcessInput((float)((1.0-joyInfoEx.dwXpos/32768.0)/2.0f));
			angle_y = ProcessInput((float)((1.0-joyInfoEx.dwYpos/32768.0)/2.0f));
			angle_z = ProcessInput((float)((1.0-joyInfoEx.dwZpos/32768.0)/2.0f));


			rot_x= ProcessInput((float)((1.0-joyInfoEx.dwUpos/32768.0)/2.0f));
			rot_y= ProcessInput((float)((1.0-joyInfoEx.dwRpos/32768.0)/2.0f));

		}

		if(networkX>=0)
			net_x = ProcessInput((float)((1.0-networkX/32768.0)/2.0f));
		if(networkY>=0)
			net_y = ProcessInput((float)((1.0-networkY/32768.0)/2.0f));
		if(networkZ>=0)
			net_z = ProcessInput((float)((1.0-networkZ/32768.0)/2.0f));
		if(networkR>=0)
			net_r = ProcessInput((float)((1.0-networkR/32768.0)/2.0f));

		if(jLeftRightAxis==1)  //rotation
			look=angle_x*lookSensitivity/100.0f;
		else if(jLeftRightAxis==2)
			look=angle_y*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==3)
			look=angle_z*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==4)
			look=rot_x*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==5)
			look=rot_y*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==6)
			look=d_leftright*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==7)
			look=d_updown*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==8)
			look=net_x*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==9)
			look=net_y*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==10)
			look=net_z*lookSensitivity/100.0f; // Rotate
		else if(jLeftRightAxis==11)
			look=net_r*lookSensitivity/100.0f; // Rotate

		if(iLookAxis)
			look=-look;

		if(look!=0)
			objCamera.Rotate_View(-look*objCamera.cameraSpeedView); // Rotate

		if(jUpDownAxis==1) //looking up /down
		{
			
			upDown=angle_x*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==2)
		{
			upDown=angle_y*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==3)
		{
			upDown=angle_z*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==4)
		{
			upDown=rot_x*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==5)
		{
			upDown=rot_y*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==6)
		{
			upDown=d_leftright*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==7)
		{
			upDown=d_updown*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==8)
		{
			upDown=net_x*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==9)
		{
			upDown=net_y*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==10)
		{
			upDown=net_z*upDownSensitivity/100.0f;
		}
		else if(jUpDownAxis==11)
		{
			upDown=net_r*upDownSensitivity/100.0f;
		}
		if(iUpDownAxis)
			upDown=-upDown;
		if(upDown!=0)
			objCamera.LookUpDown(upDown*objCamera.cameraSpeedView/1.33f);


		if(jStrafeAxis==1)  //strafing
		{
			joyStrafeSpeed+=-angle_x*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==2)
		{
			joyStrafeSpeed+=-angle_y*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==3)
		{
			joyStrafeSpeed+=-angle_z*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==4)
		{
			joyStrafeSpeed+=-rot_x*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==5)
		{
			joyStrafeSpeed+=-rot_y*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==6)
		{
			joyStrafeSpeed+=-d_leftright*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==7)
		{
			joyStrafeSpeed+=-d_updown*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==8)
		{
			joyStrafeSpeed+=-net_x*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==9)
		{
			joyStrafeSpeed+=-net_y*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==10)
		{
			joyStrafeSpeed+=-net_z*strafeSensitivity/100.0f;
		}
		else if(jStrafeAxis==11)
		{
			joyStrafeSpeed+=-net_r*strafeSensitivity/100.0f;
		}
		if(iStrafeAxis)
			joyStrafeSpeed=-joyStrafeSpeed;

		if(jMoveAxis==1) //moving
		{
			joyMoveSpeed+=angle_x*moveSensitivity/100.0f;
			if(abs(angle_x)>runningThreshold)
				running=1;
		}
		else if(jMoveAxis==2)
		{
			joyMoveSpeed+=angle_y*moveSensitivity/100.0f;
			if(abs(angle_y)>runningThreshold)
				running=1;
		}
		else if(jMoveAxis==3)
		{
			joyMoveSpeed+=angle_z*moveSensitivity/100.0f;
			if(abs(angle_z)>runningThreshold)
				running=1;
		}
		else if(jMoveAxis==4)
		{
			joyMoveSpeed+=rot_x*moveSensitivity/100.0f;
			if(abs(rot_x)>runningThreshold)
				running=1;
		}
		else if(jMoveAxis==5)
		{
			joyMoveSpeed+=rot_y*moveSensitivity/100.0f;
			if(abs(rot_y)>runningThreshold)
				running=1;
		}
		else if(jMoveAxis==6)
		{
			joyMoveSpeed+=d_leftright*moveSensitivity/100.0f;
		}
		else if(jMoveAxis==7)
		{
			joyMoveSpeed+=d_updown*moveSensitivity/100.0f;
		}
		else if(jMoveAxis==8)
		{
			joyMoveSpeed+=net_x*moveSensitivity/100.0f;
		}
		else if(jMoveAxis==9)
		{
			joyMoveSpeed+=net_y*moveSensitivity/100.0f;
		}
		else if(jMoveAxis==10)
		{
			joyMoveSpeed+=net_z*moveSensitivity/100.0f;
		}else if(jMoveAxis==11)
		{
			joyMoveSpeed+=net_r*moveSensitivity/100.0f;
		}
		if(iMoveAxis)
			joyMoveSpeed=-joyMoveSpeed;
		
		ret[0]=joyMoveSpeed;
		ret[1]=joyStrafeSpeed;
		ret[2]=running;
		ret[3]=jump;
		ret[4]=interact;

		return ret;
		
	
	}
	ret[0] = -95432;
	ret[1] = 0;
	ret[2] = 0;
	ret[3] = 0;
	ret[4] = 0;

	return ret;
	}

	float Joystick::ProcessInput(float inp)
	{

		if(inp>0)
			inp=inp*inp;
		else
			inp=inp*-inp;

		if(abs(inp)<deadZone)
			inp=0;
		if(inp>plateau)
			inp=plateau;
		else if(inp<-plateau)
			inp=-plateau;

		return inp;
	}