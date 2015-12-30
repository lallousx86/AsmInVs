#include "stdafx.h"
/*
Code copyright Elias Bachaalany <lallousz-x86@yahoo.com>

You may freely use this code. Please give credits where due.

http://lallouslab.net
*/

//--------------------------------------------------------------------------
int calculate(int count)
{
  int result;

  if (count <= 0)
    count = 1;

  __asm
  {
      push eax
      push ebx
      push edx
      mov eax, 0x11223344
      mov ebx, 0x55667788
      xor edx, edx
      mov ecx, count
    L_REPEAT:
      test ecx, 1
      jz L_ODD
      add edx, eax
      ror eax, 1
      jmp L_CONTINUE
    L_ODD:
      add edx, ebx
      rol edx, 1
    L_CONTINUE:
      loop L_REPEAT

      mov result, edx
      pop edx
      pop ebx
      pop eax
  }
  return result;
}

//--------------------------------------------------------------------------
void test_calculate()
{
	for (int i = 1; i < 10; i++)
		printf("calculate(%d)=%x\n", i, calculate(i));
}

//--------------------------------------------------------------------------
char *scramble_string(
	const char *src, 
	unsigned char k)
{
	char *result = NULL;
	__asm
	{
			push edi
			push esi
			push edx

			// get string length and bail out if zero
			push src
			call strlen
			add	esp, 4
			test eax, eax
			jz L_EXIT

			// malloc(strlen + 1)
			inc eax
			push eax
			call malloc
			add esp, 4 // cdecl, clean the stack
			test eax, eax // NULL pointer? exit
			jz L_EXIT

			mov edi, eax
			mov result, eax

			mov esi, src
			movzx edx, k
		L_NEXT:
			lodsb
			test al, al
			jz L_DONE
			xor al, dl
			stosb
			jmp L_NEXT
		L_DONE:
			mov al, 0
			stosb
		L_EXIT:
			pop edx
			pop esi
			pop edi
	}
	return result;
}

//--------------------------------------------------------------------------
void test_scramble()
{
	char *enc = scramble_string("hello world", 0xeb);
	if (enc != NULL)
	{
		char *dec = scramble_string(enc, 0xeb);
		free(enc);
		if (dec != NULL)
			free(dec);
	}
}

//--------------------------------------------------------------------------
// Creates a new function that generates a new function that no arguments
// but calls the bound function with its bound arguments
class cf1bind_t
{
private:
	void *func_body;
	size_t func_size;

	// Generate a function that calls c_func1 with the passed parameter
	static void *dyanmic_code(
		void *c_func1,
		void *param1,
		size_t *func_size)
	{
		//
		// Use static variables so we initialize them once
		//
		
		// The beginning of the asm code
		static PBYTE begin_addr = NULL;

		// Some offsets into the code and the code size
		static size_t code_size, param_offset, xfer_offset;

		// Grab the assembler code once
		if (begin_addr == NULL)
		{
			PBYTE end, param, xfero;
			__asm
			{
				jmp L_END
				L_BEGIN :
			L_PARAM_PLACEHOLDER:
				push 0x19810417 // 0x68 XXXXXXXX (5 bytes)
				call L_XFER_PLACEHOLDER
				add  esp, 4	// purge the stack
				ret
			L_XFER_PLACEHOLDER:
				push 0xaabbccdd
				ret
			}
			// Emit some free style bytes.
			// Use the __asm __emit to generate opcodes not supported by the
			// assembler or perhaps data
			__asm __emit 0x44 // 'D'
			__asm __emit 0x55 // 'U'
			__asm __emit 0x4D // 'M'
			__asm __emit 0x4D // 'M'
			__asm __emit 0x59 // 'Y'
			__asm
			{
			L_END:
				mov begin_addr, offset L_BEGIN
				mov param, offset L_PARAM_PLACEHOLDER
				mov xfero, offset L_XFER_PLACEHOLDER
				mov end, offset L_END
			}
			code_size = end - begin_addr;
			param_offset = param - begin_addr + 1;
			xfer_offset = xfero - begin_addr + 1;
		}

		// Allocate memory for the function body. Make writable but not executable (not yet)
		// Note: (demo code) it is not optimal to allocate a page per bound function
		PBYTE func_memory = (PBYTE)VirtualAlloc(
			NULL,
			code_size,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_READWRITE);

		if (func_memory == NULL)
			return NULL;

		// Copy the function
		memcpy(
			func_memory,
			begin_addr,
			code_size);

		// Customize the function body
		*((DWORD_PTR *)(func_memory + xfer_offset)) = (DWORD_PTR)c_func1;
		*((DWORD_PTR *)(func_memory + param_offset)) = (DWORD_PTR)param1;

		// Lock the memory and make executable only
		DWORD old_protect;
		if (VirtualProtect(
				func_memory, 
				code_size, 
				PAGE_EXECUTE, 
				&old_protect) == FALSE)
		{
			VirtualFree(func_memory, code_size, MEM_RELEASE);
			return NULL;
		}

		*func_size = code_size;
		return func_memory;
	}

	void free_func()
	{
		if (func_body == NULL)
			return;

		VirtualFree(
			func_body, 
			func_size, 
			MEM_RELEASE);
		func_body = NULL;
	}
public:
	cf1bind_t() : func_body(NULL), func_size(0)
	{
	}

	bool bind(void *c_func1, void *arg1)
	{
		free_func();
		func_body = dyanmic_code(
			c_func1, 
			arg1, 
			&func_size);
		
		return func_body != NULL;
	}

	void call()
	{
		typedef void(__cdecl *callfunc_t)(void);
		((callfunc_t)func_body)();
	}

	~cf1bind_t()
	{
		free_func();
	}

	static cf1bind_t *make(
		void *c_func1, 
		void *arg1)
	{
		cf1bind_t *f = new cf1bind_t();
		if (!f->bind(c_func1, arg1))
		{
			delete f;
			return NULL;
		}
		return f;
	}
};

//--------------------------------------------------------------------------
void __cdecl print_msg(char *msg)
{
	printf("%s", msg);
}

//--------------------------------------------------------------------------
void test_func_bind()
{
	cf1bind_t *say_hi = cf1bind_t::make(print_msg, "hi!\n");
	cf1bind_t *say_bye = cf1bind_t::make(print_msg, "bye!\n");

	say_hi->call();
	say_bye->call();

	delete say_bye;
	delete say_hi;
}

//--------------------------------------------------------------------------
size_t __declspec(naked) __stdcall sum_buf(
	const void *buf, 
	size_t count = size_t(-1))
{
	__asm
	{
		// Prolog <
		push ebp
		mov ebp, esp // >

		push esi
		push ecx
		push ebx

		mov esi, buf
		mov ecx, count
		cmp ecx, -1
		jnz L_START
		
		// strlen(buf) <
		push esi
		call strlen
		add esp, 4 // >

		mov ecx, eax
	L_START:
		xor eax, eax
		xor edx, edx
	L_LOOP:
		lodsb
		add edx, eax
		loop L_LOOP

		// Return value <
		mov eax, edx
		xor edx, edx // >

		// Restore saved registers <
		pop ebx
		pop ecx
		pop esi // >
		
		// Epilog <
		mov esp, ebp 
		pop ebp	// >
		
		ret	4*2
	}
}

//--------------------------------------------------------------------------
void test_sum_buf()
{
	if (sum_buf("\x01\x02\x03", 3) != 6)
	{
		printf("bad function!\n");
		return;
	}

	const char *buf = "calling a naked asm x86 function";
	printf("the sum of characters in '%s' is %d\n", buf, sum_buf(buf));
}

//--------------------------------------------------------------------------
void hello_world_asm()
{
	const char *msg = "Hello world from x86 assembly language!";
	const char *info = "Hello";

	__asm
	{
		push MB_OK
		push info
		push msg
		
		call GetForegroundWindow
		push eax

		call MessageBoxA
	}
}

//--------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	hello_world_asm();
	test_calculate();
	test_scramble();
	test_func_bind();
	test_sum_buf();
	return 0;
}