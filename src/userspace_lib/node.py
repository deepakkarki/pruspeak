class Value(object):
	def __init__(self, t, v, f=False):
		'''
		t = type (VARIABLE/CONSTANT/ ARRAY)
		v = value (string      / integer	  / tuple 	variable respectively)
		f = flag, True if array is DIO, PWM, TMR, etc.
		'''
		self.type = t
		self.val = v
		self.flag	= f
	
	def is_arr_var(self):
		'''
		checks if the value is a variable indexed array
		'''
		return (self.type == 'ARR') and (type(self.val[1]) == str)
		
	def is_arr_const(self):
		'''
		checks if the value is a array indexed by a const
		'''
		return (self.type == 'ARR') and (type(self.val[1]) == int)
		
	def is_any_var(self):
		'''
		checks if the value is either ARR[const] or VAR
		'''
		return self.type == 'VAR' or self.is_arr_const()
		
	arr_var = property(is_arr_var, None)
	arr_const =  property(is_arr_const, None)
	any_var = property(is_any_var, None)
	
