# library-eval

This library provides a simple evaluation functionality for AGK2. It consists of two parts, a run time interpreter which processes arithmetic in an RPN format, and a compiler which converts normal infix notation (e.g. (a+2)*c) to that RPN format. It is done this way because the RPN format is faster to interpret, but more complex to use. 

For additional documentation see the html file which is generated using the agkdoc program.

### Examples

There is a working AGK2 example.

Basically you work out the 'program code' you want to run, and what value if any you want returned. This looks like 
standard BASIC e.g.

A = B + 2: C = C * 4: D = SIN(45)*(A+C): D*D

which apart from the last bit is standard BASIC - the last 'command', which isn't an assignment like the other 3, 
specifies the return value. (note variables at present are just A-Z)

This is then compiled into the 'RPN Object code' - the string containing the code run by the virtual machine, which 
will be something like:

@B,2,+,!C,@C,4,*,!C,45,&SIN,@A,@C,+,*,!D,@D,@D,*,

This is then run on the 'virtual machines' - basically a stack with a variable store attached. 

You can run multiple 'object codes' on each virtual machine, or the same code on multiple machines.

### Contact

paulscottrobson@gmail.com
