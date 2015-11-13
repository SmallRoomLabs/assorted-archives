using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Mike2
{
    /// <summary>
    /// Base class for 8008 Emulation.
    /// </summary>
    public abstract class Intel8008
    {
        private int cycles;                                                                 // CPU Cycles. (400,000Hz on Mike)
        private int pctr;                                                                   // 13 bit program counter (actually it's a rotating array of counters)
        private int a, b, c, d, e, h, l;                                                    // 8 bit CPU Registers
        private int pszValue;                                                               // Value used to calculate Parity, Sign, Zero flags. Actual flag register doesn't exist
        private int halt;                                                                   // Halt flag. Can be woken up either using interrupt or reset.
        private int carry;                                                                  // 1 bit carry flag.
        private int[] stack = new int[7];                                                   // 7 level stack.
        private int stackPointer;                                                           // Stack pointer

        private int temp8, temp16;                                                          // Temporary register.

        public abstract int readMemory(int address);                                        // Implementation dependent stuff.
        protected abstract void writeMemory(int address, int data);
        protected abstract int readPort(int address);
        protected abstract void writePort(int address, int data);
        protected abstract void resetHardware();

        /// <summary>
        /// Reset the 8008 Processor.
        /// </summary>
        public void reset()
        {
            pctr = 0;                                                                       // Start from 0
            a = b = c = d = e = h = l = 0;                                                  // Clear 8 bit registers
            pszValue = 1; carry = 0;                                                        // Clear flags. S,Z,C,PE will all be 0.
            halt = 0;                                                                       // Clear Halt.
            stackPointer = 0;                                                               // Empty Stack
            cycles = 0;                                                                     // No cycles.
            resetHardware();                                                                // Hardware can be reset as well.
        }

        /// <summary>
        /// Execute a single instruction if not halted.
        /// </summary>
        public void execute()
        {
            if (halt == 0)                                                                  // Still running.
            {
                executeInstructionDirect(fetchMemory());                                    // Fetch and execute an instruction.
            }
            else                                                                            // Stopped
            {
                cycles = cycles + 128;                                                      // Waste CPU time.
            }
        }

        /// <summary>
        /// Cause an interrupt.
        /// </summary>
        public void interrupt()
        {
            halt = 0;                                                                       // Wake processor up.
            executeInstructionDirect(0x05);                                                 // And execute "RST 0".
        }

        /// <summary>
        /// Fetch a single instruction byte
        /// </summary>
        /// <returns>Fetched byte</returns>
        private int fetchMemory()
        {
            int opcode = readMemory(pctr);                                                  // Read memory.
            pctr = (pctr + 1) & 0x3FFF;                                                     // Increment program counter, 13 bit.
            return opcode;                                                                  // Return opcode.
        }

        /// <summary>
        /// Add a value and carry to the accumulator
        /// </summary>
        /// <param name="value">value to add</param>
        /// <param name="carry">carry flag</param>
        /// <returns>the result of the calculation</returns>
        private int add(int value, int carry)
        {
            int result = a + value + carry;                                                 // Work out the result  
            carry = (result >> 8) & 1;                                                      // Get the carry bit
            return result & 0xFF;                                                           // Return 8 bits.
        }

        /// <summary>
        /// Subtract a value and borrow from the accumulator
        /// </summary>
        /// <param name="value">value to subtract</param>
        /// <param name="borrow">borrow bit (carry flag)</param>
        /// <returns>result of calculation</returns>
        private int subtract(int value, int borrow)
        {
            int result = a - value - borrow;                                                // Work out the result.
            carry = (result >> 8) & 1;                                                      // Set carry flag according to borrow.
            return result & 0xFF;                                                           // Return 8 bits.
        }

        /// <summary>
        /// Check if an integer has even parity.
        /// </summary>
        /// <param name="value">Number to check</param>
        /// <returns>true if even parity.</returns>
        private Boolean isParityEven(int value)
        {
            int count = 0;                                                                  // Number of bits set
            while (value != 0)                                                              // While there are more bits.
            {
                if ((value & 1) != 0) count++;                                              // If LSB is set bump count.
                value = value >> 1;                                                         // Shift value right.
            }
            return (count & 1) == 0;                                                        // Return true if the count is even.
        }

        /// <summary>
        /// Push program counter on the stack.
        /// </summary>
        private void pushpc()
        {
            stack[stackPointer++] = pctr;                                                   // Push on stack
        }

        /// <summary>
        /// Pop program counter off stack. If stack underflows, then set program counter to zero.
        /// </summary>
        private void poppc()
        {
            pctr = 0;                                                                       // Start with zero
            if (stackPointer > 0) pctr = stack[--stackPointer];                             // If there's something on the stack, pop it.
        }

        /// <summary>
        /// Execute the given instruction. All code in the switch is auto generated.
        /// </summary>
        /// <param name="opcode">operation code</param>
        private void executeInstructionDirect(int opcode)
        {
            switch (opcode)
            {
                case 0x00:     //  ****** hlt $00 ******
                    halt = 1;
                    cycles = cycles + 4;
                    break;

                case 0x01:     //  ****** hlt $01 ******
                    halt = 1;
                    cycles = cycles + 4;
                    break;

                case 0x02:     //  ****** rlc $02 ******
                    carry = (a >> 7) & 1;
                    a = ((a << 1) & 0xFF) | carry;
                    cycles = cycles + 5;
                    break;

                case 0x03:     //  ****** rnc $03 ******
                    if (carry == 0) poppc();
                    cycles = cycles + 4;
                    break;

                case 0x04:     //  ****** adi nn $04 ******
                    a = pszValue = add(fetchMemory(), 0);
                    cycles = cycles + 8;
                    break;

                case 0x05:     //  ****** rst 0 $05 ******
                    pushpc();
                    pctr = 0x0;
                    cycles = cycles + 5;
                    break;

                case 0x06:     //  ****** mvi a,nn $06 ******
                    a = fetchMemory();
                    cycles = cycles + 5;
                    break;

                case 0x07:     //  ****** ret $07 ******
                    poppc();
                    cycles = cycles + 5;
                    break;

                case 0x08:     //  ****** inr b $08 ******
                    b = (b + 1) & 0xFF;
                    pszValue = b;
                    cycles = cycles + 5;
                    break;

                case 0x09:     //  ****** dcr b $09 ******
                    b = (b - 1) & 0xFF;
                    pszValue = b;
                    cycles = cycles + 5;
                    break;

                case 0x0A:     //  ****** rrc $0A ******
                    carry = a & 1;
                    a = (a >> 1) | (carry << 7);
                    cycles = cycles + 5;
                    break;

                case 0x0B:     //  ****** rnz $0B ******
                    if (pszValue != 0) poppc();
                    cycles = cycles + 4;
                    break;

                case 0x0C:     //  ****** aci nn $0C ******
                    a = pszValue = add(fetchMemory(), carry);
                    cycles = cycles + 8;
                    break;

                case 0x0D:     //  ****** rst 8 $0D ******
                    pushpc();
                    pctr = 0x8;
                    cycles = cycles + 5;
                    break;

                case 0x0E:     //  ****** mvi b,nn $0E ******
                    b = fetchMemory();
                    cycles = cycles + 5;
                    break;

                case 0x0F:     //  ****** ret $0F ******
                    poppc();
                    cycles = cycles + 5;
                    break;

                case 0x10:     //  ****** inr c $10 ******
                    c = (c + 1) & 0xFF;
                    pszValue = c;
                    cycles = cycles + 5;
                    break;

                case 0x11:     //  ****** dcr c $11 ******
                    c = (c - 1) & 0xFF;
                    pszValue = c;
                    cycles = cycles + 5;
                    break;

                case 0x12:     //  ****** ral $12 ******
                    temp8 = (a >> 7) & 1;
                    a = ((a << 1) & 0xFF) | carry;
                    carry = temp8;
                    cycles = cycles + 5;
                    break;

                case 0x13:     //  ****** rp $13 ******
                    if ((pszValue & 0x80) == 0) poppc();
                    cycles = cycles + 4;
                    break;

                case 0x14:     //  ****** sui nn $14 ******
                    a = pszValue = subtract(fetchMemory(), 0);
                    cycles = cycles + 8;
                    break;

                case 0x15:     //  ****** rst 10 $15 ******
                    pushpc();
                    pctr = 0x10;
                    cycles = cycles + 5;
                    break;

                case 0x16:     //  ****** mvi c,nn $16 ******
                    c = fetchMemory();
                    cycles = cycles + 5;
                    break;

                case 0x17:     //  ****** ret $17 ******
                    poppc();
                    cycles = cycles + 5;
                    break;

                case 0x18:     //  ****** inr d $18 ******
                    d = (d + 1) & 0xFF;
                    pszValue = d;
                    cycles = cycles + 5;
                    break;

                case 0x19:     //  ****** dcr d $19 ******
                    d = (d - 1) & 0xFF;
                    pszValue = d;
                    cycles = cycles + 5;
                    break;

                case 0x1A:     //  ****** rar $1A ******
                    temp8 = a & 1;
                    a = (a >> 1) | (carry << 7);
                    carry = a;
                    cycles = cycles + 5;
                    break;

                case 0x1B:     //  ****** rpo $1B ******
                    if (!isParityEven(pszValue)) poppc();
                    cycles = cycles + 4;
                    break;

                case 0x1C:     //  ****** sbi nn $1C ******
                    a = pszValue = subtract(fetchMemory(), carry);
                    cycles = cycles + 8;
                    break;

                case 0x1D:     //  ****** rst 18 $1D ******
                    pushpc();
                    pctr = 0x18;
                    cycles = cycles + 5;
                    break;

                case 0x1E:     //  ****** mvi d,nn $1E ******
                    d = fetchMemory();
                    cycles = cycles + 5;
                    break;

                case 0x1F:     //  ****** ret $1F ******
                    poppc();
                    cycles = cycles + 5;
                    break;

                case 0x20:     //  ****** inr e $20 ******
                    e = (e + 1) & 0xFF;
                    pszValue = e;
                    cycles = cycles + 5;
                    break;

                case 0x21:     //  ****** dcr e $21 ******
                    e = (e - 1) & 0xFF;
                    pszValue = e;
                    cycles = cycles + 5;
                    break;

                case 0x23:     //  ****** rc $23 ******
                    if (carry != 0) poppc();
                    cycles = cycles + 4;
                    break;

                case 0x24:     //  ****** ani nn $24 ******
                    carry = 0;
                    a = pszValue = a & (fetchMemory());
                    cycles = cycles + 8;
                    break;

                case 0x25:     //  ****** rst 20 $25 ******
                    pushpc();
                    pctr = 0x20;
                    cycles = cycles + 5;
                    break;

                case 0x26:     //  ****** mvi e,nn $26 ******
                    e = fetchMemory();
                    cycles = cycles + 5;
                    break;

                case 0x27:     //  ****** ret $27 ******
                    poppc();
                    cycles = cycles + 5;
                    break;

                case 0x28:     //  ****** inr h $28 ******
                    h = (h + 1) & 0xFF;
                    pszValue = h;
                    cycles = cycles + 5;
                    break;

                case 0x29:     //  ****** dcr h $29 ******
                    h = (h - 1) & 0xFF;
                    pszValue = h;
                    cycles = cycles + 5;
                    break;

                case 0x2B:     //  ****** rz $2B ******
                    if (pszValue == 0) poppc();
                    cycles = cycles + 4;
                    break;

                case 0x2C:     //  ****** xri nn $2C ******
                    carry = 0;
                    a = pszValue = a ^ (fetchMemory());
                    cycles = cycles + 8;
                    break;

                case 0x2D:     //  ****** rst 28 $2D ******
                    pushpc();
                    pctr = 0x28;
                    cycles = cycles + 5;
                    break;

                case 0x2E:     //  ****** mvi h,nn $2E ******
                    h = fetchMemory();
                    cycles = cycles + 5;
                    break;

                case 0x2F:     //  ****** ret $2F ******
                    poppc();
                    cycles = cycles + 5;
                    break;

                case 0x30:     //  ****** inr l $30 ******
                    l = (l + 1) & 0xFF;
                    pszValue = l;
                    cycles = cycles + 5;
                    break;

                case 0x31:     //  ****** dcr l $31 ******
                    l = (l - 1) & 0xFF;
                    pszValue = l;
                    cycles = cycles + 5;
                    break;

                case 0x33:     //  ****** rm $33 ******
                    if ((pszValue & 0x80) != 0) poppc();
                    cycles = cycles + 4;
                    break;

                case 0x34:     //  ****** ori nn $34 ******
                    carry = 0;
                    a = pszValue = a | (fetchMemory());
                    cycles = cycles + 8;
                    break;

                case 0x35:     //  ****** rst 30 $35 ******
                    pushpc();
                    pctr = 0x30;
                    cycles = cycles + 5;
                    break;

                case 0x36:     //  ****** mvi l,nn $36 ******
                    l = fetchMemory();
                    cycles = cycles + 5;
                    break;

                case 0x37:     //  ****** ret $37 ******
                    poppc();
                    cycles = cycles + 5;
                    break;

                case 0x3B:     //  ****** rpe $3B ******
                    if (isParityEven(pszValue)) poppc();
                    cycles = cycles + 4;
                    break;

                case 0x3C:     //  ****** cpi nn $3C ******
                    pszValue = subtract(fetchMemory(), 0);
                    cycles = cycles + 8;
                    break;

                case 0x3D:     //  ****** rst 38 $3D ******
                    pushpc();
                    pctr = 0x38;
                    cycles = cycles + 5;
                    break;

                case 0x3E:     //  ****** mvi m,nn $3E ******
                    writeMemory((h << 8) | l, fetchMemory());
                    cycles = cycles + 8;
                    break;

                case 0x3F:     //  ****** ret $3F ******
                    poppc();
                    cycles = cycles + 5;
                    break;

                case 0x40:     //  ****** cnc nnnn $40 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (carry == 0) pctr = temp16;
                    cycles = cycles + 10;
                    break;

                case 0x41:     //  ****** in 0 $41 ******
                    a = readPort(0x0);
                    cycles = cycles + 8;
                    break;

                case 0x42:     //  ****** cnc nnnn $42 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (carry == 0)
                    {
                        pushpc();
                        pctr = temp16;
                    };
                    cycles = cycles + 10;
                    break;

                case 0x43:     //  ****** in 1 $43 ******
                    a = readPort(0x1);
                    cycles = cycles + 8;
                    break;

                case 0x44:     //  ****** jmp nnnn $44 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    pctr = temp16;
                    cycles = cycles + 11;
                    break;

                case 0x45:     //  ****** in 2 $45 ******
                    a = readPort(0x2);
                    cycles = cycles + 8;
                    break;

                case 0x46:     //  ****** call nnnn $46 ******
                    pushpc();
                    pctr = temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    cycles = cycles + 11;
                    break;

                case 0x47:     //  ****** in 3 $47 ******
                    a = readPort(0x3);
                    cycles = cycles + 8;
                    break;

                case 0x48:     //  ****** cnz nnnn $48 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (pszValue != 0) pctr = temp16;
                    cycles = cycles + 10;
                    break;

                case 0x49:     //  ****** in 4 $49 ******
                    a = readPort(0x4);
                    cycles = cycles + 8;
                    break;

                case 0x4A:     //  ****** cnz nnnn $4A ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (pszValue != 0)
                    {
                        pushpc();
                        pctr = temp16;
                    };
                    cycles = cycles + 10;
                    break;

                case 0x4B:     //  ****** in 5 $4B ******
                    a = readPort(0x5);
                    cycles = cycles + 8;
                    break;

                case 0x4C:     //  ****** jmp nnnn $4C ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    pctr = temp16;
                    cycles = cycles + 11;
                    break;

                case 0x4D:     //  ****** in 6 $4D ******
                    a = readPort(0x6);
                    cycles = cycles + 8;
                    break;

                case 0x4E:     //  ****** call nnnn $4E ******
                    pushpc();
                    pctr = temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    cycles = cycles + 11;
                    break;

                case 0x4F:     //  ****** in 7 $4F ******
                    a = readPort(0x7);
                    cycles = cycles + 8;
                    break;

                case 0x50:     //  ****** cp nnnn $50 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if ((pszValue & 0x80) == 0) pctr = temp16;
                    cycles = cycles + 10;
                    break;

                case 0x51:     //  ****** out 8 $51 ******
                    writePort(0x8, a);
                    cycles = cycles + 6;
                    break;

                case 0x52:     //  ****** cp nnnn $52 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if ((pszValue & 0x80) == 0)
                    {
                        pushpc();
                        pctr = temp16;
                    };
                    cycles = cycles + 10;
                    break;

                case 0x53:     //  ****** out 9 $53 ******
                    writePort(0x9, a);
                    cycles = cycles + 6;
                    break;

                case 0x54:     //  ****** jmp nnnn $54 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    pctr = temp16;
                    cycles = cycles + 11;
                    break;

                case 0x55:     //  ****** out a $55 ******
                    writePort(0xa, a);
                    cycles = cycles + 6;
                    break;

                case 0x56:     //  ****** call nnnn $56 ******
                    pushpc();
                    pctr = temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    cycles = cycles + 11;
                    break;

                case 0x57:     //  ****** out b $57 ******
                    writePort(0xb, a);
                    cycles = cycles + 6;
                    break;

                case 0x58:     //  ****** cpo nnnn $58 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (!isParityEven(pszValue)) pctr = temp16;
                    cycles = cycles + 10;
                    break;

                case 0x59:     //  ****** out c $59 ******
                    writePort(0xc, a);
                    cycles = cycles + 6;
                    break;

                case 0x5A:     //  ****** cpo nnnn $5A ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (!isParityEven(pszValue))
                    {
                        pushpc();
                        pctr = temp16;
                    };
                    cycles = cycles + 10;
                    break;

                case 0x5B:     //  ****** out d $5B ******
                    writePort(0xd, a);
                    cycles = cycles + 6;
                    break;

                case 0x5C:     //  ****** jmp nnnn $5C ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    pctr = temp16;
                    cycles = cycles + 11;
                    break;

                case 0x5D:     //  ****** out e $5D ******
                    writePort(0xe, a);
                    cycles = cycles + 6;
                    break;

                case 0x5E:     //  ****** call nnnn $5E ******
                    pushpc();
                    pctr = temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    cycles = cycles + 11;
                    break;

                case 0x5F:     //  ****** out f $5F ******
                    writePort(0xf, a);
                    cycles = cycles + 6;
                    break;

                case 0x60:     //  ****** cc nnnn $60 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (carry != 0) pctr = temp16;
                    cycles = cycles + 10;
                    break;

                case 0x61:     //  ****** out 10 $61 ******
                    writePort(0x10, a);
                    cycles = cycles + 6;
                    break;

                case 0x62:     //  ****** cc nnnn $62 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (carry != 0)
                    {
                        pushpc();
                        pctr = temp16;
                    };
                    cycles = cycles + 10;
                    break;

                case 0x63:     //  ****** out 11 $63 ******
                    writePort(0x11, a);
                    cycles = cycles + 6;
                    break;

                case 0x64:     //  ****** jmp nnnn $64 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    pctr = temp16;
                    cycles = cycles + 11;
                    break;

                case 0x65:     //  ****** out 12 $65 ******
                    writePort(0x12, a);
                    cycles = cycles + 6;
                    break;

                case 0x66:     //  ****** call nnnn $66 ******
                    pushpc();
                    pctr = temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    cycles = cycles + 11;
                    break;

                case 0x67:     //  ****** out 13 $67 ******
                    writePort(0x13, a);
                    cycles = cycles + 6;
                    break;

                case 0x68:     //  ****** cz nnnn $68 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (pszValue == 0) pctr = temp16;
                    cycles = cycles + 10;
                    break;

                case 0x69:     //  ****** out 14 $69 ******
                    writePort(0x14, a);
                    cycles = cycles + 6;
                    break;

                case 0x6A:     //  ****** cz nnnn $6A ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (pszValue == 0)
                    {
                        pushpc();
                        pctr = temp16;
                    };
                    cycles = cycles + 10;
                    break;

                case 0x6B:     //  ****** out 15 $6B ******
                    writePort(0x15, a);
                    cycles = cycles + 6;
                    break;

                case 0x6C:     //  ****** jmp nnnn $6C ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    pctr = temp16;
                    cycles = cycles + 11;
                    break;

                case 0x6D:     //  ****** out 16 $6D ******
                    writePort(0x16, a);
                    cycles = cycles + 6;
                    break;

                case 0x6E:     //  ****** call nnnn $6E ******
                    pushpc();
                    pctr = temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    cycles = cycles + 11;
                    break;

                case 0x6F:     //  ****** out 17 $6F ******
                    writePort(0x17, a);
                    cycles = cycles + 6;
                    break;

                case 0x70:     //  ****** cm nnnn $70 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if ((pszValue & 0x80) != 0) pctr = temp16;
                    cycles = cycles + 10;
                    break;

                case 0x71:     //  ****** out 18 $71 ******
                    writePort(0x18, a);
                    cycles = cycles + 6;
                    break;

                case 0x72:     //  ****** cm nnnn $72 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if ((pszValue & 0x80) != 0)
                    {
                        pushpc();
                        pctr = temp16;
                    };
                    cycles = cycles + 10;
                    break;

                case 0x73:     //  ****** out 19 $73 ******
                    writePort(0x19, a);
                    cycles = cycles + 6;
                    break;

                case 0x74:     //  ****** jmp nnnn $74 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    pctr = temp16;
                    cycles = cycles + 11;
                    break;

                case 0x75:     //  ****** out 1a $75 ******
                    writePort(0x1a, a);
                    cycles = cycles + 6;
                    break;

                case 0x76:     //  ****** call nnnn $76 ******
                    pushpc();
                    pctr = temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    cycles = cycles + 11;
                    break;

                case 0x77:     //  ****** out 1b $77 ******
                    writePort(0x1b, a);
                    cycles = cycles + 6;
                    break;

                case 0x78:     //  ****** cpe nnnn $78 ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (isParityEven(pszValue)) pctr = temp16;
                    cycles = cycles + 10;
                    break;

                case 0x79:     //  ****** out 1c $79 ******
                    writePort(0x1c, a);
                    cycles = cycles + 6;
                    break;

                case 0x7A:     //  ****** cpe nnnn $7A ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    if (isParityEven(pszValue))
                    {
                        pushpc();
                        pctr = temp16;
                    };
                    cycles = cycles + 10;
                    break;

                case 0x7B:     //  ****** out 1d $7B ******
                    writePort(0x1d, a);
                    cycles = cycles + 6;
                    break;

                case 0x7C:     //  ****** jmp nnnn $7C ******
                    temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    pctr = temp16;
                    cycles = cycles + 11;
                    break;

                case 0x7D:     //  ****** out 1e $7D ******
                    writePort(0x1e, a);
                    cycles = cycles + 6;
                    break;

                case 0x7E:     //  ****** call nnnn $7E ******
                    pushpc();
                    pctr = temp16 = fetchMemory();
                    temp16 = temp16 | (fetchMemory() & 0x3F);
                    cycles = cycles + 11;
                    break;

                case 0x7F:     //  ****** out 1f $7F ******
                    writePort(0x1f, a);
                    cycles = cycles + 6;
                    break;

                case 0x80:     //  ****** add a $80 ******
                    a = pszValue = add(a, 0);
                    cycles = cycles + 5;
                    break;

                case 0x81:     //  ****** add b $81 ******
                    a = pszValue = add(b, 0);
                    cycles = cycles + 5;
                    break;

                case 0x82:     //  ****** add c $82 ******
                    a = pszValue = add(c, 0);
                    cycles = cycles + 5;
                    break;

                case 0x83:     //  ****** add d $83 ******
                    a = pszValue = add(d, 0);
                    cycles = cycles + 5;
                    break;

                case 0x84:     //  ****** add e $84 ******
                    a = pszValue = add(e, 0);
                    cycles = cycles + 5;
                    break;

                case 0x85:     //  ****** add h $85 ******
                    a = pszValue = add(h, 0);
                    cycles = cycles + 5;
                    break;

                case 0x86:     //  ****** add l $86 ******
                    a = pszValue = add(l, 0);
                    cycles = cycles + 5;
                    break;

                case 0x87:     //  ****** add m $87 ******
                    a = pszValue = add(readMemory((h << 8) | l), 0);
                    cycles = cycles + 8;
                    break;

                case 0x88:     //  ****** adc a $88 ******
                    a = pszValue = add(a, carry);
                    cycles = cycles + 5;
                    break;

                case 0x89:     //  ****** adc b $89 ******
                    a = pszValue = add(b, carry);
                    cycles = cycles + 5;
                    break;

                case 0x8A:     //  ****** adc c $8A ******
                    a = pszValue = add(c, carry);
                    cycles = cycles + 5;
                    break;

                case 0x8B:     //  ****** adc d $8B ******
                    a = pszValue = add(d, carry);
                    cycles = cycles + 5;
                    break;

                case 0x8C:     //  ****** adc e $8C ******
                    a = pszValue = add(e, carry);
                    cycles = cycles + 5;
                    break;

                case 0x8D:     //  ****** adc h $8D ******
                    a = pszValue = add(h, carry);
                    cycles = cycles + 5;
                    break;

                case 0x8E:     //  ****** adc l $8E ******
                    a = pszValue = add(l, carry);
                    cycles = cycles + 5;
                    break;

                case 0x8F:     //  ****** adc m $8F ******
                    a = pszValue = add(readMemory((h << 8) | l), carry);
                    cycles = cycles + 8;
                    break;

                case 0x90:     //  ****** sub a $90 ******
                    a = pszValue = subtract(a, 0);
                    cycles = cycles + 5;
                    break;

                case 0x91:     //  ****** sub b $91 ******
                    a = pszValue = subtract(b, 0);
                    cycles = cycles + 5;
                    break;

                case 0x92:     //  ****** sub c $92 ******
                    a = pszValue = subtract(c, 0);
                    cycles = cycles + 5;
                    break;

                case 0x93:     //  ****** sub d $93 ******
                    a = pszValue = subtract(d, 0);
                    cycles = cycles + 5;
                    break;

                case 0x94:     //  ****** sub e $94 ******
                    a = pszValue = subtract(e, 0);
                    cycles = cycles + 5;
                    break;

                case 0x95:     //  ****** sub h $95 ******
                    a = pszValue = subtract(h, 0);
                    cycles = cycles + 5;
                    break;

                case 0x96:     //  ****** sub l $96 ******
                    a = pszValue = subtract(l, 0);
                    cycles = cycles + 5;
                    break;

                case 0x97:     //  ****** sub m $97 ******
                    a = pszValue = subtract(readMemory((h << 8) | l), 0);
                    cycles = cycles + 8;
                    break;

                case 0x98:     //  ****** sbb a $98 ******
                    a = pszValue = subtract(a, carry);
                    cycles = cycles + 5;
                    break;

                case 0x99:     //  ****** sbb b $99 ******
                    a = pszValue = subtract(b, carry);
                    cycles = cycles + 5;
                    break;

                case 0x9A:     //  ****** sbb c $9A ******
                    a = pszValue = subtract(c, carry);
                    cycles = cycles + 5;
                    break;

                case 0x9B:     //  ****** sbb d $9B ******
                    a = pszValue = subtract(d, carry);
                    cycles = cycles + 5;
                    break;

                case 0x9C:     //  ****** sbb e $9C ******
                    a = pszValue = subtract(e, carry);
                    cycles = cycles + 5;
                    break;

                case 0x9D:     //  ****** sbb h $9D ******
                    a = pszValue = subtract(h, carry);
                    cycles = cycles + 5;
                    break;

                case 0x9E:     //  ****** sbb l $9E ******
                    a = pszValue = subtract(l, carry);
                    cycles = cycles + 5;
                    break;

                case 0x9F:     //  ****** sbb m $9F ******
                    a = pszValue = subtract(readMemory((h << 8) | l), carry);
                    cycles = cycles + 8;
                    break;

                case 0xA0:     //  ****** and a $A0 ******
                    carry = 0;
                    a = pszValue = a & (a);
                    cycles = cycles + 5;
                    break;

                case 0xA1:     //  ****** and b $A1 ******
                    carry = 0;
                    a = pszValue = a & (b);
                    cycles = cycles + 5;
                    break;

                case 0xA2:     //  ****** and c $A2 ******
                    carry = 0;
                    a = pszValue = a & (c);
                    cycles = cycles + 5;
                    break;

                case 0xA3:     //  ****** and d $A3 ******
                    carry = 0;
                    a = pszValue = a & (d);
                    cycles = cycles + 5;
                    break;

                case 0xA4:     //  ****** and e $A4 ******
                    carry = 0;
                    a = pszValue = a & (e);
                    cycles = cycles + 5;
                    break;

                case 0xA5:     //  ****** and h $A5 ******
                    carry = 0;
                    a = pszValue = a & (h);
                    cycles = cycles + 5;
                    break;

                case 0xA6:     //  ****** and l $A6 ******
                    carry = 0;
                    a = pszValue = a & (l);
                    cycles = cycles + 5;
                    break;

                case 0xA7:     //  ****** and m $A7 ******
                    carry = 0;
                    a = pszValue = a & (readMemory((h << 8) | l));
                    cycles = cycles + 8;
                    break;

                case 0xA8:     //  ****** xor a $A8 ******
                    carry = 0;
                    a = pszValue = a ^ (a);
                    cycles = cycles + 5;
                    break;

                case 0xA9:     //  ****** xor b $A9 ******
                    carry = 0;
                    a = pszValue = a ^ (b);
                    cycles = cycles + 5;
                    break;

                case 0xAA:     //  ****** xor c $AA ******
                    carry = 0;
                    a = pszValue = a ^ (c);
                    cycles = cycles + 5;
                    break;

                case 0xAB:     //  ****** xor d $AB ******
                    carry = 0;
                    a = pszValue = a ^ (d);
                    cycles = cycles + 5;
                    break;

                case 0xAC:     //  ****** xor e $AC ******
                    carry = 0;
                    a = pszValue = a ^ (e);
                    cycles = cycles + 5;
                    break;

                case 0xAD:     //  ****** xor h $AD ******
                    carry = 0;
                    a = pszValue = a ^ (h);
                    cycles = cycles + 5;
                    break;

                case 0xAE:     //  ****** xor l $AE ******
                    carry = 0;
                    a = pszValue = a ^ (l);
                    cycles = cycles + 5;
                    break;

                case 0xAF:     //  ****** xor m $AF ******
                    carry = 0;
                    a = pszValue = a ^ (readMemory((h << 8) | l));
                    cycles = cycles + 8;
                    break;

                case 0xB0:     //  ****** ora a $B0 ******
                    carry = 0;
                    a = pszValue = a | (a);
                    cycles = cycles + 5;
                    break;

                case 0xB1:     //  ****** ora b $B1 ******
                    carry = 0;
                    a = pszValue = a | (b);
                    cycles = cycles + 5;
                    break;

                case 0xB2:     //  ****** ora c $B2 ******
                    carry = 0;
                    a = pszValue = a | (c);
                    cycles = cycles + 5;
                    break;

                case 0xB3:     //  ****** ora d $B3 ******
                    carry = 0;
                    a = pszValue = a | (d);
                    cycles = cycles + 5;
                    break;

                case 0xB4:     //  ****** ora e $B4 ******
                    carry = 0;
                    a = pszValue = a | (e);
                    cycles = cycles + 5;
                    break;

                case 0xB5:     //  ****** ora h $B5 ******
                    carry = 0;
                    a = pszValue = a | (h);
                    cycles = cycles + 5;
                    break;

                case 0xB6:     //  ****** ora l $B6 ******
                    carry = 0;
                    a = pszValue = a | (l);
                    cycles = cycles + 5;
                    break;

                case 0xB7:     //  ****** ora m $B7 ******
                    carry = 0;
                    a = pszValue = a | (readMemory((h << 8) | l));
                    cycles = cycles + 8;
                    break;

                case 0xB8:     //  ****** cmp a $B8 ******
                    pszValue = subtract(a, 0);
                    cycles = cycles + 5;
                    break;

                case 0xB9:     //  ****** cmp b $B9 ******
                    pszValue = subtract(b, 0);
                    cycles = cycles + 5;
                    break;

                case 0xBA:     //  ****** cmp c $BA ******
                    pszValue = subtract(c, 0);
                    cycles = cycles + 5;
                    break;

                case 0xBB:     //  ****** cmp d $BB ******
                    pszValue = subtract(d, 0);
                    cycles = cycles + 5;
                    break;

                case 0xBC:     //  ****** cmp e $BC ******
                    pszValue = subtract(e, 0);
                    cycles = cycles + 5;
                    break;

                case 0xBD:     //  ****** cmp h $BD ******
                    pszValue = subtract(h, 0);
                    cycles = cycles + 5;
                    break;

                case 0xBE:     //  ****** cmp l $BE ******
                    pszValue = subtract(l, 0);
                    cycles = cycles + 5;
                    break;

                case 0xBF:     //  ****** cmp m $BF ******
                    pszValue = subtract(readMemory((h << 8) | l), 0);
                    cycles = cycles + 8;
                    break;

                case 0xC0:     //  ****** mov a,a $C0 ******
                    cycles = cycles + 5;
                    break;

                case 0xC1:     //  ****** mov a,b $C1 ******
                    a = b;
                    cycles = cycles + 5;
                    break;

                case 0xC2:     //  ****** mov a,c $C2 ******
                    a = c;
                    cycles = cycles + 5;
                    break;

                case 0xC3:     //  ****** mov a,d $C3 ******
                    a = d;
                    cycles = cycles + 5;
                    break;

                case 0xC4:     //  ****** mov a,e $C4 ******
                    a = e;
                    cycles = cycles + 5;
                    break;

                case 0xC5:     //  ****** mov a,h $C5 ******
                    a = h;
                    cycles = cycles + 5;
                    break;

                case 0xC6:     //  ****** mov a,l $C6 ******
                    a = l;
                    cycles = cycles + 5;
                    break;

                case 0xC7:     //  ****** mov a,m $C7 ******
                    a = readMemory((h << 8) | l);
                    cycles = cycles + 8;
                    break;

                case 0xC8:     //  ****** mov b,a $C8 ******
                    b = a;
                    cycles = cycles + 5;
                    break;

                case 0xC9:     //  ****** mov b,b $C9 ******
                    cycles = cycles + 5;
                    break;

                case 0xCA:     //  ****** mov b,c $CA ******
                    b = c;
                    cycles = cycles + 5;
                    break;

                case 0xCB:     //  ****** mov b,d $CB ******
                    b = d;
                    cycles = cycles + 5;
                    break;

                case 0xCC:     //  ****** mov b,e $CC ******
                    b = e;
                    cycles = cycles + 5;
                    break;

                case 0xCD:     //  ****** mov b,h $CD ******
                    b = h;
                    cycles = cycles + 5;
                    break;

                case 0xCE:     //  ****** mov b,l $CE ******
                    b = l;
                    cycles = cycles + 5;
                    break;

                case 0xCF:     //  ****** mov b,m $CF ******
                    b = readMemory((h << 8) | l);
                    cycles = cycles + 8;
                    break;

                case 0xD0:     //  ****** mov c,a $D0 ******
                    c = a;
                    cycles = cycles + 5;
                    break;

                case 0xD1:     //  ****** mov c,b $D1 ******
                    c = b;
                    cycles = cycles + 5;
                    break;

                case 0xD2:     //  ****** mov c,c $D2 ******
                    cycles = cycles + 5;
                    break;

                case 0xD3:     //  ****** mov c,d $D3 ******
                    c = d;
                    cycles = cycles + 5;
                    break;

                case 0xD4:     //  ****** mov c,e $D4 ******
                    c = e;
                    cycles = cycles + 5;
                    break;

                case 0xD5:     //  ****** mov c,h $D5 ******
                    c = h;
                    cycles = cycles + 5;
                    break;

                case 0xD6:     //  ****** mov c,l $D6 ******
                    c = l;
                    cycles = cycles + 5;
                    break;

                case 0xD7:     //  ****** mov c,m $D7 ******
                    c = readMemory((h << 8) | l);
                    cycles = cycles + 8;
                    break;

                case 0xD8:     //  ****** mov d,a $D8 ******
                    d = a;
                    cycles = cycles + 5;
                    break;

                case 0xD9:     //  ****** mov d,b $D9 ******
                    d = b;
                    cycles = cycles + 5;
                    break;

                case 0xDA:     //  ****** mov d,c $DA ******
                    d = c;
                    cycles = cycles + 5;
                    break;

                case 0xDB:     //  ****** mov d,d $DB ******
                    cycles = cycles + 5;
                    break;

                case 0xDC:     //  ****** mov d,e $DC ******
                    d = e;
                    cycles = cycles + 5;
                    break;

                case 0xDD:     //  ****** mov d,h $DD ******
                    d = h;
                    cycles = cycles + 5;
                    break;

                case 0xDE:     //  ****** mov d,l $DE ******
                    d = l;
                    cycles = cycles + 5;
                    break;

                case 0xDF:     //  ****** mov d,m $DF ******
                    d = readMemory((h << 8) | l);
                    cycles = cycles + 8;
                    break;

                case 0xE0:     //  ****** mov e,a $E0 ******
                    e = a;
                    cycles = cycles + 5;
                    break;

                case 0xE1:     //  ****** mov e,b $E1 ******
                    e = b;
                    cycles = cycles + 5;
                    break;

                case 0xE2:     //  ****** mov e,c $E2 ******
                    e = c;
                    cycles = cycles + 5;
                    break;

                case 0xE3:     //  ****** mov e,d $E3 ******
                    e = d;
                    cycles = cycles + 5;
                    break;

                case 0xE4:     //  ****** mov e,e $E4 ******
                    cycles = cycles + 5;
                    break;

                case 0xE5:     //  ****** mov e,h $E5 ******
                    e = h;
                    cycles = cycles + 5;
                    break;

                case 0xE6:     //  ****** mov e,l $E6 ******
                    e = l;
                    cycles = cycles + 5;
                    break;

                case 0xE7:     //  ****** mov e,m $E7 ******
                    e = readMemory((h << 8) | l);
                    cycles = cycles + 8;
                    break;

                case 0xE8:     //  ****** mov h,a $E8 ******
                    h = a;
                    cycles = cycles + 5;
                    break;

                case 0xE9:     //  ****** mov h,b $E9 ******
                    h = b;
                    cycles = cycles + 5;
                    break;

                case 0xEA:     //  ****** mov h,c $EA ******
                    h = c;
                    cycles = cycles + 5;
                    break;

                case 0xEB:     //  ****** mov h,d $EB ******
                    h = d;
                    cycles = cycles + 5;
                    break;

                case 0xEC:     //  ****** mov h,e $EC ******
                    h = e;
                    cycles = cycles + 5;
                    break;

                case 0xED:     //  ****** mov h,h $ED ******
                    cycles = cycles + 5;
                    break;

                case 0xEE:     //  ****** mov h,l $EE ******
                    h = l;
                    cycles = cycles + 5;
                    break;

                case 0xEF:     //  ****** mov h,m $EF ******
                    h = readMemory((h << 8) | l);
                    cycles = cycles + 8;
                    break;

                case 0xF0:     //  ****** mov l,a $F0 ******
                    l = a;
                    cycles = cycles + 5;
                    break;

                case 0xF1:     //  ****** mov l,b $F1 ******
                    l = b;
                    cycles = cycles + 5;
                    break;

                case 0xF2:     //  ****** mov l,c $F2 ******
                    l = c;
                    cycles = cycles + 5;
                    break;

                case 0xF3:     //  ****** mov l,d $F3 ******
                    l = d;
                    cycles = cycles + 5;
                    break;

                case 0xF4:     //  ****** mov l,e $F4 ******
                    l = e;
                    cycles = cycles + 5;
                    break;

                case 0xF5:     //  ****** mov l,h $F5 ******
                    l = h;
                    cycles = cycles + 5;
                    break;

                case 0xF6:     //  ****** mov l,l $F6 ******
                    cycles = cycles + 5;
                    break;

                case 0xF7:     //  ****** mov l,m $F7 ******
                    l = readMemory((h << 8) | l);
                    cycles = cycles + 8;
                    break;

                case 0xF8:     //  ****** mov m,a $F8 ******
                    writeMemory((h << 8) | l, a);
                    cycles = cycles + 7;
                    break;

                case 0xF9:     //  ****** mov m,b $F9 ******
                    writeMemory((h << 8) | l, b);
                    cycles = cycles + 7;
                    break;

                case 0xFA:     //  ****** mov m,c $FA ******
                    writeMemory((h << 8) | l, c);
                    cycles = cycles + 7;
                    break;

                case 0xFB:     //  ****** mov m,d $FB ******
                    writeMemory((h << 8) | l, d);
                    cycles = cycles + 7;
                    break;

                case 0xFC:     //  ****** mov m,e $FC ******
                    writeMemory((h << 8) | l, e);
                    cycles = cycles + 7;
                    break;

                case 0xFD:     //  ****** mov m,h $FD ******
                    writeMemory((h << 8) | l, h);
                    cycles = cycles + 7;
                    break;

                case 0xFE:     //  ****** mov m,l $FE ******
                    writeMemory((h << 8) | l, l);
                    cycles = cycles + 7;
                    break;

                case 0xFF:     //  ****** hlt $FF ******
                    halt = 1;
                    cycles = cycles + 4;
                    break;


            }
        }
    }
}
