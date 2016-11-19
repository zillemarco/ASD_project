#include "List.h"

typedef List<int> IntList;

int main(int argc, char *argv[])
{
	IntList list(10);

	for (int i = 0; i < 10; i++)
		list.Add(i);

	list.RemoveAt(4);
	list.Clear(2);

	return 0;
}