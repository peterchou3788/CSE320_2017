#include "arraylist.h"
#include <errno.h>

/**
 * @visibility HIDDEN FROM USER
 * @return     true on success, false on failure
 */
static bool resize_al(arraylist_t* self){

    if(self == NULL)
    {
        errno = EINVAL;
        return false;
    }

    P(&self->mutex);
    size_t length = self->length;
    size_t capacity = self->capacity;
    void* baseAddress = self->base;

    if(length == capacity)
    {
        size_t newCapacity = capacity *2;
        size_t newSize = newCapacity * self->item_size;

        if(realloc(baseAddress,newSize)==NULL)
        {
            errno = ENOMEM;
            V(&self->mutex);
            return false;
        }
        self->capacity = newCapacity;
        V(&self->mutex);
        return true;
    }

    if(length == capacity/2 -1)
    {
        size_t newCapacity = capacity/2;
        size_t newSize= newCapacity* self->item_size;

        if(newSize <= INIT_SZ)
        {
            newCapacity = INIT_SZ;
            newSize = INIT_SZ * self->item_size;
        }

        if(realloc(baseAddress,newSize)==NULL)
        {
            errno = ENOMEM;
            V(&self->mutex);
            return false;
        }
        self->capacity = newCapacity;
        V(&self->mutex);
        return true;
    }


    return false;
}

arraylist_t *new_al(size_t item_size){

    arraylist_t* init = (arraylist_t*)malloc(sizeof(arraylist_t));

    if(item_size == 0)
        item_size = 2;

    if(init == NULL)
    {
        errno = ENOMEM;
        return NULL;
    }
    init->base = (void*)calloc(1,item_size * INIT_SZ);
    init->capacity =  INIT_SZ;
    init->item_size = item_size;
    init->length = 0;

    sem_init(&(init->mutex),0,1);
    return init;
}

size_t insert_al(arraylist_t *self, void* data){

    if(self == NULL || data == NULL)
    {
        errno = EINVAL;
        return UINT_MAX;
    }
    P(&self->mutex);
    size_t lengthCurrent = self->length;
    V(&self->mutex);

    if(lengthCurrent == self->capacity)
    {

        resize_al(self);

    }
    P(&self->mutex);
    size_t insertLength = lengthCurrent*(self->item_size);

    char* insertAddress = (char*)(self->base) + insertLength;
    memcpy(insertAddress,data,self->item_size);

    self->length = self->length + 1;
    V(&self->mutex);

    return self->length;
}

size_t get_data_al(arraylist_t *self, void *data){
    if(data == NULL)
        return 0;
    //char* address = self->base;
    for(int i = 0;i<self->length;i++)
    {
        P(&self->mutex);
        char* address = (char*)self->base + self->item_size * i;
        int retcmp = memcmp(address,data,self->item_size);
        V(&self->mutex);
        if(retcmp == 0)
        {
            return (size_t)i;
        }
    }

    errno = EINVAL;
    return UINT_MAX;
}

void *get_index_al(arraylist_t *self, size_t index){

    if(self == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    void* data = NULL;
    P(&self->mutex);
    if(index >= self->length)
    {
        size_t lastIndex = self->item_size * self->length;
        void* lastData = (char*)self->base + lastIndex;
        memcpy(data,lastData,self->item_size);
        V(&self->mutex);
        return data;
    }

    size_t offset = index * self->item_size;
    void* tempData = (char*)self->base + offset;
    memcpy(data,tempData,self->item_size);
    V(&self->mutex);

    return data;
}

bool remove_data_al(arraylist_t *self, void *data){
    size_t index = 0;

    if(data == NULL)
    {
        P(&self->mutex);
        for(int i = 0;i<self->length;i++)
        {

            char* address = (char*)self->base + self->item_size * i;
            char* address2 = (char*)self->base + self->item_size * (i+1);

            memmove(address,address2,self->item_size);

        }
        V(&self->mutex);

        P(&self->mutex);
        self->length = self->length -1;

        if(self->length <= (self->capacity/2)-1)
        {
            V(&self->mutex);
            resize_al(self);
            P(&self->mutex);
        }
        V(&self->mutex);
        return true;

    }

    index = get_data_al(self,data);

    if(index == UINT_MAX)
        return false;

    P(&self->mutex);
    for(int i = index;i<self->length;i++)
    {
        char* address = (char*)self->base + self->item_size * i;
        char* address2 = (char*)self->base + self->item_size * (i+1);
        memmove(address,address2,self->item_size);
    }
    V(&self->mutex);

    P(&self->mutex);
    self->length = self->length -1;

    if(self->length <= (self->capacity/2)-1)
    {
        V(&self->mutex);
        resize_al(self);
        P(&self->mutex);
    }
    V(&self->mutex);
    return true;

}

void *remove_index_al(arraylist_t *self, size_t index){

    char* address = NULL;
    void* tempAddress = NULL;
    P(&self->mutex);
    if(index >= self->length)
    {

        address = (char*)self->base + self->item_size * self->length;
        memcpy(tempAddress,address,self->item_size);

        self->length = self->length -1;
        if(self->length <= (self->capacity/2)-1)
        {
            V(&self->mutex);
            resize_al(self);
            P(&self->mutex);
        }
        V(&self->mutex);
        return tempAddress;
    }
    V(&self->mutex);

    address = (char*)self->base + self->item_size * index;
    memcpy(tempAddress,address,self->item_size);

    P(&self->mutex);
    for(int i = index; i< self->length;i++)
    {
        char* address = (char*)self->base + self->item_size * i;
        char* address2 = (char*)self->base + self->item_size * (i+1);
        memmove(address,address2,self->item_size);
    }
    self->length = self->length-1;
    if(self->length <= (self->capacity/2)-1)
    {
        V(&self->mutex);
        resize_al(self);
        P(&self->mutex);
    }
    V(&self->mutex);
    return tempAddress;

}

void delete_al(arraylist_t *self, void (*free_item_func)(void*)){

    //self->item_size = 0;
    P(&self->mutex);
    for(int i = 0; i < self->length;i++)
    {
        char* address = (char*)self->base + self->item_size * i;
        free_item_func(address);
    }
    self->capacity = 0;
    self->length = 0;
    V(&self->mutex);
    free(self->base);
    return;
}

