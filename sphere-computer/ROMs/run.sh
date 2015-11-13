# Assemble and display errors if any.
crasm -l pds.asm
# Generate object file and listing file
crasm -o pds.s19 pds.asm >pds.lst
# Generate .bin file from s19 file.
srec_cat pds.s19 -offset -0xfc00  -o pds.bin -binary
