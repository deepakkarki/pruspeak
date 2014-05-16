## This is the parser cum code generator module for PRU Speak
## The only thing exposed from this module is the parser object
## The parser object takes in a BS instruction and returns the bytecode encoding
## If there is an error (syntax or semantic); None is returned; in the future throw custom Exception

import ply.yacc as yacc
from bs_lex import *
from node import *

#**********INCASE opcodes get over, we can compress a few later***************
		
#keeps track of next available memory offset to place a variable
pru_var_count = 0
#max variable slots
PRU_VAR_AREA = 239

#size of each inst in the script
#used by GOTO to find exact location to jump to
script_inst_size = []
#e.g. [1,1,1,2,2,2,2]

#Are the incoming instruction parts of scripts?
script_mode = False

#does the variable to memory mapping work
pru_vars = {
		#e.g. "var1" : 0x02,
}

#does the array to (address, size) mapping.
pru_arrs = {
		#e.g. "arr1" : (0x10, 5) 
		#start, size
	'DIO' : (240, 16),
	'AIO' : (248, 8)
}

def BYTE(val, n):
	'''
	makes value 'val' the 'n'th byte in return value
	e.g. BYTE(0x13, 3) will return a number with 0x13 as the 3rd byte
	here pos will be from 0 to 3
	'''
	return val << (n*8)

def SET_BIT(val, n):
	'''
	returns val with the Nth bit set high
	'''
	return val | (1 << n)

def CLEAR_BIT(val, n):
	'''
	returns val with the Nth bit set low
	'''
	return val & ~(1 << n)

def pack_byte(b3, b2, b1, b0):
	'''
	takes 4 bytes and packs it into an integer
	'''
	if len (filter(lambda x : x < 256, (b3, b2, b1, b0))) < 4:
		print "error - param > 256 ", b3,b2,b1,b0
		return 0
		
	return b3 << 3*8 | b2 << 2*8 | b1 << 1*8 | b0

def get_var(val):
	'''
	takes a val object, value is a var or arr[const]
	returns the value
	'''
	if val.arr_const:
		#val1 is an const indexed array

		#check out of bounds
		arr, size = pru_arrs[val.val[0]]
		size -= 1
		index = val.val[1]
		if(index < size):
			return pru_arrs[val.val[0]][0] + index + 1
		else:
			print "array out of bounds"
			return -1 #throw exception later
	else:
		#val1 is an VAR
		#return pru_vars[val.val]
		global pru_var_count	
		val_index = pru_vars.get(val.val, None)
		if val_index == None:
			pru_vars[val.val] = pru_var_count
			val_index = pru_var_count
			pru_var_count += 1	
		return val_index


def byte_code_set_r(val1, val2):
	'''
	encodes instructions of the form SET DIO[a], arr[b]; 
	where DIO can replaced by PWM, TMR, AI etc
	'''
	#opcodes used 1-15
	OPCODE_SET = {
				'DIO' 	: 0x01,
				'PWM' 	: 0x04,
				'AIO'	: 0x07,
				'TONE'	: 0x0A,
				'TMR'	: 0x0D #till 0x0F
				}
	
	OPCODE = OPCODE_SET[val1.val[0]] #byte3
	byte0 = 0
	byte1 = 0
	byte2 = 0
	
	#x == val1.val[1] (val1.val[0] = 'DIO' )
	#y == val2.val
	if val2.arr_var:
		#SET DIO[x], y;  where x is C/V
		#and y is an variable indexed array value.
		
		if not val1.arr_var:
			#x is a constant
			OPCODE += 1
		else :
			#x is a variable
			OPCODE += 2
			
		byte2 = pru_vars.get(val1.val[1], val1.val[1])
		#if x is a const, it is returned; else value of variable in pru_vars is returned
		
		byte1 = pru_arrs[val2.val[0]][0]
		#gets the address where pru mem is located
		
		byte0 = pru_vars[val2.val[1]]
		
	else:
		#SET DIO[x], y; where x is C/V
		#and y is C/V/arr[const_val]
		
		if val1.arr_var:
			#implies x is an V
			byte2 |= 1<<7
		
		if val2.type == 'VAR' or val2.type == 'ARR':
			#implies y is a V
			byte2 |= 1<<6
			
		byte1 = pru_vars.get(val1.val[1], val1.val[1])
		#if x is a const, it is returned; else value of variable in pru_vars is returned
		
		if val2.type == 'INT':
			#y is an constant
			byte0 = val2.val	
		
		else :
			#y is  V or Arr[c]
			byte0 = get_var(val2)
			if byte0 == -1:
				return 0
			
	#pack all the bytes
	return pack_byte(OPCODE, byte2, byte1, byte0)
	
	print OPCODE, byte2, byte1, byte0  #packed_bytes		
				

def byte_code_set(val1, val2):
	'''
	encodes instruction of the form SET val1, val2
	val1 is V/Arr ; val2 can be C/V/Arr(with const or var index)
	'''
	global pru_var_count
	#opcodes used 16, 17, 18 -19 is reserved, just incase for future
	OPCODE = 0x10
	byte0, byte1, byte2 = 0, 0, 0
	byte4, byte5, byte6, byte7 = 0, 0, 0, 0
	
	if val1.arr_var or val2.arr_var:
		#it is a 64 bit instruction
		#SET x, y; x or y or both are of form 'Arr[var]'
		OPCODE += 2
		
		if val1.arr_var:
		#x is an arr[var], y can be anything 
			byte0 = pru_vars[val1.val[1]]
			byte1 = pru_arrs[val1.val[0]][0]
			byte2 |= 0b10 << 6
			
			if val2.type == 'INT':
			#y is a Const
				byte2 |= 0b00 << 4
				byte4 = val2.val & 0xFF
				byte5 = (val2.val >> 8) & 0xFF
			
			elif val2.any_var:
			#y is a var
				byte2 |= 0b01 << 4
				byte4 = get_var(val2)
				if byte4 == -1:
					return 0
			
			else:
			#y is arr[var]
				byte2 |= 0b10 <<4
				byte4 = pru_vars[val2.val[1]] #var
				byte5 = pru_arrs[val2.val[0]][0] #arr
				
		else:
		#y is arr[var], x is var/arr[const]
			if val1.any_var:
				byte2 |= 0b01 << 4
				
				if val1.type == 'INT':	
				#error
					return 0

				if val1.arr_const:
				#val1 is an const indexed array
					byte0 = get_var(val1)
				else:
				#val1 is an VAR
					byte0 = pru_vars.get(val1.val, None)
					if byte0 == None:
						pru_vars[val1.val] = pru_var_count
						byte0 = pru_var_count
						pru_var_count += 1	

			byte2 |= 0b10 << 4
			byte4 = pru_vars[val2.val[1]]
			byte5 = pru_arrs[val2.val[0]][0]
		
		return pack_byte(OPCODE, byte2, byte1, byte0), pack_byte(byte7, byte6, byte5, byte4)
	else:
	#32 bit inst SET x, y
	#x is var or arr[const]; occupies byte2
	
		if val1.arr_const:
		#val1 is an const indexed array
			byte2 = get_var(val1)
			if byte2 == -1:
				return 0
		else:
		#val1 is an VAR
			byte2 = pru_vars.get(val1.val, None)
			if byte2 == None:
				pru_vars[val1.val] = pru_var_count
				byte2 = pru_var_count
				pru_var_count += 1
		
		if val2.type == "INT": 
		#type1 : SET x, y; y is a constant
			byte0 = val2.val & 0xFF #low 8 bits
			byte1 = (val2.val >> 8) & 0xFF #high 8 bits
			
		else: 
		#type2 : SET x, y; y is a variable
			OPCODE += 1
			byte0 = get_var(val2)

		#return the byte encoded information
		return pack_byte(OPCODE, byte2, byte1, byte0)
	
	#redunt - could be useful for debugging
	byte3 = OPCODE
	print byte3, byte2, byte1, byte0
	print byte7, byte6, byte5, byte4


def byte_code_dec_array(name, val):
	'''
	declare an array : SET arr[], 5
	creates array arr, 6 elements in length
	arr[0] to  arr[4] maps to arr[1] to arr[5] in the PRU
	arr[0] contains the len of accessible array, i.e. 5
	'''
	#opcode 19
	OPCODE = 19
	byte2 = 0
	byte1 = pru_arrs[name][0]

	if val.type == 'INT':
		byte2 = 0
		byte0 = val.val

	elif val.any_var:
		byte2 = 1 << 7
		byte0 = get_var(val)
		if byte0 == -1:
			return 0

	else:
		return 0
	
	return pack_byte(OPCODE, byte2, byte1, byte0)

	
def byte_code_single_op(cmd, val):
	'''
	single operand instructions
	e.g. WAIT, GOTO, GET
	'''
	#opcode 20-22
	opcode_dict = {
				'WAIT'	: 20,
				'GOTO'	: 21,
				'GET'	: 22
	}
	OPCODE = opcode_dict[cmd]
	byte2, byte1, byte0 = 0, 0, 0

	#incase wait or goto
	if OPCODE < 22 and not script_mode:
		return 0 #these inst only make sense in a script

	if val.type == 'INT':
	#case1 INT
		byte2 = 0
		if OPCODE == 21: 
			#special case for goto
			val.val = sum(script_inst_size[:val.val])

		else:
			byte0 = val.val & 0xFF #lower 8 bits
			byte1 = (val.val >> 8) & 0xFF #higher 8 bits

		
		
	elif val.any_var:
	#Var or Arr[Var]
		byte2 = 0b01 << 6
		byte0 = get_var(val)
		if byte0 == -1: return 0
		
	else:
		byte2 = 0b10 << 6
		byte0 = pru_vars[val.val[1]]
		byte1 = pru_arrs[val.val[0]][0]
	
	byte3 = OPCODE
	return pack_byte(OPCODE, byte2, byte1, byte0)
	print byte3, byte2, byte1, byte0
	

def byte_code_if(val1, val2, cond, goto):
	'''
	IF (val1 cond val2) GOTO goto
	val1, val2, goto can be any val type
	'''
	#opcode starts with 32 
	
	if not script_mode:
		return 0

	#This inst owns the (0010-XXXX) address space
	#0000 : ==
	#0001 : !=
	#0010 : >=
	#0011 : <=
	#0100 : <
	#0101 : >
	cond_code = {
				'=='	: 0b0000,
				'!='	: 0b0001,
				'>='	: 0b0010,
				'<='	: 0b0011,
				'>'	: 0b0100,
				'<'	: 0b0101
	}
	OPCODE = 0b0010 << 4
	OPCODE |= cond_code[cond]
	
	byte2, byte1, byte0 = 0, 0, 0
	byte7, byte6, byte5, byte4 = 0, 0, 0, 0
	
	#******************val1**********************
	if val1.type == 'INT': 
	#val one is a const
		#print "val1 : INT"
		byte2 |= 0b00 << 6
		byte0 = val1.val
	
	elif val1.any_var:
	#val will be a variable; either V or Arr[C]
		#print "val1 : VAR"
		byte2 |= 0b01 << 6
		byte0 = get_var(val1)
		if byte0 == -1: return 0

		
	else:
	#val is Ar[V]
		#print "val1 : VAR ARR"
		byte2 |= 0b10 <<6
		byte0 = pru_vars[val1.val[1]]
		byte1 = pru_arrs[val1.val[0]][0]
	
	#******************val2**********************
	if val2.type == 'INT': 
	#val one is a const
		#print "val2 : INT"
		byte2 |= 0b00 << 4
		byte4 = val2.val
	
	elif val2.any_var:
	#val will be a variable; either V or Arr[C]
		#print "val2 : VAR"
		byte2 |= 0b01 << 4
		byte4 = get_var(val2)
		if byte4 == -1: return 0

		
	else:
	#val is Ar[V]
		#print "val2 : VAR ARR"
		byte2 |= 0b10  << 4
		byte4 = pru_vars[val2.val[1]]
		byte5 = pru_arrs[val2.val[0]][0]
		
	#******************goto**********************
	if goto.type == 'INT': 
	#val one is a const
		#print "goto : INT"
		byte2 |= 0b00 << 2
		byte6 = goto.val
	
	elif goto.any_var:
	#val will be a variable; either V or Arr[C]
		#print "goto : VAR"
		byte2 |= 0b01 << 2
		byte6 = get_var(goto)
		if byte6 == -1: return 0

		
	else:
	#val is Ar[V]
		#print "goto : VAR ARR"
		byte2 |= 0b10  << 2
		byte6 = pru_vars[goto.val[1]]
		byte7 = pru_arrs[goto.val[0]][0]
	
	
	return pack_byte(OPCODE, byte2, byte1, byte0), pack_byte(byte7, byte6, byte5, byte4)
	
	#just in case - debugging
	byte3 = OPCODE
	print byte3, byte2, byte1, byte0
	print byte7, byte6, byte5, byte4		


def byte_code_arithmetic(cmd, val1, val2):
	'''
	generates code for two operand arithmetic instructions
	AND, SUB, MUL, DIV, MOD
	BSR, BSL, AND, OR, NOT
	'''
	#opcode starts from 48 (11-0000), ends at 79(100-1111)
	
	opcode_dict = {
		#These inst own the (0011-XXXX) address space
					'ADD'	: 48,
					'SUB'	: 50,
					'MUL'	: 52,
					'DIV'	: 54, 
					'MOD'	: 56,
					
		#These inst own the (0100-XXXX) address space 
					'BSL'	: 64,
					'BSR'	: 66, 
					'AND'	: 68,
					'OR'	: 70,
					'NOT'	: 72
	}
	
	OPCODE = opcode_dict[cmd]
	#ADD x, y
	#x, y : INT, VAR, ARR[Var]
	
	byte3, byte2, byte1, byte0 = 0,0,0,0
	byte7, byte6, byte5, byte4 = 0,0,0,0
	
	if val1.arr_var or val2.arr_var:
		#64 bit inst
		OPCODE +=1
		
		if val1.arr_var:
		#val1 is of type Arr[var]
			print "val1 : ARR_VAR"
			byte2 |= 0b10 << 6
			byte0 = pru_vars[val1.val[1]]
			byte1 = pru_arrs[val1.val[0]][0]
			
			if val2.type == 'INT': 
			#val2 is a const
				print "val2 : INT"
				byte2 |= 0b00 << 4
				byte0 = val2.val
	
			elif val2.any_var:
			#val will be a variable; either V or Arr[C]
				print "val2 : VAR"
				byte2 |= 0b01 << 4
				byte4 = get_var(val2)
				if byte4 == -1: return 0

				
			else :
			#val2 Arr[var]
				print "val2 : ARR_VAR"
				byte2 |= 0b10 << 4
				byte4 = pru_vars[val2.val[1]]
				byte5 = pru_arrs[val2.val[0]][0]
				
		else:
		#val1 is not arr[var]; val2 is.
			if val1.type == 'INT': 
			#val one is a const
				print "val1 : INT"
				byte2 |= 0b00 << 6
				byte0 = val1.val
				
			else:
			#val is a var
				print "val1 : VAR"
				byte2 |= 0b01 <<6
				byte0 = get_var(val1)
				if byte0 == -1: return 0

			#val2 stuff
			byte2 |= 0b10 << 4
			byte4 = pru_vars[val2.val[1]]
			byte5 = pru_arrs[val2.val[0]][0]
	
		#return 64bit encoded inst		
		return pack_byte(OPCODE, byte2, byte1, byte0), pack_byte(byte7, byte6, byte5, byte4)
	
	else:
		#32 bit instr
		#x, y : V/C
		
		#***********val1************
		if val1.type == 'INT': 
		#val one is a const
			#print "val1 : INT"
			byte2 |= 0b00 << 7
			byte1 = val1.val
	
		else:
		#val will be a variable; either V or Arr[C]
			#print "val1 : VAR"
			byte2 |= 0b01 << 7
			byte1 = get_var(val1)
			if byte1 == -1: return 0
			
		#***********val2************
		if val2.type == 'INT': 
		#val one is a const
			#print "val2 : INT"
			byte2 |= 0b00 << 6
			byte0 = val2.val
	
		else:
		#val will be a variable; either V or Arr[C]
			#print "val1 : VAR"
			byte2 |= 0b01 << 6
			byte0 = get_var(val2)
			if byte0 == -1: return 0
			
		#return 32bit encoded instruction
		return pack_byte(OPCODE, byte2, byte1, byte0)
		
	#debugging
	byte3= OPCODE
	print byte3, byte2, byte1, byte0
	print byte7, byte6, byte5, byte4
	
#control variables (like pre processor stuff or rather compiler directives, 
#not to be compiled: (END)SCRIPT, RUN, ABORT, DEBUG, RESET - run by python compiler module

def byte_code_ctrl(val):
	'''
	stuff like SCRIPT, ENDSCRIPT,
	RUN, ABORT, DEBUG, SYSTEM etc.
	'''
	return val

"""
Grammar for the parser :

inst : SET val , val
	| GET val
	| IF  ( val cond val ) GOTO val
	| WAIT val
	| GOTO val 
	| ADD val , val
	| SUB val , val 
	| MUL val , val
	| DIV   val , val
	| MOD val, val
	| BSR  val, val
	| BSL val, val
	| OR   val, val
	| AND val, val 
	| NOT val, val
	| LBL VAR
	| SCRIPT
	| ENDSCRIPT
	| RUN
	| HALT 
	| DEBUG
 
val : INT
	| VAR
	| arr
	
arr : VAR [ INT ]
	| VAR [ VAR ]

cond : GTE
	| LTE	
	| GT	
	| LT	
	| EQ	
	| NEQ
"""

def p_inst_SET(p):
	'''inst : SET val ',' val
		| SET VAR '[' ']' ',' val'''

	global pru_var_count	
	if len(p) == 7:
	#array declaration case
		print "Array declaration"
		arr = p[2]
		size = p[6].val
		if arr in pru_arrs:
			print "Arr already defined"
		if (pru_var_count + size + 1) > PRU_VAR_AREA:
			print "No space for Arr"
		else :
		#everything is okay
			pru_arrs[arr] = (pru_var_count, size+1)
			pru_var_count += size + 1

		print "pru_arrs : ", pru_arrs
		
		p[0] = byte_code_dec_array(arr, p[6])

	else:
		#print "SET command -", " val1 : ", p[2], " val2 : " , p[4]
		if p[2].flag: 
			#it is of type SET DIO[x] , y
			p[0] = byte_code_set_r( p[2], p[4])
		else :
			p[0] = byte_code_set(p[2], p[4])

def p_inst_LBL(p):
	'''inst : LBL VAR'''
	global pru_var_count
	byte2 = pru_vars.get(p[2], None)
	if byte2 == None:
		pru_vars[p[2]] = pru_var_count
		byte2 = pru_var_count
		pru_var_count += 1
	p[0] = pack_byte(0x10, byte2, 0, 0)

def add_label(byte_code, index):
	return (byte_code & 0xFFFF0000) | (index & 0xFFFF)

def p_inst_WAIT(p):
	'''inst : WAIT val'''
	#print "WAIT command - val :", p[2]
	p[0] = byte_code_single_op(p[1], p[2])

def p_inst_GOTO(p):
	'''inst : GOTO val'''
	#print "GOTO command - val :", p[2]
	p[0] = byte_code_single_op(p[1], p[2])
	
def p_inst_GET(p):
	'''inst : GET val'''
	#print "GET command - val :", p[2]
	p[0] = byte_code_single_op(p[1], p[2])

def p_inst_IF(p):
	'''inst : IF '(' val cond val ')' GOTO val'''
	#print "IF command - ", "val1 :", p[3], "val2 :", p[5], "cond :", p[4], "GOTO :", p[8]
	p[0] = byte_code_if(p[3], p[5], p[4], p[8])
	
def p_inst_ADD(p):
	'''inst : ADD val ',' val'''
	#print "ADD command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])
	
def p_inst_SUB(p):
	'''inst : SUB val ',' val'''
	#print "SUB command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])

def p_inst_MUL(p):
	'''inst : MUL val ',' val'''
	#print "MUL command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])

def p_inst_DIV(p):
	'''inst : DIV val ',' val'''
	#print "DIV command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])

def p_inst_MOD(p):
	'''inst : MOD val ',' val'''
	#print "MOD command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])

def p_inst_AND(p):
	'''inst : AND val ',' val'''
	#print "AND command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])

def p_inst_OR(p):
	'''inst : OR val ',' val'''
	#print "OR command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])
	
def p_inst_BSR(p):
	'''inst : BSR val ',' val'''
	#print "SUB command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])
	
def p_inst_BSL(p):
	'''inst : BSL val ',' val'''
	#print "SUB command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])

def p_inst_NOT(p):
	'''inst : NOT val ',' val'''
	#print "NOT command -", " val1 : ", p[2], " val2 : " , p[4]
	p[0] = byte_code_arithmetic(p[1], p[2], p[4])

def p_inst_HALT(p):
	'''inst : HALT'''
	p[0] = 0x7F000000

def p_val_INT(p):
	'''val : INT'''
	#print p[1]
	p[0] = Value('INT', p[1])

def p_val_VAR(p):
	'''val : VAR'''
	p[0] = Value('VAR', p[1])
	
def p_val_arr(p):
	'''val : arr'''
	p[0] = p[1]
	
def p_arr_VAR1(p):
	"""arr : VAR '[' INT ']' """
	flag = p[1] in R_VAR
	#should I convert it into a var and send it from here itself? No - how  to handle DIO, PWM etc?
	p[0] = Value("ARR", (p[1], p[3]), flag)
	
	
def p_arr_VAR2(p):
	"""arr : VAR '[' VAR ']' """
	flag = p[1] in R_VAR
	p[0] = Value("ARR", (p[1], p[3]), flag)
	
def p_cond_ops(p):
	'''cond : GTE
		| LTE
		| GT
		| LT
		| EQ
		| NEQ'''
	p[0] = p[1]


#control instructions
def p_inst_SCRIPT(p):
	'''inst : SCRIPT'''
	p[0] = byte_code_ctrl(p[1])

def p_inst_ENDSCRIPT(p):
	'''inst : ENDSCRIPT'''
	p[0] = byte_code_ctrl(p[1])
	
def p_inst_RUN(p):
	'''inst : RUN'''
	p[0] = byte_code_ctrl(p[1])
	
def p_inst_DEBUG(p):
	'''inst : DEBUG'''
	p[0] = byte_code_ctrl(p[1])
	
def p_inst_ABORT(p):
	'''inst : ABORT'''
	p[0] = byte_code_ctrl(p[1])

def p_inst_SYSTEM(p):
	'''inst : SYSTEM'''
	p[0] = byte_code_ctrl(p[1])
	
# Error rule for syntax errors
def p_error(p):
	if p != None:
		print p
		print "Syntax error in input!"


# Build the parser
parser = yacc.yacc()

if __name__ == '__main__':
	s = [ 
	'SET DIO[var2], 1', 
	'IF (arr1[var2] > var3) GOTO arr2[3]' , 
	'ADD var1, 5', 
	'SUB arr1[4], arr2[var3]',
	'GOTO 4',
	'IF ( arr1[var1] < arr1[var2] ) GOTO arr2[var3]',
	'SCRIPT',
	'ENDSCRIPT',
	'GOTO arr1[4]'
]
	try :
		print parser.parse("SET var1, 4")
		#print pru_var_count
		print parser.parse("GET DIO[0]")
                #print pru_var_count
		print parser.parse("SET var1, DIO[0]")
	except Exception as e:
		print "some error"
		print e.args
'''
	for inst in s :
		print inst
		res = parser.parse(inst)
		print res
		print ''
'''
