#include <Windows.h>

namespace HookBypass {
	void SetGamepid(DWORD pid);
	BOOL BypassCSGO_hook();
	BOOL RestoreCSGO_hook();
}