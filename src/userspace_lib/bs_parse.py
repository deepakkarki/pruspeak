import ply.yacc as yacc
from bs_lex import *

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
	p[0] = p[1]

def p_val_VAR(p):
	'''val : VAR'''
	#print p[1]
	p[0] = p[1]
	
def p_val_arr(p):
	'''val : arr'''
	#print p[1]
	p[0] = p[1]
	
def p_arr_VAR1(p):
	"""arr : VAR '[' INT ']' """
	#print "p_arr_VAR1 : p = ", p
	#print "p_arr_VAR1" , p[1], p[3]
	p[0] = (p[1], p[3])
	
def p_arr_VAR2(p):
	"""arr : VAR '[' VAR ']' """
	#print "p_arr_VAR2 : p[1], [2]" ,  p[1], p[3]
	p[0] = ( p[1], p[3])
	
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
for inst in s :
	parser.parse(inst)
#print result

