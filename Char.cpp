Char ucase(Char c){
	return std::toupper(c,loc0);
}

Char lcase(Char c){
	return std::tolower(c,loc0);
}

bool isctrl(Char c){
	return std::iscntrl(c,loc0);
}

bool upperCaseComparison(Char pat,Char c){
	return ucase(pat)==ucase(c);
}

bool equalComparison(Char pat,Char c){
	return pat==c;
}

bool unequalComparison(Char pat,Char c){
	return pat!=c;
}

