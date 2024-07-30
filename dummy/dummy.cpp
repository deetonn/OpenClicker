#include <iostream>
#include <windows.h>

using ExternalBuiltinCommandCallbackT
    = int32_t(__stdcall*)(void* /*Console*/, void*/*std::vector<std::string>*/);

__declspec(noinline)
static int32_t dummy_function(void* left, void* right) {
    return 1337;
}

static PVOID left_argument =  NULL;
static PVOID right_argument = NULL;

// This will be executed elsewhere, and the pointers will be initialized 
// using VirtualAlloc.
static void external_init() {
    left_argument = malloc(4);
    right_argument = malloc(4);
}

int main()
{
    external_init();
    return dummy_function(left_argument, right_argument);
}
