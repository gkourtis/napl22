String oout(obj o);

bool noPrimitiveClass(obj Class);
void dictShow(bool showAll=false);

std::string indexHtml(String command=0,String output=0,String view=0),dictHtml(bool showAll);
void showObjss(std::stringstream& ss,Obj o);
std::string debugHtml(Int n=notFound);

enum alignT{
	alignDisable=-1,alignLeft=0,alignCenter=1,alignRight=2
};

const char* al[]={ "left","center","right" };

std::string 
encap(std::string tag,Word w,alignT alignement=alignDisable,Word d=0,bool Hex=false),encode(String s),encode(const char* s),encode(std::string s);

template<typename T>
std::string encap(const char* tag,T s,alignT alignement=alignDisable);

std::string encapEncode(const char* tag,String s,alignT alignement=alignDisable);

std::string showObjssHtml(Obj o);

void showStack(std::stringstream& ss);

template<typename T>
void mencap(std::stringstream& ss,const char* tag,T t);

template<typename T,typename ...TT>
void mencap(std::stringstream& ss,const char* tag,T t,TT ...tt);


template<typename ...TT>
std::string Mencap(const char* outerTag,const char* innerTag,TT ...tt);

std::string htmlStatus(),htmlStack();

void showStack();
int print(Obj o);
void compilerConf();
