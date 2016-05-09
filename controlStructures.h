void foreach(Aggregate a,const obj action,bool inverseDirection=false);
//void foreach2(Aggregate a,Token action,bool inverseDirection=false);

// it takes a flag b and a number or aggragate Token.
// If it is a number then exits that level of secondaries.If it is a Token it exits up to that secondary.
void exitSec(bool b,Obj n_t);

PRIMITIVE get_i(),set_i(),loop_(),times_();
Word for_(Word i1,Word i2,Word step,Word e);
void loop_(Obj e);