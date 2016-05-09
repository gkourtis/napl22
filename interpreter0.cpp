// here the implementation should be done providing a stack of "virtual" dictionaries
// One of them for integer interpretation one for Strings one for Floats and generally 
// for other litteral values
// Lastly the "normal" dictionary used for dictionary entries


obj toObj(String s){
	Word r;
	if(String2Int(s,r,10)) 
		return newInt(r);
	double f;
	if(String2Double(s,f))
		return newDouble(f);
	//stopETs.lock();
		obj o{dict.valueOf(s)};
	//stopETs.unlock();
	return o;
}
