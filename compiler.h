#if defined(__clang__)
	#define compiler "Clang"
#elif defined(__ICC) || defined(__INTEL_COMPILER)
	#define compiler "Intel ICC/ICPC"
#elif defined(__GNUC__) || defined(__GNUG__)
	#define compiler "GNU GCC/G++"
#elif defined(__HP_cc) || defined(__HP_aCC) 
	#define compiler "Hewlett-Packard C/aC++."
#elif defined(__IBMC__) || defined(__IBMCPP__)
	#define "IBM XL C/C++"
#elif defined(_MSC_VER)
	#define compiler "Microsoft Visual Studio"
#elif defined(__PGI) 
	#define compiler "Portland Group PGCC/PGCPP"
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
	#define compiler "Oracle Solaris Studio"
#else
	#define compiler "unknown compiler" 
#endif

