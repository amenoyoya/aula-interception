call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

@set include=%~dp0..\..\include;%include%
@set include=%~dp0..\..\extlib\LuaJIT-2.1.0-beta3\src;%~dp0..\..\extlib\sol2-3.2.2;%include%
@set lib=%~dp0..\..\dist\lib\x86;%lib%

@set compile=cl.exe /c /nologo /std:c++17 /W3 /WX- /O2 /Oi /Oy- /D _CRT_SECURE_NO_WARNINGS /D WIN32 /D NDEBUG /D _MBCS /Gm- /EHsc /MT /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Gd /analyze- /errorReport:none
