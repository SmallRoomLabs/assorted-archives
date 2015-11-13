---
---			6502 Processor 
---

local modes = { }

modes.a = { eac = "FETCHWORD();eac = temp16;", cycles = 0, desc = "@2" }
modes.ax = { eac = "FETCHWORD();eac = temp16+x;", cycles = 0, desc = "@2,X" }
modes.ay = { eac = "FETCHWORD();eac = temp16+y;", cycles = 0, desc = "@2,Y" }
modes.z = { eac = "eac = FETCH();", cycles = -1, desc = "@1" }
modes.zx = { eac = "eac = (FETCH()+x) & 0xFF;", cycles = 0, desc = "@1,X" }
modes.zy = { eac = "eac = (FETCH()+y) & 0xFF;", cycles = 0, desc = "@1,Y" }
modes.i = { eac = "", cycles = -2, desc = "#@1" }
modes.id = { eac = "temp16 = FETCHWORD();eac = READWORD(temp16);", cycles = 2, desc = "(@2)" }
modes.ix = { eac = "temp8 = FETCH()+x;eac = READWORD01(temp8);", cycles = 3, desc = "(@1,X)" }
modes.iy = { eac = "temp8 = FETCH();eac = READWORD01(temp8)+y;", cycles = 2, desc = "(@1),Y" }

local codeBlock = {} 			
local mnemonicList = {}
local codeList = {}

function defineOpcode(opcode,mnemonic,code,cycles) 
	assert(mnemonicList[opcode] == nil,"Duplicate")
	mnemonicList[opcode] = mnemonic
	codeList[opcode] = "CYCLES("..cycles..");"..code
	--print(("%02x"):format(opcode),mnemonic,code)
end

local src = io.open("6502.def")
for r in src:lines() do
	local n = r:find("//") if n ~= nil then r = r:sub(1,n-1) end
	r = r:match("^%s*(.*)$")
	while r:match("%s$") do r = r:sub(1,#r-1) end
	if r:sub(1,1) == ":" then
		codeBlock[#codeBlock+1] = r:sub(2)
		r = ""
	end
	if #r ~= 0 then													-- simple address mode instructions
		local mnemonicBase,cycles,opcodes,code = r:match('^"([%w%s%@]+)"%s+(%d+)%s+([%(%)%w%,%:]+)%s+(.*)$')
		assert(mnemonicBase ~= nil,"Syntax error .. " .. r)
		if #opcodes == 2 then 
			defineOpcode(tonumber(opcodes,16),mnemonicBase,code,cycles)
		else
			opcodes = opcodes:match("^%((.*)%)$") .. ","			-- multiple address mode instructions
			assert(opcodes ~= nil,"Syntax error in opcodes .. " .. r)
			repeat 
				local mode,op
				mode,op,opcodes = opcodes:match("^(%w+)%:(%w+)%,(.*)$")
				local descr = modes[mode:lower()]
				local body = code
				if mode:lower():sub(1,1) == "z" then				-- optimise Z,ZX,ZY to use READ01()
					body = body:gsub("READ%(","READ01("):gsub("WRITE%(","WRITE01(")
				end
				if mode:lower() == "i" then 						-- if immediate no EAC,but use FETCH() to retrieve operand.
					body = body:gsub("READ%(eac%)","FETCH()")
				end
				op = tonumber(op,16)
				defineOpcode(op,mnemonicBase:gsub("@O",descr.desc),body:gsub("@EAC",descr.eac),cycles+descr.cycles)
			 until #opcodes == 0 
		end
	end
end

for i = 0,255 do
	if mnemonicList[i] == nil then mnemonicList[i] = ("byte %02X"):format(i) end
	mnemonicList[i] = mnemonicList[i]:lower()
end

hCode = io.open("6502_autocode.h","w")
for _,l in ipairs(codeBlock) do hCode:write(l.."\n") end
hCode:close()

hMnemonics = io.open("6502_mnemonics.h","w")
mnelist = '"'..table.concat(mnemonicList,'","',0,255)..'"'
hMnemonics:write("static char *_mnemonics[256] = { "..mnelist.."};")
hMnemonics:close()

hCase = io.open("6502_opcodes.h","w")
for i = 0,255 do
	if codeList[i] ~= nil then
		hCase:write(("case 0x%02x: /* $%02x:%s */\n"):format(i,i,mnemonicList[i]))
		hCase:write("    " .. codeList[i]..";break;\n")
	end
end
hCase:close()
