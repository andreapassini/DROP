#pragma once

namespace Drop {

int Main(int argc, char** argv);

struct DropDLL;
struct DropFileTime;

void LinkDLL(
	DropDLL* DLL
	, char* DLLName, size_t DLLNameSize
);

inline void GetLastTimeFileWrite(
	DropFileTime* OutFileTime
	, char* Filename
);

void CleanDLLPath(
	size_t OutSourceGameCodeDLLFullPathSize, char* OutSourceGameCodeDLLFullPath
	, size_t OutTempGameCodeDLLFullPathSize, char* OutTempGameCodeDLLFullPath
	, size_t InDLLNameSize, char* InDLLName
	, size_t InDLLTempNameSize, char* InDLLTempName
);



}