

class Joystick
{
	public:
		int jMoveAxis;
		int jStrafeAxis;
		int jLeftRightAxis;
		int jUpDownAxis;
		float deadZone;
		float plateau;
		float runningThreshold;
		int jumpButton,interactButton;
		bool jumpReleased;

		int networkX,networkY,networkZ,networkR; //for API joystick control

		Joystick(){
		deadZone=0.005f;
		plateau=0.25f;
		runningThreshold=plateau*0.9f;

		networkX=-1;
		networkY=-1;
		networkZ=-1;
		networkR=-1;
		};

		float ProcessInput(float inp);
		void Initialize();

		bool JoyHasThrottle;

		int moveSensitivity,strafeSensitivity,lookSensitivity,upDownSensitivity;
		bool iMoveAxis,iStrafeAxis,iLookAxis,iUpDownAxis;

		float* JoyStickMove();
};

