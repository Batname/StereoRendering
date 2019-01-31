#include "StereoRendering_precompiled.h"

#include "StereoRenderingUtils.h"

namespace StereoRendering
{
	DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
    ErrorCode(hr),
    FunctionName(functionName),
    Filename(filename),
    LineNumber(lineNumber)
{
}
}