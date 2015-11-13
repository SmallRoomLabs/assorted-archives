		rem hello world !
		LDN		42		 			// i like chips in gravy
		org 	200
		LSN		1 	2
		JFI		loop2
		jz 		loop2
		jnz 	loop2
		jpl 	loop2
		jmi 	loop2
 		LSN		3 	-2
loop	NZF 	loop2
 		loop 	1
somex 	14 		15
		PJB 	loop
 		jfi 	loop2
loop2	LDI 	14
		jz 		loop2
		jnz 	loop2
		jpl 	loop2
		jmi 	loop2
fred 	= 65
		con 	
 		42		-4
 		prg
 		err 	3
 		blr 	4
nocode	
		and 	#37
		and 	37
		and 	@37
		and 	nocode
		and 	test
		fred 	2
		dec 	42
		flx 	Hello./
		flr 	Hello./
		rol
		rol3
		mul10
		base 	10
		42
		jmp 	@42
		jmp 	@test 
test 	64
		hlt
