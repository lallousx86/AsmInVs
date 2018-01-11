#include "stdafx.h"
/*
Code copyright Elias Bachaalany <lallousz-x86@yahoo.com>

You may freely use this code. Please give credits where due.

http://lallouslab.net
*/
//--------------------------------------------------------------------------
#pragma pack(push, 1)
struct MyStruct
{
    char b;
    void *buf;
    void (WINAPI *fptr)(int a);
    __int64 sig;
};
#pragma pack(pop)

//--------------------------------------------------------------------------
// External assembler functions
extern "C"
{
    void hello_world_asm();
    void access_extern_data();
    __int64 add_by_ref(int a, int b, __int64 *r);
    void init_struct(MyStruct *st);
    void cause_av_bad();
    void cause_av_good();
    void cause_av_good2();
    int sum_array(int size);
};

static char c_message[] = "Hello world";

//--------------------------------------------------------------------------
extern "C"
{
    // Variables exported to the assembly module
    char c_ext_byte = 1;
    unsigned short c_ext_word = 2;
    long c_ext_dword = 3;
    __int64 c_ext_qword = 4;
    void *c_ext_ptr = (void *)(5);
    void c_ext_my_function();
    int c_ext_my_array[5] = { 100, -100, 50, -50, 1981 };
}

//--------------------------------------------------------------------------
void call_cause_av_bad()
{
    __try
    {
        printf("Calling faulty ASM function...\n");
        cause_av_bad();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        printf("caught exception...\n");
    }
}

//--------------------------------------------------------------------------
void call_cause_av_good()
{
    __try
    {
        printf("Calling faulty ASM function " __FUNCTION__ " that has unwind info...\n");
        cause_av_good();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        printf("caught exception...\n");
    }
    printf("function " __FUNCTION__ " returned!\n");
}

//--------------------------------------------------------------------------
void call_cause_av_good2()
{
    __try
    {
        printf("Calling faulty ASM " __FUNCTION__ " that has unwind info...\n");
        cause_av_good2();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        printf("caught exception...\n");
    }
    printf("function " __FUNCTION__ " returned!\n");
}

//--------------------------------------------------------------------------
__int64 c_add_by_ref(int a, int b, __int64 *r)
{
    *r = a + b;
    return *r;
}

//--------------------------------------------------------------------------
void c_ext_my_function()
{
    printf("Hello from the C function!\n");
}

//--------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
    //access_extern_data();
    //MessageBoxA(0, "Hello", "World", MB_OK);
    //GetForegroundWindow
    //hello_world_asm();

    //__int64 add_result;
    //(void)add_by_ref(1981, 4, &add_result);

    //MyStruct st;
    //init_struct(&st);

    //call_cause_av_bad();
    //call_cause_av_good();
    call_cause_av_good2();

    printf("%d\n", sum_array(_countof(c_ext_my_array)));

    return 0;
}