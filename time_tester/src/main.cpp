#include "List.h"

typedef List<int> IntList;

int main(int argc, char *argv[])
{
	IntList list;
	list.Add(5).Add(10).Add(2);
	
	return 0;
}