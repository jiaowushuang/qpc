


static inline long fls(unsigned long x) {
  return x ? sizeof(x) * 8 - __builtin_clzl(x) : 0;
}

#define BIT(x) (1ULL << (x))
#define MASK(x) (BIT(x) - 1)
/* bit manipulation */
#define BM(base, count, val) (((val) & (((1ULL) << (count)) - 1)) << (base))
#define AM(base, count) ((((1ULL) << (count)) - 1) << (base))

#define SU(me, val) ((me) |= (val))
#define CU(me, val) ((me) &= (~(val)))
#define GU(me, mask) ((me) & (mask))
#define MU(me, mask, val) ((me) = ((me) & (~GU(me, mask))) | (val))

#define GUB(me, mask, base) (((me) & (mask)) >> (base))
#define GUI(me, mask, base, val) (GUB(me, mask, base) + (val))
#define GUD(me, mask, base, val) (GUB(me, mask, base) - (val))

#define SR(me, base, count, val) (SU(me, BM(base, count, val)))
#define GR(me, base, count) (GU(me, AM(base, count)) >> (base))
#define CR(me, base, count, val) (CU(me, BM(base, count, val)))
#define ZR(me, base, count) (CU(me, AM(base, count)))
#define MR(me, base, count, val) (MU(me, AM(base, count), BM(base, count, val)))

#define IR(me, base, count, val)                                               \
	(MU(me, AM(base, count),                                               \
	    BM(base, count, GUI(me, AM(base, count), base, val))))
#define DR(me, base, count, val)                                               \
	(MU(me, AM(base, count),                                               \
	    BM(base, count, GUD(me, AM(base, count), base, val))))

#define SB(me, base) ((me) |= BIT(base))
#define GB(me, base) (((me)&BIT(base)) >> (base))
#define CB(me, base) ((me) &= ~BIT(base))