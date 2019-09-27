#include <iostream>

#include <Windows.h>

// Allocate executable memory for the JIT-ted program.
// Do not forget to protect the memory as soon done writing to it for security reasons.
void* AllocateExecutableMemory(size_t size)
{
	void* memory = VirtualAlloc(0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (memory == NULL)
	{
		//printf("ERROR: Could not allocate executable memory for JIT");
		return NULL;
	}

	return memory;
}

// Write to the executable memory. Length is the length of code.
// Do not forget to protect the memory as soon done writing to it for security reasons.
void WriteExecutableMemory(void* memory, unsigned char* code, int length)
{
	memcpy(memory, code, length);
}

// It is very important to protect the executable memory for security reasons.
// The memory should be protected at all time except when writing/rewriting that memory.
void ProtectExecutableMemory(void* memory)
{
	DWORD old;
	VirtualProtect(memory, sizeof(memory), PAGE_EXECUTE_READ, &old);
}

// Do not forget to protect the memory as soon done writing to it for security reasons.
void UnprotectExecutableMemory(void* memory)
{
	DWORD old;
	VirtualProtect(memory, sizeof(memory), PAGE_READWRITE, &old);
}

// Do not forget to release executable memory when done with it.
void ReleaseExecutableMemory(void* memory)
{
	VirtualFree(memory, 0, MEM_RELEASE);
}

int main(int argc, char* argv[])
{
	/*
		mov eax, 0  <----------- int 0 (4 bytes). Changable with this: memcpy(&code[1], &num, 4); // num is int
		ret
	*/
	//                     mov eax         int=0           ret                 
	//                       <-->  <-------------------->  <-->
	//unsigned char code[] = { 0xb8, 0x00, 0x00, 0x00, 0x00, 0xc3 };
	/*
	unsigned char code[] = {
		0xb8, // mov eax
		0x02, 0x00, 0x00, 0x00, // 2 (int)
		0xb9, // mov ecx
		0x03, 0x00, 0x00, 0x00, // 3 (int)
		0xc3 // ret
	};
	*/

	// b8 03 00 00 00 83 c0 02 c3
	unsigned char code[] = {
		0xb8, // mov eax, 3
		0x03, 0x00, 0x00, 0x00, // 3 (32 bit form as 4 bytes=int)
		0x83, // add eax, 2 // 0x83 = add,
		0xc0, // ModR/M with immediate 8 bit value
		0x02, // 2 (8 bit form as 1 byte)
		0xc3 // ret
	};

	void* mem = AllocateExecutableMemory(sizeof(code));
	WriteExecutableMemory(mem, code, sizeof(code));
	ProtectExecutableMemory(mem);
	int(*func)() = reinterpret_cast<int(*)()>(mem);
	printf("Number is %d\n", func());
	ReleaseExecutableMemory(mem);

	printf("\nPress any key to close");
	system("pause>nul");
	return 0;
}
