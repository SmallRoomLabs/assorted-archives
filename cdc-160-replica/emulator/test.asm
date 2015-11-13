//
//		Test program
//
		exf 	xfn									// select typewriter output 4210
		exf 	xfn2 								// request status 4240
		ina											// read status.
		lda 	#0
		jz 		output
		hlt

output	
		out 	pdata1
		data2

		inp 	psdata1
		sdata2

		sta 	02
		lda 	#0
		sta 	@02
		err

pdata1 	data1
psdata1 sdata1

data1	flr 	AbC.123./
data2
xfn 	4210
xfn2 	4240

sdata1 	flr 	ABCDEFGHIJKLMNO./
sdata2

