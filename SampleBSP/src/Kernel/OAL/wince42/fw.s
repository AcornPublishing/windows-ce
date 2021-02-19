
	OPT	2
	
	INCLUDE kxarm.h

	OPT	1
	OPT	128

	IMPORT	KernelStart

    STARTUPTEXT
    LEAF_ENTRY StartUp
	
	add		r0, pc, #OEMAddressTable - (. + 8)

	bl		KernelStart
	
	INCLUDE map.a

	END

