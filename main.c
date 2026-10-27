#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SEARCH_S 128

int compress(char* file) {
	FILE *fp;
	fp = fopen(file, "rb");
	if (fp == NULL) { printf("cant open file %s\n", file); return 1; }
	
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	char *i_buff = malloc(fsize);
	
	size_t o_off = 0;
	char *o_buff = malloc(fsize);
	
	size_t s_size = 0;
	char s_buff[SEARCH_S];
	
	if((i_buff == NULL) || (o_buff == NULL)) { return 1; }
	
	if (fread(i_buff, sizeof(char), fsize, fp) != fsize) { return 1; }
	
	for (int i = 0; i < fsize; i++) {
		if (s_size >= SEARCH_S) { memset(s_buff, 0, SEARCH_S); s_size = 0; }
		
		if (s_size == 0) { s_buff[s_size++] = i_buff[i]; o_buff[o_off] = i_buff[i]; o_off++; continue; }
		
		int sect_len = 0;
		int sect_off = 0;
		for (int j = 0; j < s_size; j++) {
			int curr_len = 0;
			while (i_buff[i + curr_len] == s_buff[j + curr_len]) { curr_len++; }
			if (curr_len > sect_len) {
				sect_off = j;
				sect_len = curr_len;
				/* printf("%d %d !!!\n", j, sect_off); */
			}
		}
		
		/* while (i_buff[i + sect_len] == s_buff[sect_len]) { sect_len++; } */
		/* printf("%d sect_len\n", sect_len); */
		
		if (sect_len <= 1) { s_buff[s_size++] = i_buff[i]; o_buff[o_off++] = i_buff[i]; }
		
		else if (sect_len <= 4) {
			for (int k = 0; k < sect_len; k++) {
				s_buff[s_size++] = i_buff[i+k];
				o_buff[o_off++] = i_buff[i+k];
			}
			i += sect_len - 1;
		}
		
		else { 
			for (int k = 0; k < sect_len; k++) {
				s_buff[s_size++] = i_buff[i+k];
			}
			
			int offset = s_size - (sect_off + sect_len);
			printf("%d offset\n", offset);
		
			char tmp[3];
			itoa(sect_len, tmp, 10);
			
			o_buff[o_off++] = tmp[0];
			if (sect_len >= 10) {
				o_buff[o_off++] = tmp[1];
			}
			i += sect_len - 1;
			
			for (int j = 0; j < s_size; j++) { putchar(s_buff[j]); }
			printf("\n========\n");
		}
		
		/* for (int j = 0; j < s_size; j++) { putchar(s_buff[j]); }
		printf("\n========\n"); */
	}
	
	/*
	[ФЛАГОВЫЙ БАЙТ: 0 0 1 0 0 0 0 0] -> Означает: символ, символ, ССЫЛКА, символ, символ...
	[Данные 1: 8 бит символа]
	[Данные 2: 8 бит символа]
	[Данные 3: 16 бит ССЫЛКИ] -> Распаковщик знает, что надо взять 16 бит, потому что 3-й флаг был '1'
	[Данные 4: 8 бит символа]
	*/
	
	o_buff[o_off] = '\0';
	printf("%s\n", o_buff);
	
	return 0;
}

int main(int argc, char** argv) {
	if (argc < 3) { printf("usage: lol.exe -c/-d file\n"); return 0; }
	
	if (strcmp(argv[1], "-c") == 0) {
		printf("-c %s\n", argv[2]);
		compress(argv[2]);
		return 0;
	}
	
	if (strcmp(argv[1], "-d") == 0) {
		printf("-d %s\n", argv[2]);
		return 0;
	}
	
	printf("usage: lol.exe -c/-d file\n");
	return 0;
}