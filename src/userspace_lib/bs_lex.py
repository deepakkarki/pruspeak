mport ply.lex as lex

#set of reserved variables
R_VAR = set(['DIO', 'PWM', 'TONE', 'AIO', 'TMR'])

#list of reserved words
reserved = [
	'SET',
	'GET',
	'IF',
	'GOTO',
	'ADD',
	'SUB',
	'MUL',
	'DIV',
	'MOD',
	'BSR',
	'BSL',
	'OR',
	'AND',
	'NOT',
	'SCRIPT',
	'ENDSCRIPT',
	'WAIT',
	'RUN',
	'ABORT',
	'HALT',
	'SYSTEM',
	'DEBUG'
]

tokens = [
	'VAR',
	'INT',
	
	'GTE',	# '>='
	'LTE',	# '<='
	'GT',	# '>'
	'LT',		# '<'
	'EQ',	# '=='
	'NEQ',	# '!='
	
	'COM'	# ','
] + reserved

t_GTE		=  r'>='
t_LTE		=  r'<='
t_GT		=  r'>'
t_LT		=  r'<'
t_EQ		=  r'=='
t_NEQ		=  r'!='
t_COM		=  r','
t_ignore	=  ' \t'


#returned as is
literals = '[]()'

def t_INT(t):
	'\d+'
	t.value = int(t.value)    
	return t

def t_VAR(t):
	r'[a-zA-Z_][a-zA-Z_0-9]*'
	if t.value in reserved:
		#eg t.value == 'SET'
		t.type = t.value 
		#then change the token to 'SET'
	return t

lexer = lex.lex()
data = "SET DIO[2], var1"
lexer.input(data)

if __name__ == '__main__':
	for tok in lexer:
		print tok

