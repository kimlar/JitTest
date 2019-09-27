#include <Windows.h>
#include <iostream>

using namespace std;

__int64 sub(__int64 a, __int64 b)
{
	return a - b;
}

int main(int argc, char **argv)
{
	char code[] =
	{
		0x48, 0x89, 0xC8,		// mov rax, rcx
		0xC3,					// ret

		0x48, 0x83, 0xEC, 0x20,	// sub rsp, 0x20
		0xFF, 0xD0,				// call rax
		0x48, 0x83, 0xC4, 0x20,	// add rsp, 0x20
		0xC3					// ret
	};

	char *mem = static_cast<char *>(VirtualAlloc(0, sizeof(code), MEM_COMMIT, PAGE_EXECUTE_READWRITE));

	MoveMemory(mem, code, sizeof(code));

	auto setFunc = reinterpret_cast<void *(*)(void *)>(mem);
	auto callFunc = reinterpret_cast<__int64 (*)(__int64, __int64)>(mem + 4);

	setFunc(sub);
	__int64 r = callFunc(0, 1);
	cout << "r = " << r << endl;

	VirtualFree(mem, 0, MEM_RELEASE);

	cin.ignore();
	return 0;
}
