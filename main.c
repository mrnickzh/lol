#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SEARCH_S 256

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
	
	for (int i = 0; i < fsize; ) {
		size_t flag_pos = o_off;
		o_buff[o_off++] = 0;
		for (int flag_i = 0; flag_i < 8; flag_i++) {
			if (i >= fsize) { 
				o_buff[flag_pos] |= (1 << flag_i);
				o_buff[o_off++] = (unsigned char)0;
				o_buff[o_off++] = (unsigned char)0;
				goto finish;
			}
			
			int win_start = (i < SEARCH_S) ? 0 : (i - SEARCH_S);
			
			/* if (s_size >= SEARCH_S) { memset(s_buff, 0, SEARCH_S); s_size = 0; } */
			
			/* if (s_size == 0) { 
				s_buff[s_size++] = i_buff[i];
				o_buff[o_off] = i_buff[i];
				o_off++;
				i++;
				continue;
			} */
			
			int sect_len = 0;
			int offset = 0;
			for (int j = win_start; j < i; j++) {
				int curr_len = 0;
				while ((i_buff[i + curr_len] == i_buff[j + curr_len]) && curr_len < SEARCH_S-1) { curr_len++; }
				if (curr_len > sect_len) {
					offset = i - j;
					sect_len = curr_len;
				}
			}
			
			if (sect_len <= 2) { 
				/* s_buff[s_size++] = i_buff[i]; */
				o_buff[o_off++] = i_buff[i];
				i++;
			}
			
			/* else if (sect_len <= 4) {
				for (int k = 0; k < sect_len; k++) {
					s_buff[s_size++] = i_buff[i+k];
					o_buff[o_off++] = i_buff[i+k];
				}
				i += sect_len;
				flag_i += sect_len;
			} */
			
			else { 
				o_buff[flag_pos] |= (1 << flag_i);
			
				/* for (int k = 0; k < sect_len; k++) {
					s_buff[s_size++] = i_buff[i+k];
				}
				
				int offset = s_size - (sect_off + sect_len); */
				
				printf("%d sect_len\n", sect_len);
				printf("%d offset\n", offset);
				
				o_buff[o_off++] = (unsigned char)offset;
				o_buff[o_off++] = (unsigned char)sect_len;
				
				i += sect_len;
				
				/* for (int j = 0; j < s_size; j++) { putchar(s_buff[j]); }
				printf("\n========\n"); */
				for (int i = 7; i >= 0; i--) {
					putchar((o_buff[flag_pos] & (1 << i)) ? '1' : '0');
				}
				putchar('\n');
			}
		}
	}
	
	/*
	[ФЛАГОВЫЙ БАЙТ: 0 0 1 0 0 0 0 0] -> Означает: символ, символ, ССЫЛКА, символ, символ...
	[Данные 1: 8 бит символа]
	[Данные 2: 8 бит символа]
	[Данные 3: 16 бит ССЫЛКИ] -> Распаковщик знает, что надо взять 16 бит, потому что 3-й флаг был '1'
	[Данные 4: 8 бит символа]
	*/
finish:
	/* o_buff[o_off++] = '\0'; */
	/* printf("%s\n", o_buff); */
	
	FILE *ofp;
	ofp = fopen("output.lol", "wb");
	fwrite(o_buff, 1, o_off, ofp);
	fclose(ofp);
	
	return 0;
}

int main(int argc, char** argv) {
	if (argc < 3) { printf("usage: lol.exe -c/-d file\n"); return 0; }
	
	if (strcmp(argv[1], "-c") == 0) {
		printf("-c %s\n", argv[2]);
		compress(argv[2]);
		printf("compressed!\n", argv[2]);
		return 0;
	}
	
	if (strcmp(argv[1], "-d") == 0) {
		printf("-d %s\n", argv[2]);
		return 0;
	}
	
	printf("usage: lol.exe -c/-d file\n");
	return 0;
}