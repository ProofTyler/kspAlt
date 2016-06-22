#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "procedures.h"

#include <time.h>


StackSlot stack[10000];
ObjRef *sda;
ObjRef rg;

int stackPointer = 0;
int framepointer = 0;
int version = 4;
int debugPc = 0;
bool isDebug = false;

ObjRef legeObjektInHeap(int value){
	ObjRef objRef = malloc(sizeof(unsigned int) + sizeof(int));
	objRef->size = sizeof(int);
	*(int *)objRef->data = value;
	return objRef;
}

void pushObjRef(ObjRef objRef){
	stack[stackPointer].isObjRef = TRUE;
	stack[stackPointer].u.objRef = objRef;
	stackPointer++;	
}


ObjRef popObjRef(void){
	stackPointer = stackPointer - 1;
	return stack[stackPointer].u.objRef;
}

/*
	@param val: push this value on 'top-of-Stack'
*/
void push(int immediateValue){
	stack[stackPointer].isObjRef = FALSE;
	stack[stackPointer].u.number = immediateValue;
	stackPointer++;
}

/*
	@return: Return int value from 'top-of-stack'
*/
int pop(void){
	stackPointer = stackPointer - 1;
	return stack[stackPointer].u.number;
}

/*
 * @return: Takes the value from the sda on position n
 *
*/
void pushSDA(int adresse){
	sda[adresse] = popObjRef();
}

/*
 * @return: Set a value in sda on position n,
 * the value is from the top of the stack
 *
*/
 /*
void setGlobalVal(int n){
	sda[n] = pop();
}
*/
int getPc(){
	return debugPc;
}

void setPc(int value){
	debugPc = value;
}

void runCode(unsigned int instuctions[]){	
	int ir;
	int immediateVal;
	int pc = 0;
	bool stop = false;
	/*
	int v1,v2;
	int readInt;
	*/
	char readChar;
	if(isDebug)	pc = getPc();
	while(!stop){		
		ir = OP_CODE(instuctions[pc]);
		immediateVal = SIGN_EXTEND(IMMEDIATE(instuctions[pc]));
		pc = pc + 1;			
		switch(ir){			
			case HALT:				
				stop = true;
				break;
			case PUSHC:
				bigFromInt(immediateVal); pushObjRef(bip.res);
				break;
			case ADD:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigAdd();pushObjRef(bip.res);				
				break;
			case SUB:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigSub();pushObjRef(bip.res);
				break;
			case MUL:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigMul();pushObjRef(bip.res);
				break;
			case DIV:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigDiv();pushObjRef(bip.res);			
				break;
			case MOD:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				/*bigMod();pushObjRef(bip.res);*/
				break;
			case RDINT:
				bigRead(stdin);
				pushObjRef(bip.res);
				break;
			case WRINT:
				/*printf("%d", popObjRef());*/
				bip.op1 = popObjRef(); bigPrint(stdout);
				break;
			case RDCHR:
				scanf("%c", &readChar);bigFromInt((unsigned char)readChar); 
					    pushObjRef(bip.res);
				break;
			case WRCHR:
				/*printf("%c", popObjRef());*/
				bip.op1 = popObjRef(); 
				printf("%c", bigToInt());
				break;
			case PUSHG:
				pushObjRef(sda[immediateVal]);
				/*pushObjRef(getGlobalVal(immediateVal));*/
				break;
			case POPG:
				pushSDA(immediateVal);
				/*setGlobalVal(immediateVal);*/
				break;
			case ASF:
				push(framepointer);
				framepointer = stackPointer;
				stackPointer = stackPointer + immediateVal;
				break;
			case RSF:
				stackPointer = framepointer;
				framepointer = pop();
				break;
			case POPL:
				/*stackMachine[immediateVal] = pop();*/
				stack[framepointer + immediateVal].u.number = pop();
				break;
			case PUSHL:
				/*push(stackMachine[fp + immediateVal]);*/
				push(stack[framepointer + immediateVal].u.number);
				break;
			case EQ:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigCmp() == 0 ? bigFromInt(1) : bigFromInt(0);
				pushObjRef(bip.res);
				break;
			case NE:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigCmp() != 0 ? bigFromInt(1) : bigFromInt(0);
				pushObjRef(bip.res);
				break;
			case LT:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigCmp() < 0 ? bigFromInt(1) : bigFromInt(0);
				pushObjRef(bip.res);
				break;	
			case LE:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigCmp() <= 0 ? bigFromInt(1) : bigFromInt(0);
				pushObjRef(bip.res);
				break;
			case GT:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigCmp() > 0 ? bigFromInt(1) : bigFromInt(0);
				pushObjRef(bip.res);
				break;
			case GE:
				bip.op2 = popObjRef();
				bip.op1 = popObjRef();
				bigCmp() >= 0 ? bigFromInt(1) : bigFromInt(0);
				pushObjRef(bip.res);
				break;
			case JMP:
				pc = immediateVal;
				break;
			case BRF:
				bip.op1 = popObjRef(); 
				if(bigToInt() == TRUE) break;
				else pc = immediateVal;
				break;
			case BRT:
				bip.op1 = popObjRef(); 
				if(bigToInt() == FALSE) break;
				else pc = immediateVal; 
				break;
			case CALL:
				push(pc);
				pc = immediateVal;
				break;
			case RET:
				pc = stack[stackPointer-1].u.number;
				/*pc = pop();*/
				break;
			case DROP:
				while(immediateVal != 0){
					pop();
					immediateVal--;
				}
				break;

			case PUSHR:
				pushObjRef(rg);
				break;
			case POPR:
				rg = popObjRef();
				break;
			case DUP: 
				/*
				ref = popObjRef();
				pushObjRef(ref);
				pushObjRef(ref);
				*/
				break;
		}
		if(isDebug){
			setPc(pc);
			break;	
		}
	}
}

void printCode(unsigned int instuctions[]){	
	int ir;
	int immediateVal;
	int pc = 0;
	int ic = 0;
	bool stop = false;
	if(isDebug){
		ic = getPc();
		pc = getPc();
	}
	while(!stop){		
		ir = OP_CODE(instuctions[pc]);
		immediateVal = SIGN_EXTEND(IMMEDIATE(instuctions[pc]));
		pc = pc + 1;	
		switch(ir){			
			case HALT:				
				printf("%03d:    HALT\n", ic++);
				stop = true;
				break;
			case PUSHC:
				printf("%03d:    PUSHC\t%d\n", ic++, immediateVal);
				break;
			case ADD:
				printf("%03d:    ADD\n", ic++);
				break;
			case SUB:
				printf("%03d:    SUB\n", ic++);
				break;
			case MUL:
				printf("%03d:    MUL\n", ic++);
				break;
			case DIV:
				printf("%03d:    DIV\n", ic++);
				break;
			case MOD:
				printf("%03d:    MOD\n", ic++);
				break;
			case RDINT:
				printf("%03d:    RDINT\n", ic++);
				break;
			case WRINT:
				printf("%03d:    WRINT\n", ic++);
				break;
			case RDCHR:
				printf("%03d:    RDCHR\n", ic++);
				break;
			case WRCHR:
				printf("%03d:    WRCHR\n", ic++);
				break;
			case PUSHG:
				printf("%03d:    PUSHG\t%d\n", ic++, immediateVal);
				break;
			case POPG:
				printf("%03d:    POPG\t%d\n", ic++, immediateVal);
				break;
			case ASF:
				printf("%03d:    ASF\t%d\n", ic++, immediateVal);
				break;
			case RSF:
				printf("%03d:    RSF\n", ic++);
				break;
			case PUSHL:
				printf("%03d:    PUSHL\t%d\n", ic++, immediateVal);
				break;
			case POPL:
				printf("%03d:    POPL\t%d\n", ic++,immediateVal);
				break;
			case EQ:
				printf("%03d:    EQ\n", ic++);
				break;
			case NE:
				printf("%03d:    NE\n", ic++);
				break;
			case LT:
				printf("%03d:    LT\n", ic++);
				break;	
			case LE:
				printf("%03d:    LE\n", ic++);
				break;
			case GT:
				printf("%03d:    GT\n", ic++);
				break;
			case GE:
				printf("%03d:    GE\n", ic++);
				break;
			case JMP:
				printf("%03d:    JMP\t%d\n", ic++,immediateVal);
				break;
			case BRF:
				printf("%03d:    BRF\t%d\n", ic++,immediateVal);
				break;
			case BRT:
				printf("%03d:    BRT\t%d\n", ic++,immediateVal);
				break;
			case CALL:
				printf("%03d:    CALL\t%d\n", ic++,immediateVal);
				break;
			case RET:
				printf("%03d:    RET\n", ic++);
				break;
			case DROP:
				printf("%03d:    DROP\t%d\n", ic++,immediateVal);
				break;
			case PUSHR:
				printf("%03d:    PUSHR\n", ic++);
				break;
			case POPR:
				printf("%03d:    POPR\n", ic++);
				break;
			case DUP:
				printf("%03d:    DUP\n", ic++);
				break;
		}
		if(isDebug)break; 
	}	
	
}


void showInConsole(){
	/*
	int spCounter;
	int sdaCounter = 15;
	spCounter = sp;
	printf("\n");
	while(spCounter != -1){
		printf("stack[%d]  =    ", spCounter);
		if(spCounter == sp && spCounter == fp) printf("sp, fp |\n");
		else if(spCounter == sp) printf("sp\n");
		else {
			printf("%d",stackMachine[spCounter]);
			if(spCounter == fp){
				printf(", fp");	
			}
			printf("	 |\n");	
		}				
		spCounter--;
	}
	printf("\n");
	while( sdaCounter != -1){
		if(sda[sdaCounter] != 0 && sda[sdaCounter] < 50){
			printf("  sda[%d]  =    %d\n",sdaCounter, sda[sdaCounter]);	
		}
		sdaCounter--;
	}

	printf("----------------------------------\n");
	*/
}

void debug(unsigned int *instructions, int irSize){
	char eingabe[100];
	printf("\n");
	printf("Debug Modus (type 'help' for more information):\n");
	printf("\n");		
	while(isDebug){
		if( strcmp(eingabe,"print\n") == 0 ){
			isDebug = false;
			printCode(instructions);
			isDebug = true;
		}					
		fgets(eingabe, 100, stdin);
		if( strcmp(eingabe,"next\n") == 0 ){
			printf("\n");
			printCode(instructions);
			runCode(instructions);
			showInConsole();
		}
		if( strcmp(eingabe,"goto\n") == 0 ){
			int i;
			printf("Type position u want go to:\n");
			scanf("%d",&i);
			
			while(i != getPc()){
				printCode(instructions);
				runCode(instructions);	
			}						
			showInConsole();
		}
		if( strcmp(eingabe,"run\n") == 0 ){						
			isDebug = false;
			runCode(instructions);			
			isDebug = true;
		}
		if( strcmp(eingabe,"quit\n") == 0 ){
			isDebug = false;
		}
		if( strcmp(eingabe,"help\n") == 0 ){
			printf("'print'      -> for only print the file\n");
			printf("'next'       -> run the next step in the file-instructions\n");
			printf("'goto'       -> run to a position\n");
			printf("'run'        -> run only file-instructions\n");
			printf("'quit'	     -> quit debugging and stop VM\n");
			printf("\n");
		}	
	}
}

void readFile(char *fileName){
	FILE *fp = fopen(fileName, "r");
	if (fp != NULL) {
		int feld[4];
		if(fread(feld, sizeof(int), 4, fp) != 4){
			printf("Error: Header not readable\n");
			exit(99);	
		} else {
			int fileVersion;
			int irSize;
			int sdaSize;			
			unsigned int *instructions;
			if(!((char)(feld[0] >> 0) == 'N' && (char)(feld[0] >> 8) == 'J' && 
				(char)(feld[0] >> 16) == 'B' && (char)(feld[0] >> 24) == 'F')) {
				printf("Error: Format Identifier not correct\n");
				printf("Instead of 'N,J,B,F' it is '%c,%c,%c,%c' \n",
				(feld[0] >> 0),(feld[0] >> 8),(feld[0] >> 16),(feld[0] >> 24));
				exit(99);
			}
			fileVersion = feld[1];
			if(!(fileVersion == version)){
				printf("Error: file '%s' has wrong version number\n",fileName);
				exit(99);	
			}
			irSize = feld[2];
			if(irSize == 0){
				printf("Error: No instructions available\n");
				exit(99);	
			}
			instructions = malloc(sizeof(int)*irSize);
			if(fread(instructions, sizeof(int), irSize,fp) != irSize){
				printf("Error: Instructions not readable\n");
				exit(99);
			}
			sdaSize = feld[3];
			sda = malloc(sizeof(int)*sdaSize);

			printf("%s", STARTED_NJVM);
			if(isDebug){
				debug(instructions, irSize);				
			}else{
				runCode(instructions);
			}			
			free(sda);
			free(instructions);
			fclose(fp);
			printf("%s", STOPPED_NJVM);			
		}
	}else {
		printf("Error: cannot open code file '%s'\n",fileName);
		exit(99);
	}
}


int main(int argc, char *argv[]) {
	int arg = argc - 1;
	int isOption = false;
	if(argc < 2){
		printf("Error: no code file specified\n");
		exit(99);
	}
	if(argc > 3){
		printf("Error: more than one code file specified\n");	
		exit(99);
	}	
	while(!isOption && arg != 0){
		if( strcmp(argv[arg],"--help") == 0){
			printf("%s", STARTED_NJVM);
			printf("Usage: ./njvm [option] <code file>\n");
			printf("Options:\n");
			printf("  --debug        start virtual machine in debug mode\n");
			printf("  --version        show version and exit\n");
		  	printf("  --help           show this help and exit\n");
			printf("%s", STOPPED_NJVM);
			exit(99);
		}
		if( strcmp(argv[arg],"--version") == 0){
			printf("%s", STARTED_NJVM);
			printf("Ninja Virtual Machine version %d\n",version);
			printf("%s", STOPPED_NJVM);
			exit(99);
		}
		if( strcmp(argv[arg],"--debug") == 0){
			isDebug = true;
			(arg == 2) ? readFile(argv[1]) : readFile(argv[2]);
			exit(99);
		}
		arg--;
	}
	if(argc > 2){
		printf("Error: more than one code file specified\n");
	}else{		
		readFile(argv[1]);		
	}
    return 0;
}