
extern "C"
{
extern void GetFSAPI(void);
extern void CreateInterface(void);

struct {const char *name; void *func;} lib_filesystem_stdio_exports[] = {
	{ "GetFSAPI", (void*)GetFSAPI },
	{ "CreateInterface", (void*)CreateInterface },
	{ 0, 0 }
};
}

