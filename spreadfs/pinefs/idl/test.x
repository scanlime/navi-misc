enum an_enum {
  a = 3,
  b = 5,
  from = 4
};

typedef struct {
  int a;
  double b;
} foo;


struct bar {
  int a;
  double b;
};

typedef enum {
  n = 5,
  o = 6,
  p = 7} another_enum;

struct bar2 {
  int a;
  double b;
  enum {
    q = 5,
    r = 6,
    s = 7} another_enum;
};



typedef union switch (int b) {
 case 0:
   int foo;
 case 1:
   int bar;
 case 2:
   enum {
     t = 5,
     u = 6,
     v = 7} another_enum;
 default:
   void;
} baz;

union baz_2 switch (enum {aa = 1, ab = 2} b) {
 case 0:
   int foo;
 case 1:
   int bar;
 default:
   void;
};

/* tests escaping python reserved words */
struct reserved_test {
  int from;
  int def;
  int def_;
  struct {
    int def__;
    int in___;
  } import;
};

const in = 4;

typedef foo * maybe_foo;

typedef void; 
/* This is correct according to the grammar, but semantically meaningless. */

typedef foo foo_fixed_arr [40];

typedef foo foo_var_arr_1 <40>;
typedef foo foo_var_arr_2 <>;

typedef int int_fixed_arr [40];

typedef double double_var_arr <400>;

struct ll_test1 {
  unsigned int a;
  unsigned int b;
  ll_test1 * silly_link_ptr;
};

struct ll_test2 {
  unsigned int a;
  ll_test2 *foo_link;
  unsigned int b;
};

struct ll_test3 {
  ll_test3 *foo_link;
  unsigned int a;
};

struct fooble {
  opaque res [0];
};

program goofy {
                   version goof_vers {

		     struct {int c; int d;}
		     goof_NULL(struct {int a; int b;}, 
			       enum {goof1 =1, goof2 = 2}) = 0;

                   } = 1;
           } = 0;

/*typedef unsigned double bogus;*/ /* produces error */



