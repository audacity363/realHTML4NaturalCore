#ifndef RH4NLIBUTILSNAMESTACK
#define RH4NLIBUTILSNAMESTACK

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char **names;
    uint32_t length;
} rh4nNameStack;

#define rh4nUtilsInitNameStack(stack) (stack)->length = 0; (stack)->names = NULL;

char *rh4nUtilsPopNamefromStack(rh4nNameStack *stack);
char *rh4nUtilsPushNametoStack(rh4nNameStack *stack, const char *name);
void rh4nUtilsDeinitNameStack(rh4nNameStack *stack);

#ifdef __cplusplus
}
#endif

#endif
