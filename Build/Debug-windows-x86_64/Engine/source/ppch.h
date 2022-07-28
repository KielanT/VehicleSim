#pragma once

// Precompiled header file

#ifdef P_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#include "Windows.h"

#include <cmath>
#include <cctype>
#include <atlbase.h> // C-string to unicode conversion function CA2CT
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <memory>
#include <algorithm>
#include <string>
#include <map>


#include <d3dcompiler.h>
#include <d3d11.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include "Misc/ProjectDefines.h"
#include "Utility/ErrorLogger.h"
#include "Misc/Macros.h"
