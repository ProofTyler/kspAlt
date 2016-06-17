#define IMMEDIATE(x) ((x) & 0x00FFFFFF) 
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
#define HALT 0
#define PUSHC 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10

#define PUSHG 11
#define POPG 12
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16

#define eq 17
#define ne 18
#define lt 19
#define le 20
#define gt 21
#define ge 22

#define jmp 23
#define brf 24
#define brt 25

#define CALL 26
#define RET 27
#define DROP 28
#define PUSHR 29
#define POPR 30
#define DUP 31

#define NEW 32
#define GETF 33
#define PUTF 34

#define NEWA 35
#define GETFA 36
#define PUTFA 37

#define GETSZ 38

#define PUSHN 39
#define REFEQ 40
#define REFNE 41


#define FALSE 0
#define TRUE 1

#define MSB (1 << (8 * sizeof(unsigned int) - 1))
#define IS_PRIM(objRef) (((objRef)->size & MSB) == 0)
#define GET_SIZE(objRef) ((objRef)->size & ~MSB)
#define GET_REFS(objRef) ((ObjRef *)(objRef)->data)

typedef int bool;
/*
typedef struct {
	unsigned int size;
	unsigned char data[1];
} *ObjRef;
*/

typedef struct {
	bool isObjRef;
	union {
		ObjRef objRef;
		int number;
	} u;
} StackSlot;
