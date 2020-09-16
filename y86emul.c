#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "y86emul.h"

struct flags{
	unsigned int ZF:1;
	unsigned int OF:1;
	unsigned int SF:1;
}; 

int hexToDec(char* num, int index){
	
	switch (num[index]){
		
		case 'a':
		case 'A':{
			return 10;
		}
		
		case 'b':
		case 'B':{
			return 11;
		}
		
		case 'c':
		case 'C':{
			return 12;
		}
		
		case 'd':
		case 'D':{
			return 13;
		}
		
		case 'e':
		case 'E':{
			return 14;
		}
		
		case 'f':
		case 'F':{
			return 15;
		}
		
		default : {
			fprintf(stderr, "ERROR: Invalid hexadecimal address\n");
			printf("Status Code: ADR\n");
			exit(EXIT_FAILURE);
		}
	}
}

int toDec(char* num){
	
	int i;
	int k;
	int dec = 0;
	int base = 16;
	for (i = 0, k = strlen(num) - 1; i < strlen(num); i++, k--){
		if (isalpha(num[i])){
			dec += hexToDec(num,i) * (int)pow(base,k); 
		}
		else if (isdigit(num[i])){
			int c = num[i] - '0';
			dec += c * (int)pow(base,k);
		}
	}
	return dec;
}

/*void setStackLimit(int x){
	
	if(x > maxStack)
		maxStack = x;
}
 */

unsigned int size = 0;
unsigned int maxStack = 0;

int main(int argc, char** argv){
		

	if (argc != 2){
		
		fprintf(stderr, "ERROR: INVALID NUMBER OF ARGUMENTS");
		return 1;
	}
	
	if (strcmp(argv[1], "-h") == 0){
		
		printf("Usage: y86emul <y86 input file>\n");
		return 0;
	}
	//INITILIZATIONS
	FILE* fp;
	char* memory = (char*)malloc(sizeof(char));
	int address;
	int number;
	int pc;
	char currLine[1000];
	int dSize = 0;
	int stkTop;
	
	fp = fopen(argv[1], "r+");
	if (fp == NULL){
		fprintf(stderr, "ERROR: file could not be opened\n");
		return 1;
	}
	
	unsigned int start;
	int dText = 0;
	while (fgets(currLine, 1000, fp)){
		
		char* t1 = strtok(currLine, "\t");
		char* t2 = strtok(NULL, "\t");
		char* t3 = strtok(NULL, "\t");
		
		
		if (t1 != NULL && t1[strlen(t1)-1] == '\n')
			t1[strlen(t1)-1] = '\0';
		if (t2 != NULL && t2[strlen(t2)-1] == '\n')
			t2[strlen(t2)-1] = '\0';
		if (t3 != NULL && t3[strlen(t3)-1] == '\n')
			t3[strlen(t3)-1] = '\0';
		
		if (t2 != NULL && t2[strlen(t2)-1] == '\r')
			t2[strlen(t2)-1] = '\0';
		if (t3 != NULL && t3[strlen(t3)-1] == '\r')
			t3[strlen(t3)-1] = '\0';
		
		if (strcmp(".text", t1) == 0){
			
			dText++;
			if (t2 == NULL || t3 == NULL){
				
				fprintf(stderr, "ERROR: Invalid arguments for the .text directive");
				return 1;
			}
			
			pc = toDec(t2);
			start = pc;
			int x;
			for (x = 0; x != strlen(t3); x++){
				memory[pc+x] = t3[x];
				
			}
		}
		
		if (strcmp(".size", t1) == 0){
			
			if (t2 == NULL){
				
				fprintf(stderr, "ERROR: Invalid arguments for the .size directive");
				return 1;
			}
		
			dSize++;
			size = toDec(t2);
			memory = (char*)malloc(size*sizeof(char));
			memory[size-1] = '\0'; 
			stkTop = size - 1;
		}
		
		if (strcmp(".byte", t1) == 0){
			
			if (t2 == NULL || t3 == NULL){
				
				fprintf(stderr, "ERROR: Invalid arguments for the .byte directive");
				return 1;
			}
			
			address = toDec(t2);
			number = toDec(t3);
			memory[address] = number;
		}
		
		if (strcmp(".long", t1) == 0){
			
			if (t2 == NULL || t3 == NULL){
				
				fprintf(stderr, "ERROR: Invalid arguments for the .long directive");
				return 1;
			}
			address = toDec(t2);
			memory[address] = (unsigned int)(atoi(t3))>>24;
			memory[++address] = (unsigned int)(atoi(t3))>>16 & ((unsigned int)(~0)>>8);
			memory[++address] = (unsigned int)(atoi(t3))>>8 & ((unsigned int)(~0)>>16);
		}
		
		if (strcmp(".string", t1) == 0){
			
			if (t2 == NULL || t3 == NULL || strlen(t3) < 2 || t3[0] != '"' || t3[strlen(t3-1)] != '"'){
				
				fprintf(stderr, "ERROR: Invalid arguments for the .string directive");
				return 1;
			}
			address = toDec(t2);
			int i;
			for (i = 1; i < strlen(t3) - 1; i++){
				
				memory[address + (i-1)] = t3[i];
			}
		}
		
		if (strcmp(".bss", t1) == 0){
			
			address = toDec(t2);
			int i;
			for (i = 0; i < atoi(t3); i++){
				
				memory[address + (i-1)] = 0;
			}
		}
	}
	
	if (dText != 1 || dSize != 1){
		fprintf(stderr, "ERROR: invalid number of .size and .text directives");
		return 1;
	}
	
	int error = 0;
	int registers[8];
	int i;
	for (i = 0; i < 8; i++){
		
		registers[i] = 0;
	}
	
	struct flags f;
		f.ZF = 0;
		f.OF = 0;
		f.SF = 0;
	
	int gogogo = 1;
	while (gogogo){
		
		int displacement = 0;
		
		switch(memory[pc]) {
			
			//nop
			case '0': {
				
				pc++;
				if (memory[pc] == '0')
					pc++;
				else {
					fprintf(stderr, "ERROR: Invalid opcode\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}

				break;
			}
			
			//halt
			case '1': {
				
				pc++;
				if (memory[pc] == '0'){
					pc++;
					gogogo = 0;
					printf("Status code: HLT\n");
					break;
				}
				else {
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
			}
			
			//rrmovl
			case '2': {
				
				pc++;
				if (memory[pc] != '0'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break; 
				}
				
				pc++;	
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int ra = memory[pc]-48;
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;					
					break;
				}
				int rb = memory[pc]-48;
				
				registers[rb] = registers[ra];
				pc++;
				break;
				
			}
			
			//irmovl
			case '3': {
				
				pc++;
				if (memory[pc] != '0'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;
				if (memory[pc] != 'f' && memory[pc] != 'F'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int rb = memory[pc] - 48;
				
				pc++;
				char* correct = (char*)malloc(sizeof(char)*9);
				correct[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
					
					correct[(8-((2*j)+1))-1] = memory[pc];
					pc++;
					correct[(8-(2*j))-1] = memory[pc];
					pc++;
					
				}
				
				registers[rb] = toDec(correct);
				free(correct);
				break;
				
			}
			
			//rmmovl
			case '4': {
				
				pc++;
				if (memory[pc] != '0'){					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int ra = memory[pc]-48;
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int rb = memory[pc]-48;

				pc++;
				char* disp = (char*)malloc(sizeof(char)*9);
				disp[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
					
					disp[(8-((2*j)+1))-1] = memory[pc];
					pc++;
					disp[(8-(2*j))-1] = memory[pc];
					pc++;

				}
				displacement = toDec(disp);
				memory[registers[rb] + displacement + 3] = (unsigned int)registers[ra]>>24;
				memory[registers[rb] + displacement + 2] = ((unsigned int)registers[ra]>>16) & ((unsigned int)(~0)>>24);
				memory[registers[rb] + displacement + 1] = ((unsigned int)registers[ra]>>8) & ((unsigned int)(~0)>>24);
				memory[registers[rb] + displacement + 0] = registers[ra] & ((unsigned int)(~0)>>24);
				free(disp);
				break;
				
			}
			
			//mrmovl
			case '5': {
				
				pc++;
				if (memory[pc] != '0'){					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int ra = memory[pc]-48;
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int rb = memory[pc]-48;
				
				pc++;
				char* disp = (char*)malloc(sizeof(char)*9);
				disp[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
					disp[(8-((2*j)+1))-1] = memory[pc];
					pc++;
					disp[(8-(2*j))-1] = memory[pc];
					pc++;
				}
				displacement = toDec(disp);
				registers[ra] = (unsigned char)memory[registers[rb] + displacement];
				free(disp);
				break;
			}
			
			//op
			case '6': {
				
				pc++;
				switch (memory[pc]) {
					
					//add
					case '0': {
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int ra = memory[pc]-48;
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}			
						int rb = memory[pc]-48;
						
						int comp = registers[rb] + registers[ra];
												
						f.ZF = (comp == 0) ? 1 : 0;
						f.OF = ((registers[rb] < 0 && registers[ra] < 0 && comp > 0) || (registers[rb] > 0 && registers[ra] > 0 && comp < 0)) ? 1 : 0;
						f.SF = (comp < 0) ? 1 : 0;
						
						registers[rb] = comp;
						pc++;
						break;
					}
					
					//sub
					case '1': {
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int ra = memory[pc] - 48;
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int rb = memory[pc] - 48;
						
						int comp = registers[rb] - registers[ra];
						
						f.ZF = (comp == 0) ? 1 : 0;
						f.OF = ((registers[rb] < 0 && registers[ra] > 0 && comp > 0) || (registers[rb] > 0 && registers[ra] < 0 && comp < 0)) ? 1 : 0;
						f.SF = (comp < 0) ? 1 : 0;
						
						registers[rb] = comp;
						pc++;
						break;
					}
					
					//andl
					case '2': {
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int ra = memory[pc]-48;
				
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int rb = memory[pc]-48;
												
						registers[rb] &= registers[ra];
						
						f.OF = 0;
						f.SF = (registers[rb] < 0) ? 1 : 0;
						f.ZF = (registers[rb] == 0) ? 1 : 0;
						pc++;
						break;
						
					}
					
					//xorl
					case '3': {
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int ra = memory[pc]-48;
				
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int rb = memory[pc]-48;
						
						registers[rb] ^= registers[ra];
						
						f.OF = 0;
						f.SF = (registers[rb] < 0) ? 1 : 0;
						f.ZF = (registers[rb] == 0) ? 1 : 0;
						pc++;
						break;
					}
					
					//mull
					case '4': {
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int ra = memory[pc]-48;
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int rb = memory[pc]-48;

						int comp = registers[rb] * registers[ra];
						f.ZF = (comp == 0) ? 1 : 0;
						f.OF = ((registers[rb] < 0 && registers[ra] < 0 && comp < 0) || (registers[rb] > 0 && registers[ra] > 0 && comp < 0)) ? 1 : 0;
						f.SF = (comp < 0) ? 1 : 0;
						
						registers[rb] = comp;
						pc++;
						break;
					}
					
					//cmpl
					case '5': {
						
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int ra = memory[pc]-48;
				
						pc++;
						if (memory[pc] < '0' || memory[pc] > '7'){
							fprintf(stderr, "ERROR: Invalid register\n");
							printf("Status Code: INS\n");
							error = 1;
							break;
						}
						int rb = memory[pc]-48;		
						
						int comp = registers[rb] - registers[ra];												
						f.ZF = (comp == 0) ? 1 : 0;
						f.OF = ((registers[rb] < 0 && registers[ra] > 0 && comp > 0) || (registers[rb] > 0 && registers[ra] < 0 && comp < 0)) ? 1 : 0;
						f.SF = (comp < 0) ? 1 : 0;
						
						pc++;
						break;
					}
					
					default : {
						
						fprintf(stderr, "ERROR: Invalid opcode\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
				}
				break;
			}
			
			//JXX
			case '7': {
				
				pc++;
				switch (memory[pc]){
					
					//jmp
					case '0': {
						
						pc++;
						char* jump = (char*)malloc(sizeof(char)*9);
						jump[8] = '\0';
						int j;
						for (j = 0; j < 4; j++){
							
							jump[(8-((2*j)+1))-1] = memory[pc];
							pc++;
							jump[(8-(2*j))-1] = memory[pc];
							pc++;
						}
						
						pc = ((toDec(jump)-start)*2)+start;						
						free(jump);
						break;
					}
					
					//jle
					case '1': {
						
						pc++;
						if ((f.OF ^ f.SF) | f.ZF){
							
							char* jump = (char*)malloc(sizeof(char)*9);
							jump[8] = '\0';
							int j;
							for (j = 0; j < 4; j++){
							
								jump[(8-((2*j)+1))-1] = memory[pc];
								pc++;
								jump[(8-(2*j))-1] = memory[pc];
								pc++;
							}
							pc = ((toDec(jump)-start)*2)+start;						
							free(jump);
						}
						else
							pc += 8;
						
						break;
					}
					
					//jl
					case '2': {
						
						pc++;
						if (f.SF ^ f.OF){
							
							char* jump = (char*)malloc(sizeof(char)*9);
							jump[8] = '\0';
							int j;
							for (j = 0; j < 4; j++){
							
								jump[(8-((2*j)+1))-1] = memory[pc];
								pc++;
								jump[(8-(2*j))-1] = memory[pc];
								pc++;
							}
							pc = ((toDec(jump)-start)*2)+start;							
							free(jump);
						}
						else
							pc += 8;
						break;
					}
					
					//je
					case '3': {
						
						pc++;
						if (f.ZF){
							
							char* jump = (char*)malloc(sizeof(char)*9);
							jump[8] = '\0';
							int j;
							for (j = 0; j < 4; j++){
							
								jump[(8-((2*j)+1))-1] = memory[pc];
								pc++;
								jump[(8-(2*j))-1] = memory[pc];
								pc++;
							}
							pc = ((toDec(jump)-start)*2)+start;							
							free(jump);
						}
						else
							pc += 8;
						break;
					}
					
					//jne
					case '4': {
						
						pc++;
						if (!f.ZF){
							
							char* jump = (char*)malloc(sizeof(char)*9);
							jump[8] = '\0';
							int j;
							for (j = 0; j < 4; j++){
							
								jump[(8-((2*j)+1))-1] = memory[pc];
								pc++;
								jump[(8-(2*j))-1] = memory[pc];
								pc++;
							}
							pc = ((toDec(jump)-start)*2)+start;							
							free(jump);
						}
						else
							pc += 8;
						break;
					}
					
					//jge
					case '5': {
						
						pc++;
						if (!(f.SF ^ f.OF)){
							
							char* jump = (char*)malloc(sizeof(char)*9);
							jump[8] = '\0';
							int j;
							for (j = 0; j < 4; j++){
							
								jump[(8-((2*j)+1))-1] = memory[pc];
								pc++;
								jump[(8-(2*j))-1] = memory[pc];
								pc++;
							}
							pc = ((toDec(jump)-start)*2)+start;							
							free(jump);
						}
						else
							pc += 8;
						break;
					}
					
					//jg
					case '6': {
						
						pc++;
						if (!(f.SF ^ f.OF) & !f.ZF){
							
							char* jump = (char*)malloc(sizeof(char)*9);
							jump[8] = '\0';
							int j;
							for (j = 0; j < 4; j++){
							
								jump[(8-((2*j)+1))-1] = memory[pc];
								pc++;
								jump[(8-(2*j))-1] = memory[pc];
								pc++;
							}
							pc = ((toDec(jump)-start)*2)+start;							
							free(jump);
						}
						else
							pc += 8;
						break;
					}
					
					default : {
						
						fprintf(stderr, "ERROR: Invalid directive");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
				}
				break;
			}
			
			//call
			case ('8'): {
				
				pc++;
				if (memory[pc] != '0'){
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;	
				char* call = (char*)malloc(sizeof(char)*9);
				call[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
			
					call[(8-((2*j)+1))-1] = memory[pc];
					pc++;
					call[(8-(2*j))-1] = memory[pc];
					pc++;
				}
				
				memory[registers[4]-1] = (unsigned int)pc>>24;
				memory[registers[4]-2] = ((unsigned int)pc>>16) & ((unsigned int)(~0)>>24);
				memory[registers[4]-3] = ((unsigned int)pc>>8) & ((unsigned int)(~0)>>24);
				memory[registers[4]-4] = pc & ((unsigned int)(~0)>>24);
				registers[4] -= 4;
				
				j = toDec(call);
				pc = ((j-start)*2)+start;
				free(call);
				break;
			}
			
			//ret
			case ('9'): {
				
				pc++;
				if (memory[pc] != '0'){
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;
				pc=(~0);
				//pop ret address into pc
				pc&=(((unsigned int)(~0)>>8)|((int)(unsigned char)memory[registers[4]+3])<<24);
				pc&=((((unsigned int)(~0)>>16)|((~0)<<24))|(((int)(unsigned char)memory[registers[4]+2])<<16));
				pc&=((((unsigned int)(~0)>>24)|((~0)<<16))|(((int)(unsigned char)memory[registers[4]+1])<<8));
				pc&=(((unsigned int)(~0)<<8)|((int)(unsigned char)memory[registers[4]]));
				registers[4] += 4;
				break;
			}
			
			//push
			case ('A'): 
			case ('a'): {
				
				pc++;
				if (memory[pc] != '0'){
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int ra = memory[pc]-48;
				
				pc++;
				if (memory[pc] != 'f' && memory[pc] != 'F'){
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
					
				memory[registers[4]-1] = (unsigned int)registers[ra]>>24;
				memory[registers[4]-2] = ((unsigned int)registers[ra]>>16) & ((unsigned int)(~0)>>24);
				memory[registers[4]-3] = ((unsigned int)registers[ra]>>8) & ((unsigned int)(~0)>>24);
				memory[registers[4]-4] = registers[ra] & ((unsigned int)(~0)>>24);
				registers[4] -= 4;
				pc++;
				break;
				
			}
			
			//pop
			case ('B'):
			case ('b'): {
				
				pc++;
				if (memory[pc] != '0'){
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int ra = memory[pc]-48;
				
				pc++;
				if (memory[pc] != 'f' && memory[pc] != 'F'){
					
					fprintf(stderr, "35ERROR: Invalid register\n");
					error = 1;
					break;
				}
				
				registers[ra]=(~0);
				//pop ret address into pc
				registers[ra]&=(((unsigned int)(~0)>>8)|((int)(unsigned char)memory[registers[4]+3])<<24);
				registers[ra]&=((((unsigned int)(~0)>>16)|((~0)<<24))|(((int)(unsigned char)memory[registers[4]+2])<<16));
				registers[ra]&=((((unsigned int)(~0)>>24)|((~0)<<16))|(((int)(unsigned char)memory[registers[4]+1])<<8));
				registers[ra]&=(((unsigned int)(~0)<<8)|((int)(unsigned char)memory[registers[4]]));
				registers[4] += 4;
				pc++;
				break;
			}
			
			//read
			case 'c':
			case 'C': {
				
				pc++;
				//readb
				if (memory[pc] == '0'){
					
					pc++;
					if (memory[pc] < '0' || memory[pc] > '7'){
						
						fprintf(stderr, "ERROR: Invalid register\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
					int ra = memory[pc]-48;
					
					pc++;
					if (memory[pc] != 'f' && memory[pc] != 'F'){
						
						fprintf(stderr, "ERROR: Invalid register\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
					
					pc++;
					char* disp = (char*)malloc(sizeof(char)*9);
					disp[8] = '\0';
					int j;
					for (j = 0; j < 4; j++){
					
						disp[(8-((2*j)+1))-1] = memory[pc];
						pc++;
						disp[(8-(2*j))-1] = memory[pc];
						pc++;
					}
					displacement = toDec(disp);
					f.ZF  = (scanf("%c", &memory[registers[ra] + displacement]) < 0) ? 1 : 0;
					
					memory[registers[ra] + displacement] -= 48;
					free(disp);
					
					break;
					
				}
						
				//readl
				else if (memory[pc] == '1'){
					
					pc++;
					if (memory[pc] < '0' || memory[pc] > '7'){
						
						fprintf(stderr, "ERROR: Invalid register\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
					int ra = memory[pc]-48;
					
					pc++;
					if (memory[pc] != 'f' && memory[pc] != 'F'){
						
						fprintf(stderr, "ERROR: Invalid register\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
					
					pc++;
					char* disp = (char*)malloc(sizeof(char)*9);
					disp[8] = '\0';
					int j;
					for (j = 0; j < 4; j++){
					
						disp[(8-((2*j)+1))-1] = memory[pc];
						pc++;
						disp[(8-(2*j))-1] = memory[pc];
						pc++;
					}

					displacement = toDec(disp);
					
					f.ZF=(scanf("%c%c%c%c",
					&memory[registers[ra]+displacement],
					&memory[registers[ra]+displacement+1],
					&memory[registers[ra]+displacement+2],
					&memory[registers[ra]+displacement+3])<0)?1:0;
					
					memory[registers[ra]+displacement]-=48;
					memory[registers[ra]+displacement+1]-=48;
					memory[registers[ra]+displacement+2]-=48;
					memory[registers[ra]+displacement+3]-=48;
					
					if(memory[registers[ra]+displacement+1]>=0){	
					
						memory[registers[ra]+displacement]*=10;
						memory[registers[ra]+displacement]+=memory[registers[ra]+displacement+1];
					}

					free(disp);
					break;
					
				}
				
				else {
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
			}
			
			//write
			case 'd':
			case 'D': {
				
				pc++;
				//writeb
				if (memory[pc] ==  '0'){
					
					pc++;
					if (memory[pc] < '0' || memory[pc] > '7'){
						
						fprintf(stderr, "ERROR: Invalid register\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
					int ra = memory[pc]-48;
					
					pc++;
					if (memory[pc] != 'f' && memory[pc] != 'F'){
						
						fprintf(stderr, "ERROR: Invalid register\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
					
					pc++;
					char* disp = (char*)malloc(sizeof(char)*9);
					disp[8] = '\0';
					int j;
					for (j = 0; j < 4; j++){
					
						disp[(8-((2*j)+1))-1] = memory[pc];
						pc++;
						disp[(8-(2*j))-1] = memory[pc];
						pc++;
					}
					displacement = toDec(disp);
					free(disp);
					
					printf("%c", memory[registers[ra]+displacement]);
				}
				
				//writel
				else if (memory[pc] == '1'){
					
					pc++;
					if (memory[pc] < '0' || memory[pc] > '7'){
						
						fprintf(stderr, "ERROR: Invalid register\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
					int ra = memory[pc]-48;
					
					pc++;
					if (memory[pc] != 'f' && memory[pc] != 'F'){
						
						fprintf(stderr, "ERROR: Invalid register\n");
						printf("Status Code: INS\n");
						error = 1;
						break;
					}
					
					pc++;
					char* disp = (char*)malloc(sizeof(char)*9);
					disp[8] = '\0';
					int j;
					for (j = 0; j < 4; j++){
					
						disp[(8-((2*j)+1))-1] = memory[pc];
						pc++;
						disp[(8-(2*j))-1] = memory[pc];
						pc++;
					}
					displacement = toDec(disp);
					
					printf("%d",(((unsigned int)(unsigned char)(memory[registers[ra]+displacement]))+
					(((unsigned int)(unsigned char)(memory[registers[ra]+displacement+1])))*16*16+
					(((unsigned int)(unsigned char)(memory[registers[ra]+displacement+2])))*16*16*16*16+
					(((unsigned int)(unsigned char)(memory[registers[ra]+displacement+3])))*16*16*16*16*16*16));
					
					free(disp);
					break;
					
				}
				
				else {
					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				break;
			}
			
			//movsbl
			case 'e':
			case 'E': {
				
				pc++;
				if (memory[pc] != '0'){					
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int ra = memory[pc]-48;
				
				pc++;
				if (memory[pc] < '0' || memory[pc] > '7'){
					fprintf(stderr, "ERROR: Invalid register\n");
					printf("Status Code: INS\n");
					error = 1;
					break;
				}
				int rb = memory[pc]-48;
				
				pc++;
				char* disp = (char*)malloc(sizeof(char)*9);
				disp[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
					disp[(8-((2*j)+1))-1] = memory[pc];
					pc++;
					disp[(8-(2*j))-1] = memory[pc];
					pc++;
				}
				displacement = toDec(disp);
				
				registers[ra] = memory[registers[rb] + displacement];
				
				free(disp);
				break;
			}
			
			default: {
				
				fprintf(stderr, "ERROR: Invalid case\n");
				printf("Status Code: ADR\n");
				error = 1;
				break;
			}
		}	
		if (error == 1)
			break;
	}
	free(memory);
	fclose(fp);
	return 0;
}