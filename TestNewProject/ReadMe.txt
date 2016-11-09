Model: Tad.fbx is used, including 51 bones and 96 keyframes per animated bone

Implemented algorithm:

Path Interpolation : Using Bezier curve
Arc Length Calc: multi-segment table
Speed and Orientation: sin/cos as ease-in/out, Look at ahead of 1 unit for orientation control.

Instructions: 
	FPS-like control:
		move forward : w
		move backward: s
		move leftward: a
		move rightward: d
		move upward: left shift
		move downward: left ctrl