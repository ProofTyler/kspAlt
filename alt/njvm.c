#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <bigint.h>
#include "instruction.h"


StackSlot stack[10000];
ObjRef *sda;
int stackPointer = 0;
int framepointer = 0;
ObjRef rg;
bool debug = FALSE;


ObjRef legeObjektInHeap(int value){
	ObjRef objRef = malloc(sizeof(unsigned int) + sizeof(int));
	objRef->size = sizeof(int);
	*(int *)objRef->data = value;
	return objRef;
}

/*
ObjRef newPrimitiveObject(int numBytes);
ObjRef newCompoundObject(int numObjRefs); 
*/

void pushObjRef(ObjRef objRef){
	stack[stackPointer].isObjRef = TRUE;
	stack[stackPointer].u.objRef = objRef;
	stackPointer++;	
}

/*int getObjValue(ObjRef objRef){
	return *(int*)objRef->data;
}*/

ObjRef popObjRef(void){
	stackPointer = stackPointer - 1;
	return stack[stackPointer].u.objRef;
}



/*Kein Objekt*/
void push(int immediateValue){
	stack[stackPointer].isObjRef = FALSE;
	stack[stackPointer].u.number = immediateValue;
	stackPointer++;
}
int pop(void){
	stackPointer = stackPointer - 1;
	return stack[stackPointer].u.number;
}
/*Kein Objekt*/

void pushOnSDA(int adresse){
	sda[adresse] = popObjRef();
}

void runCode(unsigned int *code, int adresse){
	int pc = 0;
	/*int n1 = 0;
	int n2 = 0;
	int readInt;*/
	char readChar;
	bool halt = FALSE;
	ObjRef ref;
	while(halt == FALSE){		
		unsigned int opCode = code[pc] >> 24;
		int immediateValue = SIGN_EXTEND(IMMEDIATE(code[pc]));
		if(pc >= 1000){
			printf("Stackoverflow! Program stopped\n");
			halt = TRUE;
		}
		pc++;		
		switch(opCode){
			case HALT: halt = TRUE;
				break;
			case PUSHC: bigFromInt(immediateValue); pushObjRef(bip.res);
				break;
			case PUSHG: pushObjRef(sda[immediateValue]);
				break;
			case RDCHR: scanf("%c", &readChar);bigFromInt((unsigned char)readChar); 
					    pushObjRef(bip.res);
				break;
			case RDINT: bigRead(stdin);
						pushObjRef(bip.res);
				break;
			case ASF: push(framepointer);
					  framepointer = stackPointer;
    				  stackPointer = stackPointer + immediateValue;
				break;
			case RSF: stackPointer = framepointer; framepointer = pop();
				break;
			case jmp: pc = immediateValue;
				break;
			case CALL: 	
				push(pc);
				pc = immediateValue;
				break;
			case RET:	pc = stack[stackPointer-1].u.number;
				break;
			case DROP:
				while(immediateValue != 0){
					pop();
					immediateValue--;
				}
				break;
			case POPR: rg = popObjRef(); 
				break;
			case PUSHR: pushObjRef(rg);
				break;
			case DUP: 
				ref = popObjRef();
				pushObjRef(ref);
				pushObjRef(ref);
				break;
			case NEW :
				n1 =  
				/*
				-n1 = Anzahl Refs von immediateWert
				-neues Objekt mit malloc und size =
				*/

				break;
			case GETF :
				break;
			case PUTF :
				break;
			case NEWA :
				break;
			case GETFA :
				break;
			case PUTFA :
				break;
			case GETSZ :
				break;
			case PUSHN :
				break;
			case REFEQ :
				break;
			case REFNE :
				break;
		}

		/*Wenn der Stack nicht leer ist, wird der letzte Wert geschrieben
		oder auf dem sta angelegt
		-> stackPointer > 0 */
		if(stackPointer > 0){
			switch(opCode){
				case POPL: stack[framepointer + immediateValue].u.number = pop();
					break;
				case PUSHL: push(stack[framepointer + immediateValue].u.number);
					break;
				case POPG: pushOnSDA(immediateValue);
					break;
				case WRINT: bip.op1 = popObjRef(); bigPrint(stdout);       
					break;
				case WRCHR: bip.op1 = popObjRef(); 
							printf("%c", bigToInt());
					break;
				case brf: bip.op1 = popObjRef(); 
						  if(bigToInt() == TRUE){ 
						  	break;
						  } else{ 
						  	pc = immediateValue;
						  } 
					break;
				case brt: bip.op1 = popObjRef(); 
						  if(bigToInt() == FALSE){ 
						  	break;
						  } else{ 
						  	pc = immediateValue;
						  } 
					break;
			}
		}		
		/*Wenn mind. 2 Werte im Stack liegen, 
		kann eine Operation durchgeführt werden oder 
		eine Bedingung verglichen werden
		*/
		if(stackPointer >= 2){
			if( (opCode >= 2 && opCode <= 6) || 
				(opCode >= 17 && opCode <= 22) ){
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				switch(opCode){
					case ADD: bigAdd();pushObjRef(bip.res);break;
					case SUB: bigSub();pushObjRef(bip.res);break;
					case MUL: bigMul();pushObjRef(bip.res);break;
					case DIV: bigDiv();pushObjRef(bip.res); break;
					case MOD: bigDiv();pushObjRef(bip.rem);break;
					case eq: bigCmp() == 0 ? bigFromInt(1) : bigFromInt(0);
							 pushObjRef(bip.res); break;
					case ne: bigCmp() != 0 ? bigFromInt(1) : bigFromInt(0);
							 pushObjRef(bip.res); break;
					case lt: bigCmp() < 0 ? bigFromInt(1) : bigFromInt(0);
							 pushObjRef(bip.res); break;
					case le: bigCmp() <= 0 ? bigFromInt(1) : bigFromInt(0);
							 pushObjRef(bip.res); break;
					case gt: bigCmp() > 0 ? bigFromInt(1) : bigFromInt(0);
							 pushObjRef(bip.res); break;
					case ge: bigCmp() >= 0 ? bigFromInt(1) : bigFromInt(0);
							 pushObjRef(bip.res); break;
				}	
			}
			continue;
		}			
	}
}

void printCode(unsigned int *code, int adresse){
	bool halt = FALSE;
	int pc = 0;
	pc = adresse;

	while(halt == FALSE){
		unsigned int opCode = code[pc] >> 24;
		int immediateValue = SIGN_EXTEND(IMMEDIATE(code[pc]));
		printf("%d:	", pc);
		pc++;		
		switch(opCode){
			case HALT: 
				printf("HALT\n");
				halt = TRUE;
				break;
			case CALL:
				printf("CALL 	%d\n", immediateValue);	
			case RET:
				printf("RET\n");
			case DROP:
				printf("DROP 	%d\n", immediateValue);
			case PUSHR:
				printf("PUSHR\n");
			case POPR:
				printf("POPR\n");
			case DUP:
				printf("DUP\n");
			case PUSHC:
				printf("PUSHC:	%d\n", immediateValue);
				break;
			case PUSHG:
				printf("PUSHG:	%d\n", immediateValue);
				break;
			case POPG:
				printf("POPG:	%d\n", immediateValue);
				break;
			case ASF:
				printf("ASF:	%d\n", immediateValue);
				break;
			case RSF:
				printf("RSF\n");
				break;
			case PUSHL:
				printf("PUSHL:	%d\n", immediateValue);
				break;
			case POPL:
				printf("POPL:	%d\n", immediateValue);
				break;
			case ADD:
				printf("ADD\n");
				break;
			case SUB:
				printf("SUB\n");
				break;
			case MUL:
				printf("MUL\n");
				break;
			case DIV:
				printf("DIV\n");
				break;
			case MOD:
				printf("MOD\n");
				break;
			case RDINT:
				printf("RDINT\n");
				break;
			case WRINT:
				printf("WRINT\n");
				break;
			case RDCHR:
				printf("RDCHR\n");
				break;
			case WRCHR:
				printf("WRCHR\n");
				break;

			case eq:
				printf("eq\n");
				break;
			case ne:
				printf("ne\n");
				break;
			case lt:
				printf("lt\n");
				break;
			case le:
				printf("le\n");
				break;
			case gt:
				printf("gt\n");
				break;
			case jmp:
				printf("jmp:	%d\n", immediateValue);
				break;
			case brf:
				printf("brf:	%d\n", immediateValue);
				break;
			case brt:
				printf("brt:	%d\n", immediateValue);
				break;
		}
		if(debug == TRUE){
			break;			
		}

	}	
}

void startDebugger(unsigned int *code){
	char debugInstruction[2];
	int adresse = 0;
	bool isBreakpoint = FALSE;
	int breakpoint;
	printCode(code,adresse);
	while(TRUE){		
		printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
		scanf("%99s", &debugInstruction[0]);
		if(strcmp( debugInstruction, "step") == 0){
			adresse++;
			printCode(code, adresse);			
		} else if(strcmp( debugInstruction, "run") == 0){
			runCode(code, 0);
			break;
		} else if(strcmp( debugInstruction, "list") == 0){
			debug = FALSE;
			printCode(code,0);
			printf("	--- end of code ---\n");
			debug = TRUE;
			printCode(code, adresse);
		} else if(strcmp( debugInstruction, "breakpoint") == 0){
			if(isBreakpoint == TRUE){
				printf("DEBUG [breakpoint]: set at %d\n", breakpoint);
			}else {
				printf("DEBUG [breakpoint]: cleared\n");	
			}			
			printf("DEBUG [breakpoint]: address to set, -1 to clear, <ret> for no change?\n");
			scanf("%d",&breakpoint);
			if(breakpoint != -1){				
				printf("DEBUG [breakpoint]: now set at %d\n", breakpoint);
				isBreakpoint = TRUE;
			} else {
				printf("DEBUG [breakpoint]: now cleared\n");
				isBreakpoint = FALSE;
			}			
			printCode(code, adresse);
		} else if(strcmp( debugInstruction, "quit") == 0){
			break;
		} else{
			continue;
		}
	}
}

void startNjvm(int size, FILE *datei){	
    unsigned int *instructions;
    int adresse = 0;
    printf("Ninja Virtual Machine started\n");
    instructions = malloc(size*sizeof(int));
    sda = malloc(instructions[3]*sizeof(int));
    fread(instructions, sizeof(int), size, datei);
    if(debug == TRUE){
    	startDebugger(instructions);	
    }else{
    	runCode(instructions, adresse);	
    }	
	free(sda);
	free(instructions);
	printf("Ninja Virtual Machine stopped\n");
}

int readHeader(FILE *datei, char *filename){
   	int puffer[4];
   	int size;
   	if( fread(puffer, sizeof(int), 4, datei) == 4){
   		if(! (((puffer[0] >> 0)&0xFF) == 'N' && ((puffer[0] >> 8)&0xFF) == 'J' && 
			((puffer[0] >> 16)&0xFF) == 'B' &&  ((puffer[0] >> 24)&0xFF) == 'F') ){
			printf("Error: Fehler beim fread\n");
			printf("Ninja Virtual Machine stopped\n");
			exit(99);	
		}

	    if(debug == TRUE){
		    printf("DEBUG: file '%s' loaded (code size = %d, data size = %d)\n", 
		    	filename, puffer[2], puffer[3]);
		}
	    size = puffer[2];	
   	}else{
   		printf("Error: Fehler beim fread\n");
   		exit(99);
   	}    
    return size;
}

void openFile(char *filename){
	FILE *datei = fopen(filename, "r");
   	if(NULL != datei) {
   		int size = readHeader(datei, filename);
   		startNjvm(size, datei);
   		fclose(datei);	
   	} else {
   		printf("Error: cannont open code file '%s'\n", filename);
   		exit(99);
   	}
}

int main(int argc, char *argv[]) {
	if(argc == 1){
		printf("Error: no code file specified\n");
	} else if(argc > 3) {
		printf("Error: more than one code file specified\n");
	} else {
		int i;
		for(i = argc-1; i > 0; i--){
			if(strcmp( argv[i], "--help") == 0){

				printf("Usage: ./njvm [options] <code file>\n"); 
				printf("Options:\n");
				printf("	--debug          start virtual machine in debug mode\n"); 
  				printf("	--version        show version and exit\n");
  				printf("	--help           show this help and exit\n");
				return 0;
			}
			if(strcmp( argv[i], "--version") == 0){
				printf("Ninja Virtual Machine version 4\n");
				return 0;
			}
		}
		if(argc == 3){
			if(strcmp( argv[1], "--debug") == 0){
				debug = TRUE;
				openFile(argv[2]);		
			}else{
				printf("Error: more than one code file specified\n");
			}			
		}
		if(argc == 2){
			openFile(argv[1]);
		}		
	}	
	return 0;
}

/*
1. Wie groß muss sda und stack sein?
2. Was darf alle global stehen?
3. Was soll als Validierung in der Console ausgegeben werden? bsp: durch null teilen?
4. Version ?!
*/
/*
    printf("-------------\n");
    printf("Header File:\n");
    printf("-------------\n");
    printf("Format Identiefies: %c, %c, %c, %c \n", 
    puffer[0] >> 0, puffer[0] >> 8, puffer[0] >> 16, puffer[0] >> 24 );
    printf("Version: %d\n", puffer[1]);
    printf("Number of Instruction: %d\n", puffer[2]);
    printf("Number of variables in sda: %d\n", puffer[3]);
    */