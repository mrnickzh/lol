#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SEARCH_S 255

int compress(char* file) {
	FILE *fp;
	fp = fopen(file, "rb");
	if (fp == NULL) { printf("cant open file %s\n", file); return 1; }
	
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	char *i_buff = malloc(fsize);
	
	size_t o_off = 0;
	char *o_buff = malloc(fsize * 2);
	
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
			
			int sect_len = 0;
			int offset = 0;
			for (int j = win_start; j < i; j++) {
				int curr_len = 0;
				while ((i + curr_len < fsize) && (j + curr_len < i) && (i_buff[i + curr_len] == i_buff[j + curr_len]) && curr_len < SEARCH_S-1) { curr_len++; }
				if (curr_len > sect_len) {
					offset = i - j;
					sect_len = curr_len;
				}
			}
			
			if (sect_len <= 2) { 
				o_buff[o_off++] = i_buff[i];
				i++;
			}
			
			else { 
				o_buff[flag_pos] |= (1 << flag_i);
				
				/* printf("%d sect_len\n", sect_len);
				printf("%d offset\n", offset); */
				
				o_buff[o_off++] = (unsigned char)offset;
				o_buff[o_off++] = (unsigned char)sect_len;
				
				i += sect_len;
				
				/* for (int b = 7; b >= 0; b--) {
					putchar((o_buff[flag_pos] & (1 << b)) ? '1' : '0');
				}
				putchar('\n'); */
			}
		}
		
		/* printf("%d\n", i); */
	}
	
finish:
	FILE *ofp;
	ofp = fopen("output.lol", "wb");
	fwrite(o_buff, 1, o_off, ofp);
	fclose(ofp);
	
	return 0;
}

int decompress(char* file) {
	FILE *fp;
	fp = fopen(file, "rb");
	if (fp == NULL) { printf("cant open file %s\n", file); return 1; }
	
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	char *i_buff = malloc(fsize);
	
	size_t o_size = 0;
	size_t of_size = fsize;
	char *o_buff = malloc(of_size);
	
	size_t s_size = 0;
	char s_buff[SEARCH_S];
	
	if((i_buff == NULL) || (o_buff == NULL)) { return 1; }
	
	if (fread(i_buff, sizeof(char), fsize, fp) != fsize) { return 1; }
	
	for (int i = 0; i < fsize; ) {
		size_t flag_pos = i++;
		unsigned int flag = i_buff[flag_pos];
		for (int flag_i = 0; flag_i < 8; flag_i++) {
			int bit = (flag >> flag_i) & 1;
			if (bit) {
				unsigned char offset = i_buff[i++];
				unsigned char sect_len = i_buff[i++];
				if (!offset && !sect_len) { goto finish; }
				if (o_size + sect_len >= of_size) {
					/* printf("o_size %d, sect_len %d, of_size %d realloc\n", o_size, sect_len, of_size); */
					of_size += (sect_len > 64 ? sect_len : 64);
					o_buff = realloc(o_buff, of_size);
				}
				
				/* printf("%d sect_len\n", sect_len);
				printf("%d offset\n", offset); */
				
				for (int k = 0; k < sect_len; k++) {
					o_buff[o_size + k] = o_buff[o_size - offset + k];
				}
				
				o_size += sect_len;
			}
			else {
				o_buff[o_size] = i_buff[i];
				o_size++;
				i++;
				
				if (o_size >= of_size) {
					/* printf("o_size %d, of_size %d realloc\n", o_size, of_size); */
					of_size += 64;
					o_buff = realloc(o_buff, of_size);
				}
			}
		}
		/* printf("%d\n", i); */
	}
	
finish:
	FILE *ofp;
	ofp = fopen("output.lold", "wb");
	fwrite(o_buff, 1, o_size, ofp);
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
		decompress(argv[2]);
		printf("decompressed!\n", argv[2]);
		return 0;
	}
	
	printf("usage: lol.exe -c/-d file\n");
	return 0;
}