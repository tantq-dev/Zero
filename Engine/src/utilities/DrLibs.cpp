// One-time implementation for dr_libs (single-header audio decoders).
// GLOB_RECURSE in Engine/CMakeLists.txt picks this up automatically.

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244 4245 4701 4703 4706)
#endif

#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif
