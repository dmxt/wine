/*
 *	IMAGEHLP library
 *
 *	Copyright 1998	Patrik Stridvall
 */

#include "winbase.h"
#include "winnt.h"
#include "winerror.h"
#include "windef.h"
#include "heap.h"
#include "debug.h"
#include "imagehlp.h"

/***********************************************************************
 *           Data
 */

static PLOADED_IMAGE IMAGEHLP_pFirstLoadedImage=NULL;
static PLOADED_IMAGE IMAGEHLP_pLastLoadedImage=NULL;

static LOADED_IMAGE IMAGEHLP_EmptyLoadedImage = {
  NULL,       /* ModuleName */
  0xffffffff, /* hFile */
  NULL,       /* MappedAddress */
  NULL,       /* FileHeader */
  NULL,       /* LastRvaSection */
  0,          /* NumberOfSections */
  NULL,       /* Sections */
  1,          /* Characteristics */
  FALSE,      /* fSystemImage */
  FALSE,      /* fDOSImage */
  { &IMAGEHLP_EmptyLoadedImage.Links, &IMAGEHLP_EmptyLoadedImage.Links }, /* Links */
  148,        /* SizeOfImage; */
};

/***********************************************************************
 *           EnumerateLoadedModules32 (IMAGEHLP.4)
 */
BOOL WINAPI EnumerateLoadedModules(
  HANDLE hProcess,
  PENUMLOADED_MODULES_CALLBACK EnumLoadedModulesCallback,
  PVOID UserContext)
{
  FIXME(imagehlp, "(0x%08x, %p, %p): stub\n",
    hProcess, EnumLoadedModulesCallback, UserContext
  );
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           GetTimestampForLoadedLibrary32 (IMAGEHLP.9)
 */
DWORD WINAPI GetTimestampForLoadedLibrary(HMODULE Module)
{
  FIXME(imagehlp, "(0x%08x): stub\n", Module);
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           GetImageConfigInformation32 (IMAGEHLP.7)
 */
BOOL WINAPI GetImageConfigInformation(
  PLOADED_IMAGE LoadedImage,
  PIMAGE_LOAD_CONFIG_DIRECTORY ImageConfigInformation)
{
  FIXME(imagehlp, "(%p, %p): stub\n",
    LoadedImage, ImageConfigInformation
  );
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           GetImageUnusedHeaderBytes32 (IMAGEHLP.8)
 */
DWORD WINAPI GetImageUnusedHeaderBytes(
  PLOADED_IMAGE LoadedImage,
  LPDWORD SizeUnusedHeaderBytes)
{
  FIXME(imagehlp, "(%p, %p): stub\n",
    LoadedImage, SizeUnusedHeaderBytes
  );
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return 0;
}

/***********************************************************************
 *           ImageDirectoryEntryToData32 (IMAGEHLP.11)
 */
PVOID WINAPI ImageDirectoryEntryToData(
  PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, PULONG Size)
{
  FIXME(imagehlp, "(%p, %d, %d, %p): stub\n",
    Base, MappedAsImage, DirectoryEntry, Size
  );
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}

/***********************************************************************
 *           ImageLoad32 (IMAGEHLP.16)
 */
PLOADED_IMAGE WINAPI ImageLoad(LPSTR DllName, LPSTR DllPath)
{
  PLOADED_IMAGE pLoadedImage = 
    HeapAlloc(IMAGEHLP_hHeap, 0, sizeof(LOADED_IMAGE));
  return pLoadedImage;
}

/***********************************************************************
 *           ImageNtHeader32 (IMAGEHLP.17)
 */
PIMAGE_NT_HEADERS WINAPI ImageNtHeader(PVOID Base)
{
  return (PIMAGE_NT_HEADERS)
    ((LPBYTE) Base + ((PIMAGE_DOS_HEADER) Base)->e_lfanew);
}

/***********************************************************************
 *           ImageRvaToSection32 (IMAGEHLP.19)
 */
PIMAGE_SECTION_HEADER WINAPI ImageRvaToSection(
  PIMAGE_NT_HEADERS NtHeaders, PVOID Base, ULONG Rva)
{
  FIXME(imagehlp, "(%p, %p, %ld): stub\n", NtHeaders, Base, Rva);
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}

/***********************************************************************
 *           ImageRvaToVa32 (IMAGEHLP.20)
 */
PVOID WINAPI ImageRvaToVa(
  PIMAGE_NT_HEADERS NtHeaders, PVOID Base, ULONG Rva,
  PIMAGE_SECTION_HEADER *LastRvaSection)
{
  FIXME(imagehlp, "(%p, %p, %ld, %p): stub\n",
    NtHeaders, Base, Rva, LastRvaSection
  );
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return NULL;
}

/***********************************************************************
 *           ImageUnload32 (IMAGEHLP.21)
 */
BOOL WINAPI ImageUnload(PLOADED_IMAGE pLoadedImage)
{
  LIST_ENTRY *pCurrent, *pFind;
  if(!IMAGEHLP_pFirstLoadedImage || !pLoadedImage)
    {
      /* No image loaded or null pointer */
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

  pFind=&pLoadedImage->Links;
  pCurrent=&IMAGEHLP_pFirstLoadedImage->Links;
  while((pCurrent != pFind) && 
    (pCurrent != NULL)) 
      pCurrent = pCurrent->Flink;
  if(!pCurrent)
    {
      /* Not found */
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

  if(pCurrent->Blink)
    pCurrent->Blink->Flink = pCurrent->Flink;
  else
    IMAGEHLP_pFirstLoadedImage = pCurrent->Flink?CONTAINING_RECORD(
      pCurrent->Flink, LOADED_IMAGE, Links):NULL;

  if(pCurrent->Flink)
    pCurrent->Flink->Blink = pCurrent->Blink;
  else
    IMAGEHLP_pLastLoadedImage = pCurrent->Blink?CONTAINING_RECORD(
      pCurrent->Blink, LOADED_IMAGE, Links):NULL;

  return FALSE;
}

/***********************************************************************
 *           MapAndLoad32 (IMAGEHLP.25)
 */
BOOL WINAPI MapAndLoad(
  LPSTR pszImageName, LPSTR pszDllPath, PLOADED_IMAGE pLoadedImage,
  BOOL bDotDll, BOOL bReadOnly)
{
  CHAR szFileName[MAX_PATH];
  HANDLE hFile = (HANDLE) NULL;
  HANDLE hFileMapping = (HANDLE) NULL;
  HMODULE hModule = (HMODULE) NULL;
  PIMAGE_NT_HEADERS pNtHeader = NULL;

  /* PathCombine(&szFileName, pszDllPath, pszImageName); */
  /* PathRenameExtension(&szFileName, bDotDll?:"dll":"exe"); */

  /* FIXME: Check if the file already loaded (use IMAGEHLP_pFirstLoadedImage32) */
  if(!(hFile = CreateFileA(
    szFileName, GENERIC_READ, 1, /* FIXME: FILE_SHARE_READ not defined */
    NULL, OPEN_EXISTING, 0, (HANDLE) NULL)))
    {
      SetLastError(ERROR_FILE_NOT_FOUND);
      goto Error;
    }

  if(!(hFileMapping = CreateFileMappingA(
    hFile, NULL, PAGE_READONLY | SEC_COMMIT, 0, 0, NULL)))
    {
      DWORD dwLastError = GetLastError();
      WARN(imagehlp, "CreateFileMapping: Error = %ld\n", dwLastError);
      SetLastError(dwLastError);
      goto Error;
    }
  CloseHandle(hFile);
  hFile = (HANDLE) NULL;

  if(!(hModule = (HMODULE) MapViewOfFile(
    hFileMapping, FILE_MAP_READ, 0, 0, 0)))
    {
      DWORD dwLastError = GetLastError();
      WARN(imagehlp, "MapViewOfFile: Error = %ld\n", dwLastError);
      SetLastError(dwLastError);
      goto Error;
    }

  CloseHandle(hFileMapping);
  hFileMapping=(HANDLE) NULL;

  pLoadedImage = (PLOADED_IMAGE) HeapAlloc(
    IMAGEHLP_hHeap, 0, sizeof(LOADED_IMAGE)
  );

  pNtHeader = ImageNtHeader((PVOID) hModule);

  pLoadedImage->ModuleName =
    HEAP_strdupA(IMAGEHLP_hHeap, 0, pszDllPath); /* FIXME: Correct? */
  pLoadedImage->hFile = hFile;
  pLoadedImage->MappedAddress = (PUCHAR) hModule;
  pLoadedImage->FileHeader = pNtHeader;
  pLoadedImage->Sections = (PIMAGE_SECTION_HEADER)
    ((LPBYTE) &pNtHeader->OptionalHeader +
      pNtHeader->FileHeader.SizeOfOptionalHeader);
  pLoadedImage->NumberOfSections =
    pNtHeader->FileHeader.NumberOfSections;
  pLoadedImage->SizeOfImage =
    pNtHeader->OptionalHeader.SizeOfImage;
  pLoadedImage->Characteristics =
    pNtHeader->FileHeader.Characteristics;
  pLoadedImage->LastRvaSection = pLoadedImage->Sections;

  pLoadedImage->fSystemImage = FALSE; /* FIXME */
  pLoadedImage->fDOSImage = FALSE;    /* FIXME */

  /* FIXME: Make thread safe */
  pLoadedImage->Links.Flink = NULL;
  pLoadedImage->Links.Blink = &IMAGEHLP_pLastLoadedImage->Links;
  if(IMAGEHLP_pLastLoadedImage)
    IMAGEHLP_pLastLoadedImage->Links.Flink = &pLoadedImage->Links;
  IMAGEHLP_pLastLoadedImage = pLoadedImage;
  if(!IMAGEHLP_pFirstLoadedImage)
    IMAGEHLP_pFirstLoadedImage = pLoadedImage;

  return TRUE;

Error:
  if(hModule)
    UnmapViewOfFile((PVOID) hModule);
  if(hFileMapping)
    CloseHandle(hFileMapping);
  if(hFile)
    CloseHandle(hFile);
  return FALSE;
}

/***********************************************************************
 *           SetImageConfigInformation32 (IMAGEHLP.34)
 */
BOOL WINAPI SetImageConfigInformation(
  PLOADED_IMAGE LoadedImage,
  PIMAGE_LOAD_CONFIG_DIRECTORY ImageConfigInformation)
{
  FIXME(imagehlp, "(%p, %p): stub\n",
    LoadedImage, ImageConfigInformation
  );
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           UnMapAndLoad32 (IMAGEHLP.58)
 */
BOOL WINAPI UnMapAndLoad(PLOADED_IMAGE LoadedImage)
{
  FIXME(imagehlp, "(%p): stub\n", LoadedImage);
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}


