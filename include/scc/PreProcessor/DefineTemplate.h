
// keep the definition of a template
// #define FOO(x, ...) printf(x, __VA_ARGS__)
//
// NOTE : you need to expend the entire chain of define at once to avoid recursion in define and also to work with the bait of recurrsion
//
//	ex : this don't work
//	#define FOO(x) BAR(X)
//	#define BAR(x) FOO(X)
//
//	but this work
//	#define FOO BAR
//	#define FOO_PARENTESIS(x) (x)
//	#define BAR(x) FOO FOO_PARENTESIS(x)
//

// #define FOO BAR
// #define FOO_PARENTESIS(x) (x)
// #define BAR(x) FOO FOO_PARENTESIS(x)

#define FOO(x) BAR(X)
#define BAR(x) FOO(X)
