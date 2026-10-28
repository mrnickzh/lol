#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SEARCH_S 255

int compress(char* file) {
	FILE *fp, *ofp;
	char *i_buff, *o_buff;
	size_t s_size = 0, o_off = 0;
	long fsize;
	int i, j, flag_i;
	char s_buff[SEARCH_S];
	
	fp = fopen(file, "rb");
	if (fp == NULL) { printf("cant open file %s\n", file); return 1; }
	
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	i_buff = malloc(fsize);
	
	o_buff = malloc(fsize * 2);
                              
	
	if((i_buff == NULL) || (o_buff == NULL)) { return 1; }
	
	if (fread(i_buff, sizeof(char), fsize, fp) != fsize) { return 1; }
	
	for (i = 0; i < fsize; ) {
		size_t flag_pos = o_off;
		o_buff[o_off++] = 0;
		for (flag_i = 0; flag_i < 8; flag_i++) {
			int win_start, sect_len = 0, offset = 0;
			if (i >= fsize) { 
				o_buff[flag_pos] |= (1 << flag_i);
				o_buff[o_off++] = (unsigned char)0;
				o_buff[o_off++] = (unsigned char)0;
				goto finish;
			}
			
			win_start = (i < SEARCH_S) ? 0 : (i - SEARCH_S);
			
			for (j = win_start; j < i; j++) {
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
	ofp = fopen("output.lol", "wb");
	fwrite(o_buff, 1, o_off, ofp);
	fclose(ofp);
	
	return 0;
}

int decompress(char* file) {
	FILE *fp, *ofp;
	long fsize;
	char *i_buff, *o_buff;
	size_t o_size = 0, s_size = 0, of_size;
	int i, j, k;
	char s_buff[SEARCH_S];
	int flag_i;
	

	fp = fopen(file, "rb");
	if (fp == NULL) { printf("cant open file %s\n", file); return 1; }
	
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	i_buff = malloc(fsize);

	of_size = fsize;
	o_buff = malloc(of_size);
	
	if((i_buff == NULL) || (o_buff == NULL)) { return 1; }
	
	if (fread(i_buff, sizeof(char), fsize, fp) != fsize) { return 1; }
	
	for (i = 0; i < fsize; ) {
		size_t flag_pos = i++;
		unsigned int flag = i_buff[flag_pos];
		for (flag_i = 0; flag_i < 8; flag_i++) {
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
				
				for (k = 0; k < sect_len; k++) {
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