enum UnionIntersection:Word{Union,Intersection};
enum patternKind:Word{patInferred,patInt,patFloat,patExe,patSeq,patAny,patNone};
const Int patFail=IntMax,noMarked=IntMax;
typedef enum:int {matchNegated=1,matchMarked=2,matchKeepFailures=4} matchFlag;
