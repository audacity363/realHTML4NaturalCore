#ifndef RH4NJSONGENERATOR
#define RH4NJSONGENERATOR

#ifdef __cplusplus
extern "C" {
#endif

#define RH4NJSON_PRINTNAME(file, name) write(file, "\"", 1); \
                                       write(file, name, strlen(name)); \
                                       write(file, "\":", 2);

#define RH4NJSON_PRINTARRAYCLOSE(file) write(file, "]", 1);
#define RH4NJSON_PRINTARRAYOPEN(file) write(file, "[", 1);
#define RH4NJSON_PRINTOBJECTOPEN(file) write(file, "{", 1);
#define RH4NJSON_PRINTOBJECTCLOSE(file) write(file, "}", 1);

typedef struct {
    int aimdim;
    int length[3];
    int index[3];
} RH4nJSONObjectArrayParms_t;

//Public
void rh4njsonPrintJSON(RH4nVarList*, int, RH4nProperties*);
int rh4njsonPrintJSONToFile(RH4nVarList*, char*, RH4nProperties*);


//Private
bool rh4njsonCheckIfJSONArray(RH4nVarEntry_t*, RH4nProperties*, short);
int rh4njsonCountGroups(RH4nVarEntry_t*, RH4nProperties*);
bool rh4njsonCheckIfOnlyOneGroup(RH4nVarEntry_t*, RH4nProperties*);
void rh4njsonPrintObject(RH4nVarEntry_t*, int, int, RH4nProperties*);
void rh4njsonPrintJSONArray(RH4nVarObj*, int, RH4nProperties*);
void rh4njsonPrintJSONObjectArray(RH4nVarEntry_t*, int, RH4nProperties*);
void rh4njsonpregetCommonDimension(RH4nVarEntry_t*, int[3], int[3], RH4nProperties*);
void rh4njsoncheckCommonDimension(int[3], int[3], int[3]);
void rh4njsonMergeCommonDimResults(int[3], int[3], int[3], int[3], RH4nProperties*);
int rh4njsongetCommonDimension(int[3]); 
void rh4njsonPrintObjectArrayEntry(RH4nVarEntry_t*, int, int, RH4nJSONObjectArrayParms_t, RH4nProperties*);

#ifdef __cplusplus
}
#endif

#endif
