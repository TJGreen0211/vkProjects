#include "loadObject.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include <tinyobj/tinyobj_loader_c.h>

static const char *mmap_file(size_t* len, const char* filename) {
	FILE* f;
	long file_size;
	char* p;
	int fd;

	(*len) = 0;

	f = fopen(filename, "r");
	fseek(f, 0, SEEK_END);
	file_size = ftell(f);
	fclose(f);
	fd = _open(filename, 0);

	p = (char*)mmap(0, (size_t)file_size, PROT_READ, MAP_SHARED, fd, 0);

	if (p == MAP_FAILED) {
		perror("mmap");
		return NULL;
	}

	if (fclose(f) == -1) {
		//perror("close");
		//return NULL;
	}

	(*len) = (size_t)file_size;

	return p;
}

static const char *get_file_data(size_t *len, const char *filename) {
	const char *data = NULL;
	size_t dataLen = 0;

	data = mmap_file(&dataLen, filename);
	(*len) = dataLen;
	return data;
}

void loadObject(const char *filepath) {
	const char *MODEL_PATH = "conc.obj";
	//const *char TEXTURE_PATH = "";
	tinyobj_attrib_t attrib;
	tinyobj_shape_t *shapes;
	tinyobj_material_t *materials;
	size_t numMaterials;
	size_t numShapes;
	size_t dataLen = 0;

	unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
	const char *data = get_file_data(&dataLen, MODEL_PATH);
	if(data == NULL) {
		printf("Failed to load object file.");
		exit(1);
	}
	int ret = tinyobj_parse_obj(&attrib, &shapes, &numShapes, &materials, &numMaterials, data, dataLen, flags);
	printf("# of shapes    = %d\n", (int)numShapes);
    printf("# of materials = %d\n", (int)numMaterials);

	if(ret != TINYOBJ_SUCCESS) {
		exit(1);
	}
}