/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint16_t module_id_t;

typedef struct {
  int head;
  int next[1];
} UE_list_t;

typedef struct {
  char *name;
  void *(*setup)(void);
  void (*unset)(void **);
  int (*run)(
      module_id_t, int, int, int, UE_list_t *, int, int, uint8_t *, void *);
  void *data;
} default_sched_dl_algo_t;






int run_func(module_id_t mod_id, int b, int c, int d, UE_list_t *e, int g, int f, uint8_t *uu, void *vv) {
    // e->name = "run_func_name";
    printf("run_func\n");
}

void abc(void **d){
    // printf("%d\n", (int **)d);
}

void fun(char *a) {
    printf("fun %s\n", a);
}

void fun1(int a, int b) {
    printf("fun1\n");
}

void fun2(int a, int b) {
    printf("fun2\n");
}

void wrapper(void (*fun)(char *))
{
    fun("21332");
}

void *setup(void) {
    printf("setup\n");
}

struct S { 
    int x; 
    union code {
        int16_t re;
        int16_t im;
        int z;
    } Z;
    struct code2 {
        int a;
    } CC;
};

typedef struct S TT;


struct Q {
    int a;
    int b;
};





typedef long	 NR_ControlResourceSetId_t;


typedef struct NR_ControlResourceSet NR_ControlResourceSet;
/* NR_ControlResourceSet */
typedef struct NR_ControlResourceSet {
	NR_ControlResourceSetId_t	 controlResourceSetId;
	long	 duration;

} NR_ControlResourceSet_t;

#define A_SET_OF(type)                   \
    struct {                             \
        type **array;                    \
        int count; /* Meaningful size */ \
        int size;  /* Allocated size */  \
        void (*free)(type *);    \
    }

#define A_SEQUENCE_OF(type) A_SET_OF(type)

/* NR_PDCCH-Config */
typedef struct NR_PDCCH_Config {
    
	struct NR_PDCCH_Config__controlResourceSetToAddModList {
		A_SEQUENCE_OF(struct NR_ControlResourceSet) list;
		long a;
		/* Context for parsing across buffer boundaries */
		// asn_struct_ctx_t _asn_ctx;
	} *controlResourceSetToAddModList;


} NR_PDCCH_Config_t;

int main()
{

    int aa = 5;
    NR_ControlResourceSet *bb = (NR_ControlResourceSet *)malloc(sizeof(NR_ControlResourceSet));
    bb->controlResourceSetId = 1;

    printf("bb %ld\n", bb->controlResourceSetId);

    // struct NR_PDCCH_Config__controlResourceSetToAddModList *controlResourceSetToAddModList = malloc(sizeof(struct NR_PDCCH_Config__controlResourceSetToAddModList));
    struct NR_PDCCH_Config__controlResourceSetToAddModList *controlResourceSetToAddModList = (struct NR_PDCCH_Config__controlResourceSetToAddModList *)malloc(sizeof(struct NR_PDCCH_Config__controlResourceSetToAddModList));

    controlResourceSetToAddModList->list.array = (struct NR_ControlResourceSet **)malloc(sizeof(struct NR_ControlResourceSet *));
    for(int i = 0; i < 2; i++) {
        controlResourceSetToAddModList->list.array[i] = (struct NR_ControlResourceSet *)malloc(sizeof(struct NR_ControlResourceSet));
    }

    printf("bb %p\n", bb);
        controlResourceSetToAddModList->list.array[0] = bb;

    printf("controlResourceSetToAddModList->list.array[0] %p\n", controlResourceSetToAddModList->list.array[0]);

    // if(controlResourceSetToAddModList->list.array[0] == NULL) {

    // }
    // printf("132 %ld\n", controlResourceSetToAddModList->list.array[0]->controlResourceSetId);

    free(bb);
    free(controlResourceSetToAddModList);
    printf("done\n");
    exit(0);







    struct Q *q = (struct Q*)malloc(sizeof(struct Q));
    q->a = 2;
    printf("%d\n", q->a);
    free(q);
    TT tt;
    tt.Z.z = 1111111;
    tt.CC.a = 5;
    // Z i;
    // i.z = 327700;
    printf("%d - %d \n", tt.Z.re, tt.CC.a);  // 120 - x
struct abc_t *ab = NULL;    

    TT t;
    t.x = 2;
    printf("tx = %d\n", t.x);

    printf("Hello World\n");
    
    // struct NR_PDCCH_Config__controlResourceSetToAddModList *controlResourceSetToAddModList = choice.setup->controlResourceSetToAddModList;
    
    default_sched_dl_algo_t *a = (default_sched_dl_algo_t *)malloc(sizeof(default_sched_dl_algo_t));
    
    a->name = "1";
    // a->setup = &fun;
    wrapper(fun);
    a->setup = setup;
    a->unset = abc;
    a->run = run_func;

    UE_list_t *b = (UE_list_t *)a;


    a->run(1, 2, 3, 4, NULL, 0, 0, 0, NULL);

    // module_id_t *b = a;


    a->unset((void**)555);

    a->setup();
    // void (*fun_ptr)(int, int) = fun;  // & removed
  
    // void *(*setup)(void) = fun_ptr;

    // void (*fun_arr[])(int, int) = {fun, fun1, fun2};

    // fun_ptr(5, 1);  // * removed

    // fun_arr[2](1, 2);

    printf("%s\n", a->name);
    
    free(a);

    return 0;
}
