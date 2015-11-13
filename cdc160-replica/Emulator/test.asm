//
//		Test program
//
		exf 	xfn									// select typewriter output 4210
		exf 	xfn2 								// request status 4240
		ina											// read status.
		lda 	#0
		jz 		output
		hlt

output	otc 	#43
		lda		data1
		out 	pdata1
		data2
		err

pdata1 	data1
data1	ttr 	ABC./
data2
xfn 	4210
xfn2 	4240

