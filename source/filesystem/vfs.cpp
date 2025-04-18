#include "vfs.h"

static VirtualFileSystem* g_vfs = nullptr;

void			   setVirtualFileSystem(VirtualFileSystem* vfs) { g_vfs = vfs; }
VirtualFileSystem* getVirtualFileSystem() { return g_vfs; }