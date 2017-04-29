#include "debug.h"
#include "arraylist.h"
#include "foreach.h"


void *foreach_init(arraylist_t *self){

    if(self == NULL || self->base == NULL)
        return NULL;

    void* beginning = get_index_al(self,0);

    return beginning;
}

void *foreach_next(arraylist_t *self, void* data){
    void *ret = NULL;

    if(data == NULL)
        return data;

    size_t index = get_data_al(self,data);

    if(index == UINT_MAX)
        return NULL;





    return ret;
}

size_t foreach_index(){
    size_t ret = 0;

    return ret;
}

bool foreach_break_f(){
    bool ret = false;

    return ret;
}

int apply(arraylist_t *self, int (*application)(void*)){
    int ret = 0;

    return ret;
}
