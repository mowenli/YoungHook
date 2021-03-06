
#import <objc/runtime.h>
#import "CamoCall.h"

//#define YoungHook_AutoInit
//#define YoungHook_CydiaSubstrate
//#define YoungHook_FishHook
//#define YoungHook_MultiProcess

//
#ifdef __cplusplus
extern "C"
{
#endif

#if defined(YoungHook_CydiaSubstrate) || defined(YoungHook_FishHook)
bool YHHookFunction(const char *lib, const char *func, void *hook, void **old);
#endif
bool YHHookMessage(const char * cls, bool meta, const char *name, IMP hook, IMP *old);	// Name means ObjC message, use '_' for ':', '__' for '_'

#ifdef YoungHook_MultiProcess
#if defined(YoungHook_CydiaSubstrate) || defined(YoungHook_FishHook)
bool YHHookFunctionForProcess(const char *proc, const char *lib, const char *func, void *hook, void **old);
#endif
bool YHHookMessageForProcess(const char *proc, const char * cls, bool meta, const char *name, IMP hook, IMP *old);
#endif

#ifdef __cplusplus
}
#endif

//
#define __YHHookFunction(NOUSE, ...)						YHHookFunction(__VA_ARGS__)
#define __YHHookMessage(NOUSE, ...)							YHHookMessage(__VA_ARGS__)

#define __HOOK_FUNCTION(MOD, HKFN, PROC, RET, LIB, FUNC, ...) RET $##FUNC(__VA_ARGS__);\
															RET (*_##FUNC)(__VA_ARGS__);\
															__attribute__((MOD)) void _Init_##FUNC()\
															{\
																HKFN(#PROC, #LIB, #FUNC, (void *)$##FUNC, (void **)&_##FUNC);\
															}\
															RET $##FUNC(__VA_ARGS__)

#define __HOOK_MESSAGE(MOD, HKFN, PROC, RET, CLS, MSG, META, ...)\
															RET $##CLS##_##MSG(id self, SEL sel, ##__VA_ARGS__);\
															RET (*_##CLS##_##MSG)(id self, SEL sel, ##__VA_ARGS__);\
															__attribute__((MOD)) void _Init_##CLS##_##MSG()\
															{\
																HKFN(#PROC, #CLS, META, #MSG, (IMP)$##CLS##_##MSG, (IMP *)&_##CLS##_##MSG);\
															}\
															RET $##CLS##_##MSG(id self, SEL sel, ##__VA_ARGS__)

// Manual hook, call _Init_*** to enable the hook
#define _HOOK_FUNCTION(RET, LIB, FUN, ...)					__HOOK_FUNCTION(always_inline, __YHHookFunction, , RET, LIB, FUN, ##__VA_ARGS__)
#define _HOOK_MESSAGE(RET, CLS, MSG, ...)					__HOOK_MESSAGE(always_inline, __YHHookMessage, , RET, CLS, MSG, false, ##__VA_ARGS__)
#define _HOOK_CLASS(RET, CLS, MSG, ...)						__HOOK_MESSAGE(always_inline, __YHHookMessage, , RET, CLS, MSG, true, ##__VA_ARGS__)

#ifdef YoungHook_AutoInit
// Automatic hook
#define HOOK_FUNCTION(RET, LIB, FUN, ...)					__HOOK_FUNCTION(constructor, __YHHookFunction, , RET, LIB, FUN, ##__VA_ARGS__)
#define HOOK_MESSAGE(RET, CLS, MSG, ...)					__HOOK_MESSAGE(constructor, __YHHookMessage, , RET, CLS, MSG, false, ##__VA_ARGS__)
#define HOOK_CLASS(RET, CLS, MSG, ...)						__HOOK_MESSAGE(constructor, __YHHookMessage, , RET, CLS, MSG, true, ##__VA_ARGS__)

// Automatic hook for special process name
// Use | separator for multiple process name
#ifdef YoungHook_MultiProcess
#define HOOK_FUNCTION_FOR_PROCESS(PROC, RET, LIB, FUN, ...)	__HOOK_FUNCTION(constructor, YHHookFunctionForProcess, PROC, RET, LIB, FUN, ##__VA_ARGS__)
#define HOOK_MESSAGE_FOR_PROCESS(PROC, RET, CLS, MSG, ...)	__HOOK_MESSAGE(constructor, YHHookMessageForProcess, PROC, RET, CLS, MSG, false, ##__VA_ARGS__)
#define HOOK_CLASS_FOR_PROCESS(PROC, RET, CLS, MSG, ...)	__HOOK_MESSAGE(constructor, YHHookMessageForProcess, PROC, RET, CLS, MSG, true, ##__VA_ARGS__)
#endif
#endif
