### Auto-generated at Wed, 04 Jun 2003 16:15:47 +0000 from idl/test.x
import rpchelp
a = 3
b = 5
from_ = 4
n = 5
o = 6
p = 7
q = 5
r = 6
s = 7
t = 5
u = 6
v = 7
aa = 1
ab = 2
goof1 = 1
goof2 = 2
an_enum = rpchelp.r_int
foo = rpchelp.struct ('foo', [('a',rpchelp.r_int),('b',rpchelp.r_double)])
bar = rpchelp.struct ('bar', [('a',rpchelp.r_int),('b',rpchelp.r_double)])
another_enum = rpchelp.r_int
bar2 = rpchelp.struct ('bar2', [('a',rpchelp.r_int),('b',rpchelp.r_double),('another_enum',rpchelp.r_int)])
baz = rpchelp.union ('baz', rpchelp.r_int, 'b', {0 : rpchelp.r_int, 1 : rpchelp.r_int, 2 : rpchelp.r_int, None : rpchelp.r_void})
baz_2 = rpchelp.union ('baz_2', rpchelp.r_int, 'b', {0 : rpchelp.r_int, 1 : rpchelp.r_int, None : rpchelp.r_void})
reserved_test = rpchelp.struct ('reserved_test', [('from_',rpchelp.r_int),('def_',rpchelp.r_int),('def__',rpchelp.r_int),('import_',rpchelp.struct ('import_', [('def___',rpchelp.r_int),('in____',rpchelp.r_int)]))])
in_ = 4
maybe_foo = rpchelp.opt_data (lambda : foo)
# "typedef void;" encountered
foo_fixed_arr = rpchelp.arr (foo, rpchelp.fixed, 40)
foo_var_arr_1 = rpchelp.arr (foo, rpchelp.var, 40)
foo_var_arr_2 = rpchelp.arr (foo, rpchelp.var, None)
int_fixed_arr = rpchelp.arr (rpchelp.r_int, rpchelp.fixed, 40)
double_var_arr = rpchelp.arr (rpchelp.r_double, rpchelp.var, 400)
ll_test1 = rpchelp.linked_list ('ll_test1', 2, [('a',rpchelp.r_uint),('b',rpchelp.r_uint),('silly_link_ptr',rpchelp.opt_data (lambda : ll_test1))])
ll_test2 = rpchelp.linked_list ('ll_test2', 2, [('a',rpchelp.r_uint),('foo_link',rpchelp.opt_data (lambda : ll_test2)),('b',rpchelp.r_uint)])
ll_test3 = rpchelp.linked_list ('ll_test3', 1, [('foo_link',rpchelp.opt_data (lambda : ll_test3)),('a',rpchelp.r_uint)])
fooble = rpchelp.struct ('fooble', [('res',rpchelp.opaque (rpchelp.fixed, 0))])
class goofy_1(rpchelp.Server):
	prog = 0
	vers = 1
	procs = {0 : rpchelp.Proc ('goof_NULL', rpchelp.struct ('_unnamed', [('c',rpchelp.r_int),('d',rpchelp.r_int)]), [rpchelp.struct ('_unnamed', [('a',rpchelp.r_int),('b',rpchelp.r_int)]),rpchelp.r_int])}

