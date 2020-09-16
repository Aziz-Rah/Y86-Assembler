#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "y86dis.h"

char* convertToHex(int num){
    char* output = (char*)malloc(sizeof(char));
    int i = 0;
    while(1){
		
        if(strlen(output)<(i+1))
            output = (char*)realloc(output, sizeof(output)+sizeof(char));
        if((num%16)<10)
            output[i]=(char)((num%16)+48);
        else{
            switch(num%16){
                case 10:{
                    output[i]='A';
                    break;
                }
                case 11:{
                    output[i]='B';
                    break;
                }
                case 12:{
                    output[i]='C';
                    break;
                }
                case 13:{
                    output[i]='D';
                    break;
                }
                case 14:{
                    output[i]='E';
                    break;
                }
                case 15:{
                    output[i]='F';
                    break;
                }
            }
        }
        num/=16;
        if(num==0)
            break;
        i++;
    }
    output = (char*)realloc(output, sizeof(output)+sizeof(char));
    output[i+1] = '\0';
    int j = strlen(output)-1;
    for(i = 0; i < j; i++, j--){
        char temp = output[i];
        output[i]=output[j];
        output[j]=temp;
    }
    return output;
}



char* registers(int n){
	
	switch(n){
		
		case 0: {
			
			return "%eax";
		}
		case 1: {
			
			return "%ecx";
		}
		case 2: {
			
			return "%edx";
		}
		case 3: {
			
			return "%ebx";
		}
		case 4: {
			
			return "%esp";
		}
		case 5: {
			
			return "%ebp";
		}
		case 6: {
			
			return "%esi";
		}
		case 7: {
			
			return "%edi";
		}
		default : {
			
			fprintf(stderr, "invalid register\n");
			exit(EXIT_FAILURE);
		}
	}
	
}

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
			fprintf(stderr, "ERROR: Problem in Hex Method");
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

int main(int argc, char** argv){
	
	if (argc != 2){
		
		fprintf(stderr, "ERROR: Invalid number of arguments");
		return 1;
	}
	
	if (strcmp(argv[1], "-h") == 0){
		
		printf("Usage: y86dis <y86 input file>\n");
		return 0;
	}
	
	//INITILIZATIONS
	FILE* fp;
	char* text = (char*)malloc(sizeof(char));
	int pc;
	char currLine[1000];
	int dText = 0;
	
	fp = fopen(argv[1], "r+");
	if (fp == NULL){
		fprintf(stderr, "ERROR: file could not be opened");
		return 1;
	}
	
	unsigned int start = 0;
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
		if (t3 != NULL && t3[strlen(t3)-1] == '\r')
			t3[strlen(t3)-1] = '\0';
		
		if (strcmp(".text", t1) == 0){
			
			dText++;
			if (t2 == NULL || t3 == NULL){
				
				fprintf(stderr, "ERROR: Invalid arguments for the .text directive");
				return 1;
			}
			
			pc = 0;
			start = toDec(t2);
			text = (char*)malloc(sizeof(char)*(strlen(t3)+1));
			int x;
			for (x = 0; x != strlen(t3); x++){
				text[pc+x] = t3[x];	
			}
			break;
		}
	}
	
	if (dText != 1){
		fprintf(stderr, "ERROR: Invalid number of .text directives");
		return 1;
	}
	
	int error = 0;
	while (pc < strlen(text)){
		
		int displacement = 0;
		
		switch(text[pc]) {
			
			//nop
			case '0': {
				
				char* hexadr = convertToHex(start+pc);

				pc++;
				if (text[pc] == '0'){
					pc++;
					printf("0x%s\tNOP\t\t\t00\n", hexadr);
				}
				else {
					fprintf(stderr, "ERROR: 1Invalid register\n");
					error = 1;
					break;
				}
				break;
			}
			
			//halt
			case '1': {
				
				char* hexadr = convertToHex(start+pc);
				pc++;
				if (text[pc] == '0'){
					pc++;
					printf("0x%s\tHLT\t\t\t10\n", hexadr);
					break;
				}
				else {
					fprintf(stderr, "ERROR: 2Invalid register\n");
					error = 1;
					break;
				}
			}
			
			//rrmovl
			case '2': {
				
				char* hexadr = convertToHex(start+pc);
				char next[5];
				next[4] = '\0';
				int i;
				for (i = 0; i < 4; i++){
					
					next[i] = text[pc+i];
				}

				pc++;
				if (text[pc] != '0'){
					fprintf(stderr, "ERROR: 3Invalid register\n");
					error = 1;
					break; 
				}
				
				pc++;	
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "ERROR: 4Invalid register\n");
					error = 1;
					break;
				}
				int ra = text[pc]-48;
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "ERROR: 5Invalid register\n");
					error = 1;					
					break;
				}
				int rb = text[pc]-48;
				
				printf("0x%s\tMOVL\t%s, %s\t%s\n", hexadr, registers(ra), registers(rb), next);
				pc++;
				break;
				
			}
			
			//irmovl
			case '3': {
				
				char* hexadr = convertToHex(start+pc);
				char next[13];
				next[12] = '\0';
				int i;
				for (i = 0; i < 12; i++){
					
					next[i] = text[pc+i];
				}
				
				pc++;
				if (text[pc] != '0'){
					fprintf(stderr, "ERROR: 6Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;
				if (text[pc] != 'f' && text[pc] != 'F'){
					fprintf(stderr, "ERROR: 7Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "ERROR: 8Invalid register\n");
					error = 1;
					break;
				}
				int rb = text[pc] - 48;
				
				pc++;
				char* correct = (char*)malloc(sizeof(char)*9);
				correct[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
					
					correct[(8-((2*j)+1))-1] = text[pc];
					pc++;
					correct[(8-(2*j))-1] = text[pc];
					pc++;
					
				}
				
				j = toDec(correct);
				printf("0x%s\tMOVL\t$%d, %s\t%s\n", hexadr, j, registers(rb), next);
				free(correct);
				break;
				
			}
			
			//rmmovl
			case '4': {
				
				char* hexadr = convertToHex(start+pc);
				char next[13];
				next[12] = '\0';
				int i;
				for (i = 0; i < 12; i++){
					
					next[i] = text[pc+i];
				}
				pc++;
				if (text[pc] != '0'){					
					fprintf(stderr, "ERROR: 9Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "ERROR: 10Invalid register\n");
					error = 1;
					break;
				}
				int ra = text[pc]-48;
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "ERROR: 11Invalid register\n");
					error = 1;
					break;
				}
				int rb = text[pc]-48;
				
				pc++;
				char* disp = (char*)malloc(sizeof(char)*9);
				disp[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
					
					disp[(8-((2*j)+1))-1] = text[pc];
					pc++;
					disp[(8-(2*j))-1] = text[pc];
					pc++;

				}
				displacement = toDec(disp);
				printf("0x%s\tMOVL\t%s, %d(%s)\t%s\n", hexadr,registers(ra), displacement, registers(rb), next);
				free(disp);
				break;
				
			}
			
			//mrmovl
			case '5': {
				
				char* hexadr = convertToHex(start+pc);
				char next[13];
				next[12] = '\0';
				int i;
				for (i = 0; i < 12; i++){
					
					next[i] = text[pc+i];
				}
				pc++;
				if (text[pc] != '0'){					
					fprintf(stderr, "ERROR: 12Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "ERROR: 13Invalid register\n");
					error = 1;
					break;
				}
				int ra = text[pc]-48;
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "ERROR: 14Invalid register\n");
					error = 1;
					break;
				}
				int rb = text[pc]-48;
				
				pc++;
				char* disp = (char*)malloc(sizeof(char)*9);
				disp[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
					disp[(8-((2*j)+1))-1] = text[pc];
					pc++;
					disp[(8-(2*j))-1] = text[pc];
					pc++;
				}
				displacement = toDec(disp);

				printf("0x%s\tMOVL\t%d(%s), %s\t%s\n", hexadr, displacement, registers(rb), registers(ra), next);
				free(disp);
				break;
			}
			
			//op
			case '6': {
				
				char* hexadr = convertToHex(start+pc);
				char next[5];
				next[4] = '\0';
				int i;
				for (i = 0; i < 4; i++){
					
					next[i] = text[pc+i];
				}
				
				pc++;
				switch (text[pc]) {
					
					//add
					case '0': {
						
						
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 15Invalid register\n");
							error = 1;
							break;
						}
						int ra = text[pc]-48;
						
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 16Invalid register\n");
							error = 1;
							break;
						}			
						int rb = text[pc]-48;
												
						printf("0x%s\tADD\t%s, %s\t%s\n", hexadr, registers(ra), registers(rb), next);
						
						pc++;
						break;
					}
					
					//sub
					case '1': {
						
						char* hexadr = convertToHex(start+pc);
						char next[5];
						next[4] = '\0';
						int i;
						for (i = 0; i < 4; i++){
					
							next[i] = text[pc+i];
						}
						
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 17Invalid register\n");
							error = 1;
							break;
						}
						int ra = text[pc] - 48;
						
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 18Invalid register\n");
							error = 1;
							break;
						}
						int rb = text[pc] - 48;
						
						printf("0x%s\tSUB\t%s, %s\t%s\n", hexadr, registers(ra), registers(rb),next);
						pc++;
						break;
					}
					
					//andl
					case '2': {
						
						char* hexadr = convertToHex(start+pc);
						char next[5];
						next[4] = '\0';
						int i;
						for (i = 0; i < 4; i++){
					
							next[i] = text[pc+i];
						}
						
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 19Invalid register\n");
							error = 1;
							break;
						}
						int ra = text[pc]-48;
				
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 20Invalid register\n");
							error = 1;
							break;
						}
						int rb = text[pc]-48;
												
						printf("0x%s\tANDL\t%s, %s\t%s\n", hexadr, registers(ra), registers(rb),next);
						pc++;
						break;
						
					}
					
					//xorl
					case '3': {
						
						char* hexadr = convertToHex(start+pc);
						char next[5];
						next[4] = '\0';
						int i;
						for (i = 0; i < 4; i++){
					
							next[i] = text[pc+i];
						}
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 21Invalid register\n");
							error = 1;
							break;
						}
						int ra = text[pc]-48;
				
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 22Invalid register\n");
							error = 1;
							break;
						}
						int rb = text[pc]-48;
						
						printf("0x%s\tXORL\t%s, %s\t%s\n", hexadr, registers(ra), registers(rb),next);
						pc++;
						break;
					}
					
					//mull
					case '4': {
						
						char* hexadr = convertToHex(start+pc);
						char next[5];
						next[4] = '\0';
						int i;
						for (i = 0; i < 4; i++){
					
							next[i] = text[pc+i];
						}
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 23Invalid register\n");
							error = 1;
							break;
						}
						int ra = text[pc]-48;
						
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 24Invalid register\n");
							error = 1;
							break;
						}
						int rb = text[pc]-48;

						printf("0x%s\tIMULL\t%s, %s\t%s\n", hexadr, registers(ra), registers(rb), next);
						pc++;
						break;
					}
					
					//cmpl
					case '5': {
						
						char* hexadr = convertToHex(start+pc);
						char next[5];
						next[4] = '\0';
						int i;
						for (i = 0; i < 4; i++){
					
							next[i] = text[pc+i];
						}
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 25Invalid register\n");
							error = 1;
							break;
						}
						int ra = text[pc]-48;
				
						pc++;
						if (text[pc] < '0' || text[pc] > '7'){
							fprintf(stderr, "ERROR: 26Invalid register\n");
							error = 1;
							break;
						}
						int rb = text[pc]-48;		
						
						printf("0x%s\tCMPL\t%s, %s\t%s\n", hexadr, registers(ra), registers(rb), next);
						
						pc++;
						break;
					}
					
					default : {
						
						fprintf(stderr, "ERROR: 27Invalid opcode\n");
						error = 1;
						break;
					}
				}
				break;
			}
			
			//JXX
			case '7': {
				
				char* hexadr = convertToHex(start+pc);
				char next[11];
				next[10] = '\0';
				int i;
				for (i = 0; i < 10; i++){
					
					next[i] = text[pc+i];
				}
				
				pc++;
				switch (text[pc]){
					
					//jmp
					case '0': {
						
						
						pc++;
						char* jump = (char*)malloc(sizeof(char)*9);
						jump[8] = '\0';
						int j;
						for (j = 0; j < 4; j++){
							
							jump[(8-((2*j)+1))-1] = text[pc];
							pc++;
							jump[(8-(2*j))-1] = text[pc];
							pc++;
						}
						
						printf("0x%s\tJMP\t0x%s\t%s\n",hexadr,jump,next);				
						free(jump);
						break;
					}
					
					//jle
					case '1': {
						
						pc++;
						char* jump = (char*)malloc(sizeof(char)*9);
						jump[8] = '\0';
						int j;
						for (j = 0; j < 4; j++){
							
							jump[(8-((2*j)+1))-1] = text[pc];
							pc++;
							jump[(8-(2*j))-1] = text[pc];
							pc++;
						}
						
						printf("0x%s\tJLE\t0x%s\t%s\n",hexadr,jump,next);				
						free(jump);
						break;
					}
					
					//jl
					case '2': {
						
						pc++;
						char* jump = (char*)malloc(sizeof(char)*9);
						jump[8] = '\0';
						int j;
						for (j = 0; j < 4; j++){
							
							jump[(8-((2*j)+1))-1] = text[pc];
							pc++;
							jump[(8-(2*j))-1] = text[pc];
							pc++;
						}
						
					printf("0x%s\tJL\t0x%s\t%s\n",hexadr,jump,next);					
						free(jump);
						break;
					}
					
					//je
					case '3': {
						
						pc++;
						char* jump = (char*)malloc(sizeof(char)*9);
						jump[8] = '\0';
						int j;
						for (j = 0; j < 4; j++){
							
							jump[(8-((2*j)+1))-1] = text[pc];
							pc++;
							jump[(8-(2*j))-1] = text[pc];
							pc++;
						}
						
						printf("0x%s\tJE\t0x%s\t%s\n",hexadr,jump,next);					
						free(jump);
						break;
					}
					
					//jne
					case '4': {
						
						pc++;
						char* jump = (char*)malloc(sizeof(char)*9);
						jump[8] = '\0';
						int j;
						for (j = 0; j < 4; j++){
							
							jump[(8-((2*j)+1))-1] = text[pc];
							pc++;
							jump[(8-(2*j))-1] = text[pc];
							pc++;
						}
						
						printf("0x%s\tJNE\t0x%s\t%s\n",hexadr,jump,next);				
						free(jump);
						break;
					}
					
					//jge
					case '5': {
						
						pc++;
						char* jump = (char*)malloc(sizeof(char)*9);
						jump[8] = '\0';
						int j;
						for (j = 0; j < 4; j++){
							
							jump[(8-((2*j)+1))-1] = text[pc];
							pc++;
							jump[(8-(2*j))-1] = text[pc];
							pc++;
						}
						
						printf("0x%s\tJGE\t0x%s\t%s\n",hexadr,jump,next);					
						free(jump);
						break;
					}
					
					//jg
					case '6': {
						
						pc++;
						char* jump = (char*)malloc(sizeof(char)*9);
						jump[8] = '\0';
						int j;
						for (j = 0; j < 4; j++){
							
							jump[(8-((2*j)+1))-1] = text[pc];
							pc++;
							jump[(8-(2*j))-1] = text[pc];
							pc++;
						}
						
						printf("0x%s\tJG\t0x%s\t%s\n",hexadr,jump,next);				
						free(jump);
						break;
					}
					
					default : {
						
						fprintf(stderr, "28ERROR: Invalid directive");
						error = 1;
						break;
					}
				}
				break;
			}
			
			//call
			case ('8'): {
				
				char* hexadr = convertToHex(start+pc);
				char next[11];
				next[10] = '\0';
				int i;
				for (i = 0; i < 10; i++){
				
					next[i] = text[pc+i];
				}
				pc++;
				if (text[pc] != '0'){
					
					fprintf(stderr, "29ERROR: Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;	
				char* call = (char*)malloc(sizeof(char)*9);
				call[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
			
					call[(8-((2*j)+1))-1] = text[pc];
					pc++;
					call[(8-(2*j))-1] = text[pc];
					pc++;
				}
				
				printf("0x%s\tCALL\t0x%s\t%s\n", hexadr,call,next);
				free(call);
				break;
			}
			
			//ret
			case ('9'): {
				
				char* hexadr = convertToHex(start+pc);
				pc++;
				if (text[pc] != '0'){
					
					fprintf(stderr, "30ERROR: Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;
				printf("0x%s\tRET\t\t\t90\n",hexadr);
				break;
			}
			
			//push
			case ('A'): 
			case ('a'): {
				
				char* hexadr = convertToHex(start+pc);
				char next[5];
				next[4] = '\0';
				int i;
				for (i = 0; i < 4; i++){
				
					next[i] = text[pc+i];
				}
				pc++;
				if (text[pc] != '0'){
					
					fprintf(stderr, "31ERROR: Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					
					fprintf(stderr, "32ERROR: Invalid register\n");
					error = 1;
					break;
				}
				int ra = text[pc]-48;
				
				pc++;
				if (text[pc] != 'f' && text[pc] != 'F'){
					
					fprintf(stderr, "33ERROR: Invalid register\n");
					error = 1;
					break;
				}
					
				printf("0x%s\tPUSHL\t%s\t\t%s\n", hexadr, registers(ra),next);
				pc++;
				break;
				
			}
			
			//pop
			case ('B'):
			case ('b'): {
				
				char* hexadr = convertToHex(start+pc);
				char next[5];
				next[4] = '\0';
				int i;
				for (i = 0; i < 4; i++){
				
					next[i] = text[pc+i];
				}
				pc++;
				if (text[pc] != '0'){
					
					fprintf(stderr, "34ERROR: Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					
					fprintf(stderr, "35ERROR: Invalid register\n");
					error = 1;
					break;
				}
				int ra = text[pc]-48;
				
				pc++;
				if (text[pc] != 'f' && text[pc] != 'F'){
					
					fprintf(stderr, "35ERROR: Invalid register\n");
					error = 1;
					break;
				}
				
				printf("0x%s\tPOP\t%s\t\t%s\n", hexadr, registers(ra),next);
				pc++;
				break;
			}
			
			//read
			case 'c':
			case 'C': {
				
				char* hexadr = convertToHex(start+pc);
				char next[13];
				next[12] = '\0';
				int i;
				for (i = 0; i < 12; i++){
					
					next[i] = text[pc+i];
				}
				
				pc++;
				//readb
				if (text[pc] == '0'){
					
					pc++;
					if (text[pc] < '0' || text[pc] > '7'){
						
						fprintf(stderr, "36ERROR: Invalid register\n");
						error = 1;
						break;
					}
					int ra = text[pc]-48;
					
					pc++;
					if (text[pc] != 'f' && text[pc] != 'F'){
						
						fprintf(stderr, "37ERROR: Invalid register\n");
						error = 1;
						break;
					}
					
					pc++;
					char* disp = (char*)malloc(sizeof(char)*9);
					disp[8] = '\0';
					int j;
					for (j = 0; j < 4; j++){
					
						disp[(8-((2*j)+1))-1] = text[pc];
						pc++;
						disp[(8-(2*j))-1] = text[pc];
						pc++;
					}
					displacement = toDec(disp);
				
					printf("0x%s\tREADB\t%s\t\t%s\n", hexadr, registers(ra), next);
					
					free(disp);
					
					break;
					
				}
				
				//readl
				else if (text[pc] == '1'){
					
					pc++;
					if (text[pc] < '0' || text[pc] > '7'){
						
						fprintf(stderr, "38ERROR: Invalid register\n");
						error = 1;
						break;
					}
					int ra = text[pc]-48;
					
					pc++;
					if (text[pc] != 'f' && text[pc] != 'F'){
						
						fprintf(stderr, "39ERROR: Invalid register\n");
						error = 1;
						break;
					}
					
					pc++;
					char* disp = (char*)malloc(sizeof(char)*9);
					disp[8] = '\0';
					int j;
					for (j = 0; j < 4; j++){
					
						disp[(8-((2*j)+1))-1] = text[pc];
						pc++;
						disp[(8-(2*j))-1] = text[pc];
						pc++;
					}
					
					displacement = toDec(disp);
					free(disp);
					printf("0x%s\tREADL\t%s\t\t%s\n", hexadr, registers(ra), next);
					break;
					
				}
				
				else {
					
					fprintf(stderr, "40ERROR: Invalid register\n");
					error = 1;
					break;
				}
			}
			
			//write
			case 'd':
			case 'D': {
				
				char* hexadr = convertToHex(start+pc);
				char next[13];
				next[12] = '\0';
				int i;
				for (i = 0; i < 12; i++){
					
					next[i] = text[pc+i];
				}
				
				pc++;
				//writeb
				if (text[pc] ==  '0'){
					
					pc++;
					if (text[pc] < '0' || text[pc] > '7'){
						
						fprintf(stderr, "41ERROR: Invalid register\n");
						error = 1;
						break;
					}
					int ra = text[pc]-48;
					
					pc++;
					if (text[pc] != 'f' && text[pc] != 'F'){
						
						fprintf(stderr, "42ERROR: Invalid register\n");
						error = 1;
						break;
					}
					
					pc++;
					char* disp = (char*)malloc(sizeof(char)*9);
					disp[8] = '\0';
					int j;
					for (j = 0; j < 4; j++){
					
						disp[(8-((2*j)+1))-1] = text[pc];
						pc++;
						disp[(8-(2*j))-1] = text[pc];
						pc++;
					}
					displacement = toDec(disp);
					free(disp);
					printf("0x%s\tWRITEB\t%s\t\t%s\n", hexadr, registers(ra), next);


				}
				
				//writel
				else if (text[pc] == '1'){
					
					pc++;
					if (text[pc] < '0' || text[pc] > '7'){
						
						fprintf(stderr, "43ERROR: Invalid register\n");
						error = 1;
						break;
					}
					int ra = text[pc]-48;
					
					pc++;
					if (text[pc] != 'f' && text[pc] != 'F'){
						
						fprintf(stderr, "44ERROR: Invalid register\n");
						error = 1;
						break;
					}
					
					pc++;
					char* disp = (char*)malloc(sizeof(char)*9);
					disp[8] = '\0';
					int j;
					for (j = 0; j < 4; j++){
					
						disp[(8-((2*j)+1))-1] = text[pc];
						pc++;
						disp[(8-(2*j))-1] = text[pc];
						pc++;
					}
					printf("0x%s\tWRITEL\t%s\t\t%s\n", hexadr, registers(ra), next);

					displacement = toDec(disp);
					free(disp);
					break;
				}
				
				else {
					
					fprintf(stderr, "45ERROR: Invalid register\n");
					error = 1;
					break;
				}
				break;
			}
			
			//movsbl
			case 'e':
			case 'E': {
				
				char* hexadr = convertToHex(start+pc);
				char next[13];
				next[12] = '\0';
				int i;
				for (i = 0; i < 12; i++){
				
					next[i] = text[pc+i];
				}
				pc++;
				if (text[pc] != '0'){					
					fprintf(stderr, "46ERROR: Invalid register\n");
					error = 1;
					break;
				}
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "47ERROR: Invalid register\n");
					error = 1;
					break;
				}
				int ra = text[pc]-48;
				
				pc++;
				if (text[pc] < '0' || text[pc] > '7'){
					fprintf(stderr, "48ERROR: Invalid register\n");
					error = 1;
					break;
				}
				int rb = text[pc]-48;
				
				pc++;
				char* disp = (char*)malloc(sizeof(char)*9);
				disp[8] = '\0';
				int j;
				for (j = 0; j < 4; j++){
					disp[(8-((2*j)+1))-1] = text[pc];
					pc++;
					disp[(8-(2*j))-1] = text[pc];
					pc++;
				}
				displacement = toDec(disp);
				
				printf("0x%s\tMOVSX\t%d(%s), %s\t%s\n", hexadr,displacement, registers(rb), registers(ra),next);
				free(disp);
				break;
			}
			
			default: {
				
				fprintf(stderr, "ERROR: Invalid opcode\n");
				error = 1;
				break;
			}
		}	
		if (error == 1)
			break;
	}
	
	free(text);
	fclose(fp);
	return 0;
}