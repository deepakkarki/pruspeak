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
		
	arr_var = property(is_arr_var, None)
