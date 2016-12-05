Model: Tad.fbx is used

Manipulator depth (7-8)
Target Object drawn
Move along a path

IK algorithm : CCD
Smooth motion
constraints (line 600 - 601 of App.cpp in AnimationDemo project, commented out)

Memo:
I've faced the constraints problem days ago. 
I've implemented the constrains, but that results in the framerates under 60fps and take a lot of time when the target is far away.
So I just commented the lines out.
Since this is the last day of the second week Åi0.9 penalty), I decide to give it up, already spent several days on it.

Instructions: 
	target control:
		arrow keys to move
	camera control:
		move forward : w
		move backward: s
		move leftward: a
		move rightward: d
		move upward: left shift
		move downward: left ctrl


Open CrossRenderer.sln, compile and run, or you can directly run the executable AnimationDemo.exe inside x64/Debug/