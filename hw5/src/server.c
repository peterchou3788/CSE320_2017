#include "debug.h"
#include "arraylist.h"
#include "foreach.h"

int main(int argc, char *argv[])
{
	arraylist_t* arraylist = new_al(5);
	debug("Capacity: %zu\n",arraylist->capacity);
	debug("Length: %zu\n",arraylist->length);
	debug("Item Size: %zu\n",arraylist->item_size);

	//int a = 20;
	char* string = "hello";
//	int h = 2;
//	int b = 5;
//	int c = 10;
/*	insert_al(arraylist,&a);
	insert_al(arraylist,&h);
	insert_al(arraylist,&b);
	insert_al(arraylist,&c);
	insert_al(arraylist,&a);*/

	for(int i = 0;i < 4;i++)
	{
		insert_al(arraylist,string);
	}

	debug("Length: %zu\n",arraylist->length);
	debug("Capacity: %zu\n",arraylist->capacity);
	for(int i = 0;i< arraylist->length;i++)
	{
		debug("Elements: %s\n",((char*)arraylist->base + i*arraylist->item_size));
	}

	return 0;
}
