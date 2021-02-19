	OPT 2
  
	STARTUPTEXT
	LEAF_ENTRY StartUp
    
	B	ResetHandler
	B	Undefined_Handler
	B	SWI_Handler
	B	PrefetchAbort_Handler
	B	DataAbort_Handler
	B	.
	B	IRQ_Handler
	B	FIQ_Handler

ResetHandler
	B	.

Undefined_Handler
	B	.

SWI_Handler
	B	.

PrefetchAbort_Handler
	B	.

DataAbort_Handler
	B	.

IRQ_Handler
	B	.

FIQ_Handler
	B	.

	END

