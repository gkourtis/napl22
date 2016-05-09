	
	va_start(args,extended); 
		for(uWord i=0;i<n;i++){ 
			tAggregate a((obj)va_arg(args,Word));Word i1,i2,n;if(extended){i1=va_arg(args,Word);i2=va_arg(args,Word);}
			Obj c=a.Class();
			if(classGreaterEqual(classAggregate,c)){
#ifdef individuateClass
				if(Class.w==0 || c>Class) 
					Class=c;
#endif
				hAggregate& h=*a.hP();
				if(!extended){
					i1=0;i2=h.n;
				}else{
					ADJUSTOFFSET2(h.n,i1,i2);//if(i1<0) i1=0;if(i1>i2)i1=i2;
				}	
				n=abs(i2-i1);
				Instructions;
			}
		}
	va_end(args);
	
