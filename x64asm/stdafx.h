#pragma once

#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>


extern "C" void ShangalPreStub(int a);
extern "C" void sample2();
extern "C" void av_exc();
extern "C" void frmex2();

//extern "C" void ShangalPreStubEnd();
//extern "C" void ShangalPreStub_oCommonStub();
//extern "C" void ShangalPreStub_oUserCallback();
//extern "C" void ShangalPreStub_NbArgs32();
//extern "C" void ShangalCommonStub();

struct ShangalFuncCapture_t;
typedef ULONG_PTR(CALLBACK *ShangalUserCallback_proto)(ShangalFuncCapture_t *c);

//--------------------------------------------------------------------------
#define SHANGAL_SHADOW_CAPTURE_SIZE 9

//--------------------------------------------------------------------------
// Captured registers. The order of the registers follow the stub code
enum ShangalX64RegsCapture_e
{
    SH_X64R_RSP,
    SH_X64R_R11,
    SH_X64R_R10,
    SH_X64R_RAX,
    SH_X64R_R15,
    SH_X64R_R14,
    SH_X64R_R13,
    SH_X64R_R12,
    SH_X64R_RDI,
    SH_X64R_RSI,
    SH_X64R_RBX,
    SH_X64R_RBP,
    SH_X64R_R9,
    SH_X64R_R8,
    SH_X64R_RDX,
    SH_X64R_RCX,

    SH_X64R_COUNT
};

//--------------------------------------------------------------------------
#pragma warning (push)
#pragma warning(disable: 4200)
struct ShangalFuncCapture_t
{
    ShangalUserCallback_proto UserCallback;
    ULONG_PTR NbArgsFlags;
    PVOID RealFunc;
    PVOID Context;
    PVOID Registers[SH_X64R_COUNT];
    PVOID FrameCapture[SHANGAL_SHADOW_CAPTURE_SIZE];
    PVOID ReturnAddress;
    ULONG_PTR Args[0];
};
#pragma warning(pop)
