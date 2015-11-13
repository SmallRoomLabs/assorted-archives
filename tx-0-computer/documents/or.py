#
#	Or (?)
#


n1 = 130772
n2 = 135341

print(n1|n2)





		cla 
		adi 	n1 											; get n1
		cyl 												; bit 15 -> 0 
		sto 	n1
		cll 												; 000 xxx with bit 15 in bit 0
		cyr 												; now bit 15 is on its own.
		clr
		sto 	temp
		cla 	
		adi 	n2
		cyl 	
		sto 	n2
		cll	
		cyr
		clr
		add 	temp 										; bit 0 is now '1' if both bits are 1.
		cll
		cyr 												; back to bit 15.
		add 	result 										; add to result.
		sto 	result

		


abcdef
abcdef


