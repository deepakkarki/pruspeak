import ply.yacc as yacc
from bs_lex import *
from node import *

#keeps track of next available memory offset to place a variable
pru_var_count = 0

#does the variable to memory mapping work
pru_vars = {
		#e.g. "var1" : 0x02,
		"var1" : 0x02,
		"var2" : 0x03
}

#does the array to (address, size) mapping.
pru_arrs = {#add DIO, PWM etc. here
		#e.g. "arr1" : (0x10, 5) 
					#start, size
		"arr1" : (0x10, 5) 
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

def byte_code_set_r(val1, val2):
	'''
	encodes instructions of the form SET DIO[a], arr[b]; 
	where DIO can replaced by PWM, TMR, AI etc
	'''
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
		
		if val2.type == 'ARR' :
			#if y is an array; should add error checking here - to check overflow
			byte0 = pru_arrs[val2.val[0]][0] + val2.val[1]#arr address + const
		
		else:
			byte0 = pru_vars.get(val2.val, val2.val)
			#if y is a const, it is returned; else value of variable in pru_vars is returned
			
	#pack all the bytes
	print OPCODE, byte2, byte1, byte0  #packed_bytes		
				
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
	'''inst : SET val ',' val'''
	print "SET command -", " val1 : ", p[2], " val2 : " , p[4]
	if p[2].flag: 
		#it is of type SET DIO[x] , y
		return byte_code_set_r( p[2], p[4])
	else :
		return byte_code_set(p[2], p[4])

def p_inst_WAIT(p):
	'''inst : WAIT val'''
	print "WAIT command - val :", p[2]
	
def p_inst_GOTO(p):
	'''inst : GOTO val'''
	print "GOTO command - val :", p[2]

def p_inst_IF(p):
	'''inst : IF '(' val cond val ')' GOTO val'''
	print "IF command - ", "val1 :", p[3], "val2 :", p[5], "cond :", p[4], "GOTO :", p[8]
	
def p_inst_ADD(p):
	'''inst : ADD val ',' val'''
	print "ADD command -", " val1 : ", p[2], " val2 : " , p[4]
	
def p_inst_SUB(p):
	'''inst : SUB val ',' val'''
	print "SUB command -", " val1 : ", p[2], " val2 : " , p[4]

def p_inst_SCRIPT(p):
	'''inst : SCRIPT'''
	print 'SCRIPT'

def p_inst_ENDSCRIPT(p):
	'''inst : ENDSCRIPT'''
	print 'ENDSCRIPT'

def p_inst_HALT(p):
	'''inst : HALT'''
	print "HALT"

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

# Error rule for syntax errors
def p_error(p):
	print p
	print "Syntax error in input!"

# Build the parser
parser = yacc.yacc()
s = [ 
	'SET DIO[myvar], 1', 
	'IF (a > b) GOTO c' , 
	'ADD myvar, 5', 
	'SUB arr[4], another_var',
	'GOTO 4',
	'IF ( arr1[a] < arr2[b] ) GOTO arr3[c]',
	'SCRIPT',
	'ENDSCRIPT',
	'GOTO arr3[4]'
]
print parser.parse("SET DIO[var2], arr1[var1]")
#for inst in s :
#	parser.parse(inst)
#print result

