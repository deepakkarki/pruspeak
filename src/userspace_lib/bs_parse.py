import ply.yacc as yacc
from bs_lex import *

"""
Grammar for the parser :

inst : SET val , val
	| GET val
	| IF  ( val COND val ) GOTO val
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
	
"""

def p_inst_SET(p):
	'''inst : SET val ',' val'''
	print "SET command -", " val1 : " + str(p[2]), " val2 : " + str(p[4])
	
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

# Error rule for syntax errors
def p_error(p):
	print p
	print "Syntax error in input!"


# Build the parser
parser = yacc.yacc()
s = '''SET DIO[myvar], 1'''
result = parser.parse(s)
#print result

