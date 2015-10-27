The code 

If you're ssh'ed into CDF, make sure that you enable X11 forwarding.
    ie. ssh -X username@cdf.toronto.edu

Step 1.  Enter directory example<#>/, where <#> is the example number.
Step 2.  Type 'setenv LD_LIBRARY_PATH "<installation directory>/local/lib/"'.  Use the command 'pwd' to find the full installation directory path.
Step 3.  Type 'gcc -o main main.cpp -I../local/include -L../local/lib -lGL -lGLU -lglut -lIL -lILU -lILUT' to compile the code.
Step 4.  Run the code by typing ./main


COMMANDS
	1 : Toggle specularity
	3 : Toggle wireframe
	5 : Toggle depth testing
	6 : Toggle culling (try disabling both depth testing and culling)
	7 : Toggle between smooth and flat shading
	8 : Toggle texture
	
	o,O : Toggle teapot rotation along the y-axis
	p,P : Toggle teapot rotation along the x-axis
	r,R : Activate "reset board option"
	y,Y : Confirm  "reset board option"
	n,N : Cancel   "reset board option"
	v,V : Set color to white (This makes testing a bit faster)
	b,B : Set color to black (This makes testing a bit faster)
	
	wasd  : Move a piece around the board
	enter : Place a piece on the board

	ON RIGHT CLICK AND HOLD MOUSE:
		slide left  : move camera left
		slide right : move camera right
		slide up    : zoom camera in
		slide down  : zoom camera out
