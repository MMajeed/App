Camera Keys:
Up and Down arrow keys to move forward and backward
Right and Left arrow keys to turn right and left
PgUp and PgDn to turn up and down

Press 1 to make the top part be idle at all time
Press 2 to turn off the partial idle top

Press W to move forward and Shift + W to run

Press NumPad Minus Key to slow down the animation speed
Press NumPad Plus Key to increase the animation speed

Input Factory:

While in Idle: 
	- Hold W to go into Walk in Cross Fade
	- Hold W and shift to go into Start Run in a Cross Fade

While in Walking:
	- Hold shift to enter Run anim in time sync
	- Stop holding W to go into a idle in a cross fade

While in Start Run:
	- Stop holding W and you will go into Idle in a cross fade
	- Once the anim is done than you will go into Run animation in a snap shot

While in Run:
	- Stop holding shift and you will go into walking in time sync
	- Stop holding W and you will go into stop running in a cross fade

While in stop running:
	- Once the anim is done you will go into idle in a snap shot

The input state machine can be found in AnimationState.cpp
