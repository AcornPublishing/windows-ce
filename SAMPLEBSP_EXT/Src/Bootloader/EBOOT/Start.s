	IMPORT main
	OPT   2

	INCLUDE kxarm.h

	OPT   1
	OPT   128

	STARTUPTEXT

	; Reset -> StartUp
	LEAF_ENTRY 	StartUp

	; Main.C's main function call
	b       main

	END