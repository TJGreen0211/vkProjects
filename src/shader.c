#include "shader.h"

char *readShader(char *filename, size_t *size)
{
	FILE *fp;
	char *content = NULL;

	size_t count = 0;
	if(filename != NULL)
	{
		fp = fopen(filename, "rb");
		if(fp != NULL){
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) {
            	content = (char *)malloc(sizeof(char) * (count+1));
            	count = fread(content,sizeof(char),count,fp);
            	content[count] = '\0';
        	}
        }
        fclose(fp);
	}
	*size = count;
	return content;
}