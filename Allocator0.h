const size_t KB=1024;
const size_t MB=KB*KB;
const size_t GB=KB*MB;
const size_t memToAllocate=GB/8*1;

const agusz_t meanObjSize=0x40; // the mean size of objects in agu is supposed 0x40

const size_t vvElements0=memToAllocate/meanObjSize;
const size_t vvElements=MIN(vvElements0,uWord(TokenMax)-uWord(TokenMin)+1); 

const agusz_t vvSize=sizeof(vect)*vvElements;
//const addrT vvSize=sizeof(vect[vvElements]);

const agusz_t mmSize=sizeof(agu)*vvElements*meanObjSize; // mmSize shouldn't overflow


// addrT is unsigned. The starting address is calculated from M.vv-(maxAddrT+1)
// when the freePos arrives to overflow to 0, then there is no more free space and due to arithemtical reasons
// the last usefull agu (address granularity unit) shouldn't be used as when the freePos is 0 then adding 0 to 
// M.vv-(maxAddrT+1) would not give M.vv as requested
// Attention should be given to that during check for available space.
// The heap and vectors are alocated repectively on the left and on the right of the address of reference.

__attribute__((aligned(PAGE_SIZE)))struct heap{
	agu mmStart[mmSize];
	const ptr mm(){ return (ptr)&mmStart[mmSize]-std::numeric_limits<addrT>::max()-1;};
	vect vv[vvElements];
	void* vptr(addrT q){return (ptr)mm()+q;}
	
} M,M1;

// Two heaps (M,M1) are created in order to allow for concurrent garbage collection

