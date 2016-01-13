/*++

Copyright (c) 2012 Minoca Corp. All Rights Reserved

Module Name:

    im.h

Abstract:

    This header contains definitions for manipulating binary images.

Author:

    Evan Green 13-Oct-2012

--*/

//
// ------------------------------------------------------------------- Includes
//

//
// ---------------------------------------------------------------- Definitions
//

//
// Define the allocation tag used by the image library: Imag.
//

#define IM_ALLOCATION_TAG 0x67616D49

//
// Define the constant used to indicate loading an image at its preferred base
// address.
//

#define PREFERRED_IMAGE_BASE MAX_ULONG

//
// Define image load flags.
//

//
// Set this flag to indicate that this is the interpreter, or that generally
// any interpreter directives specified in the program header should be
// ignored.
//

#define IMAGE_LOAD_FLAG_IGNORE_INTERPRETER 0x00000001

//
// Set this flag to indicate that this is the primary executable being loaded.
//

#define IMAGE_LOAD_FLAG_PRIMARY_EXECUTABLE 0x00000002

//
// This flag is set on all images that were loaded as a result of loading the
// primary executable. It is also set on the primary executable itself.
//

#define IMAGE_LOAD_FLAG_PRIMARY_LOAD 0x00000004

//
// Set this flag to indicate the loaded image structure is just a placeholder
// to keep track of image accounting, but doesn't actually contain the guts of
// a loaded image.
//

#define IMAGE_LOAD_FLAG_PLACEHOLDER 0x00000008

//
// Set this flag to skip finding static constructor and destructor functions.
//

#define IMAGE_LOAD_FLAG_NO_STATIC_CONSTRUCTORS 0x00000010

//
// Set this flag to skip processing relocations.
//

#define IMAGE_LOAD_FLAG_NO_RELOCATIONS 0x00000020

//
// Set this flag to only load the images, but not process their dynamic
// sections at all.
//

#define IMAGE_LOAD_FLAG_LOAD_ONLY 0x00000040

//
// Define flags passed into the map image section routine.
//

#define IMAGE_MAP_FLAG_WRITE 0x00000001
#define IMAGE_MAP_FLAG_EXECUTE 0x00000002
#define IMAGE_MAP_FLAG_FIXED 0x00000004

//
// Define the name of the dynamic library path variable.
//

#define IMAGE_DYNAMIC_LIBRARY_PATH_VARIABLE "LD_LIBRARY_PATH"

//
// Define image flags.
//

#define IMAGE_FLAG_IMPORTS_LOADED     0x00000001
#define IMAGE_FLAG_RELOCATED          0x00000002
#define IMAGE_FLAG_INITIALIZED        0x00000004
#define IMAGE_FLAG_RELOCATABLE        0x00000008
#define IMAGE_FLAG_STATIC_TLS         0x00000010
#define IMAGE_FLAG_GNU_HASH           0x00000020

//
// ------------------------------------------------------ Data Type Definitions
//

typedef enum _IMAGE_FORMAT {
    ImageInvalidFormat,
    ImageUnknownFormat,
    ImagePe32,
    ImageElf32,
    MaxImageFormats
} IMAGE_FORMAT, *PIMAGE_FORMAT;

typedef enum _IMAGE_MACHINE_TYPE {
    ImageMachineTypeInvalid,
    ImageMachineTypeUnknown,
    ImageMachineTypeX86,
    ImageMachineTypeArm32
} IMAGE_MACHINE_TYPE, *PIMAGE_MACHINE_TYPE;

typedef enum _IMAGE_SEGMENT_TYPE {
    ImageSegmentInvalid = 0,
    ImageSegmentFileSection,
    ImageSegmentZeroedMemory
} IMAGE_SEGMENT_TYPE, *PIMAGE_SEGMENT_TYPE;

typedef
VOID
(*PIMAGE_STATIC_FUNCTION) (
    VOID
    );

/*++

Routine Description:

    This routine defines the prototype for image static constructors and
    destructors such as _init, _fini, and those in .preinit_array, .init_array,
    and .fini_array.

Arguments:

    None.

Return Value:

    None.

--*/

/*++

Structure Description:

    This structure stores information about an executable image.

Members:

    Format - Stores the basic file format of the executable image.

    Machine - Stores the machine type this image was built for.

    ImageBase - Stores the default image base of the image.

    EntryPoint - Stores the default (unrelocated) entry point of the image.

--*/

typedef struct _IMAGE_INFORMATION {
    IMAGE_FORMAT Format;
    IMAGE_MACHINE_TYPE Machine;
    ULONGLONG ImageBase;
    ULONGLONG EntryPoint;
} IMAGE_INFORMATION, *PIMAGE_INFORMATION;

/*++

Structure Description:

    This structure stores information about a file for the image library.

Members:

    Handle - Stores an open handle to the file.

    Size - Stores the size of the file in bytes.

    ModificationDate - Stores the modification date of the file in seconds
        since 2001.

--*/

typedef struct _IMAGE_FILE_INFORMATION {
    HANDLE Handle;
    ULONGLONG Size;
    ULONGLONG ModificationDate;
} IMAGE_FILE_INFORMATION, *PIMAGE_FILE_INFORMATION;

/*++

Structure Description:

    This structure stores information about a segment or region of an
    executable image loaded into memory.

Members:

    Type - Stores the type of segment this structure represents.

    VirtualAddress - Stores the virtual address of the image segment.

    Size - Stores the size, in bytes, of the image segment.

    FileSize - Stores the size, in bytes, of the segment mapped to the file.

    MemorySize - Stores the size, in bytes, of the segment in memory. This must
        be at least as big as the file size, and bytes after the file size
        will be initialized to 0.

    Flags - Stores the bitfield of attributes about the mapping. See
        IMAGE_MAP_FLAG_* definitions.

    MappingStart - Stores an optional pointer not used by the image library
        indicating the location where the memory mapping of the segment began.

--*/

typedef struct _IMAGE_SEGMENT {
    IMAGE_SEGMENT_TYPE Type;
    PVOID VirtualAddress;
    UINTN FileSize;
    UINTN MemorySize;
    ULONG Flags;
    PVOID MappingStart;
} IMAGE_SEGMENT, *PIMAGE_SEGMENT;

/*++

Structure Description:

    This structure stores information about static constructors and destructors
    in the image. All pointers are final virtual addresses. The order these
    are called in is .preinit_array, _init, .init_array, .fini_array (in
    reverse order), and _fini.

Members:

    PreinitArray - Stores an optional pointer to the array of pre-init
        functions in a dynamic library.

    PreinitArraySize - Stores the size of the preinit array in bytes.

    InitArray - Stores an optional pointer to the array of static constructor
        functions in a dynamic library.

    InitArraySize - Stores the size of the init array in bytes.

    FiniArray - Stores an optional pointer to the array of static destructor
        functions in a dynamic library.

    FiniArraySize - Stores the size of the fini array in bytes.

    InitFunction - Stores an optional pointer to the _init function in a
        dynamic library.

    FiniFunction - Stores an optional pointer to the _fini function in a
        dynamic library.

--*/

typedef struct _IMAGE_STATIC_FUNCTIONS {
    PIMAGE_STATIC_FUNCTION *PreinitArray;
    UINTN PreinitArraySize;
    PIMAGE_STATIC_FUNCTION *InitArray;
    UINTN InitArraySize;
    PIMAGE_STATIC_FUNCTION *FiniArray;
    UINTN FiniArraySize;
    PIMAGE_STATIC_FUNCTION InitFunction;
    PIMAGE_STATIC_FUNCTION FiniFunction;
} IMAGE_STATIC_FUNCTIONS, *PIMAGE_STATIC_FUNCTIONS;

/*++

Structure Description:

    This structure stores information about a loaded executable image.

Members:

    ListEntry - Stores pointers to the next and previous images. This is not
        used by the Image Library, and can be used by the subsystem managing
        the image library.

    BinaryName - Stores a pointer to a buffer containing the name of the binary
        image.

    ModuleNumber - Stores the module identifier. This is not used by the image
        library, but can be assigned by the consumer of the image library.

    TlsOffset - Stores the offset from the thread pointer to the start of the
        static TLS block for this module. This only applies to modules using
        the static TLS regime. This will be initialized to -1 if the module
        has no TLS offset or is loaded dynamically.

    Format - Stores an integer indicating the binary image format.

    Machine - Stores the machine type for the image.

    File - Stores information about the file itself, including potentially an
        open handle to it during the load process.

    Size - Stores the size of the image as expanded in memory, in bytes.

    DeclaredBase - Stores the base address as declared in the file. This may
        not actually be the image's lowest loaded VA. This is always 0 for ELF
        files.

    PreferredLowestAddress - Stores the image's default lowest virtual address.

    LoadedLowestAddress - Stores the image's actual lowest virtual address.

    ImageContext - Stores a pointer to context specific to the image backend.

    LoadedImageBuffer - Stores a pointer to the image's in-memory layout. In
        a live system, this is probably the same as the actual loaded VA of the
        image. In offline situations, this may be a different buffer.
        Relocations and other modifications to the image are made through this
        pointer.

    SystemContext - Stores a pointer of context that gets passed to system
        backend functions.

    AllocatorHandle - Stores the handle associated with the overall allocation
        of virtual address space.

    SegmentCount - Stores the number of segments in the loaded image.

    Segments - Stores a pointer to the loaded image segments.

    EntryPoint - Stores the entry point of the image. This pointer is
        absolute (it has already been rebased).

    ReferenceCount - Stores the reference count on this image.

    ExportSymbolTable - Stores a pointer to the export symbol table.

    ExportStringTable - Stores a pointer to the export string table.

    ExportStringTable - Stores the size of the export string table in bytes.

    ExportHashTable - Stores a pointer to the export hash table, not used in
        all image formats.

    ImportDepth - Stores the import depth of the image (the number of images
        between the image and some image that was actually requested to be
        loaded). An image's imports, unless already loaded, have an import
        depth of one greater than the image itself.

    ImportCount - Stores the number of import images this image requires.

    Imports - Stores a pointer to an array of loaded images that this image
        imports from.

    TlsImage - Stores a pointer to the thread-local storage initialization
        data.

    TlsImageSize - Stores the size of the thread-local storage initialization
        data, in bytes.

    TlsSize - Stores the size of the thread-local storage region, in bytes.
        This may be bigger than the TLS image size if there is uninitialized
        data.

    TlsAlignment - Stores the alignment requirement of the TLS section.

    DebuggerModule - Stores an optional pointer to the debugger's module
        information if this module is loaded in the kernel debugger.

    SystemExtension - Stores a pointer to the additional information the
        system stores attached to this image.

    Flags - Stores internal image flags. See IMAGE_FLAG_* definitions.

    LoadFlags - Stores the flags passed in when the image load was requested.

    StaticFunctions - Stores an optional pointer to an array of static
        functions.

    VisitMarker - Stores space for the address search routine to mark nodes
        as visited so as to avoid cycles.

--*/

typedef struct _LOADED_IMAGE {
    LIST_ENTRY ListEntry;
    PSTR BinaryName;
    UINTN ModuleNumber;
    UINTN TlsOffset;
    IMAGE_FORMAT Format;
    IMAGE_MACHINE_TYPE Machine;
    IMAGE_FILE_INFORMATION File;
    UINTN Size;
    PVOID DeclaredBase;
    PVOID PreferredLowestAddress;
    PVOID LoadedLowestAddress;
    PVOID LoadedImageBuffer;
    PVOID ImageContext;
    PVOID SystemContext;
    HANDLE AllocatorHandle;
    ULONG SegmentCount;
    PIMAGE_SEGMENT Segments;
    PVOID EntryPoint;
    ULONG ReferenceCount;
    PVOID ExportSymbolTable;
    PVOID ExportStringTable;
    ULONG ExportStringTableSize;
    PVOID ExportHashTable;
    ULONG ImportDepth;
    ULONG ImportCount;
    PVOID *Imports;
    PVOID TlsImage;
    UINTN TlsImageSize;
    UINTN TlsSize;
    UINTN TlsAlignment;
    PVOID DebuggerModule;
    PVOID SystemExtension;
    ULONG Flags;
    ULONG LoadFlags;
    PIMAGE_STATIC_FUNCTIONS StaticFunctions;
    UCHAR VisitMarker;
} LOADED_IMAGE, *PLOADED_IMAGE;

//
// Outside support routines needed by the image library.
//

typedef
PVOID
(*PIM_ALLOCATE_MEMORY) (
    ULONG Size,
    ULONG Tag
    );

/*++

Routine Description:

    This routine allocates memory for the image library.

Arguments:

    Size - Supplies the number of bytes required for the memory allocation.

    Tag - Supplies a 32-bit ASCII identifier used to tag the memroy allocation.

Return Value:

    Returns a pointer to the memory allocation on success.

    NULL on failure.

--*/

typedef
VOID
(*PIM_FREE_MEMORY) (
    PVOID Allocation
    );

/*++

Routine Description:

    This routine frees memory allocated by the image library.

Arguments:

    Allocation - Supplies a pointer the allocation to free.

Return Value:

    None.

--*/

typedef
KSTATUS
(*PIM_OPEN_FILE) (
    PVOID SystemContext,
    PSTR BinaryName,
    PIMAGE_FILE_INFORMATION File
    );

/*++

Routine Description:

    This routine opens a file.

Arguments:

    SystemContext - Supplies the context pointer passed to the load executable
        function.

    BinaryName - Supplies the name of the executable image to open.

    File - Supplies a pointer where the information for the file including its
        open handle will be returned.

Return Value:

    Status code.

--*/

typedef
VOID
(*PIM_CLOSE_FILE) (
    PIMAGE_FILE_INFORMATION File
    );

/*++

Routine Description:

    This routine closes an open file, invalidating any memory mappings to it.

Arguments:

    File - Supplies a pointer to the file information.

Return Value:

    None.

--*/

typedef
KSTATUS
(*PIM_LOAD_FILE) (
    PIMAGE_FILE_INFORMATION File,
    PVOID *FileBuffer
    );

/*++

Routine Description:

    This routine loads a file into memory so the image library can read it.

Arguments:

    File - Supplies a pointer to the file information.

    FileBuffer - Supplies a pointer where a pointer to the file buffer will be
        returned on success.

Return Value:

    Status code.

--*/

typedef
VOID
(*PIM_UNLOAD_FILE) (
    PIMAGE_FILE_INFORMATION File,
    PVOID Buffer
    );

/*++

Routine Description:

    This routine unloads a file and frees the buffer associated with a load
    image call.

Arguments:

    File - Supplies a pointer to the file information.

    Buffer - Supplies the buffer returned by the load file function.

Return Value:

    None.

--*/

typedef
KSTATUS
(*PIM_ALLOCATE_ADDRESS_SPACE) (
    PVOID SystemContext,
    PIMAGE_FILE_INFORMATION File,
    ULONG Size,
    HANDLE *Handle,
    PVOID *Address,
    PVOID *AccessibleAddress
    );

/*++

Routine Description:

    This routine allocates a section of virtual address space that an image
    can be mapped in to.

Arguments:

    SystemContext - Supplies the context pointer passed to the load executable
        function.

    File - Supplies a pointer to the image file information.

    Size - Supplies the required size of the allocation, in bytes.

    Handle - Supplies a pointer where the handle representing this allocation
        will be returned on success.

    Address - Supplies a pointer that on input contains the preferred virtual
        address of the image load. On output, contains the allocated virtual
        address range. This is the VA allocated, but it may not actually be
        accessible at this time.

    AccessibleAddress - Supplies a pointer where a pointer will be returned
        that the caller can reach through to access the in-memory image. In
        online image loads this is probably the same as the returned address,
        though this cannot be assumed.

Return Value:

    Status code.

--*/

typedef
VOID
(*PIM_FREE_ADDRESS_SPACE) (
    HANDLE Handle,
    PVOID Address,
    UINTN Size
    );

/*++

Routine Description:

    This routine frees a section of virtual address space that was previously
    allocated.

Arguments:

    Handle - Supplies the handle returned during the allocate call.

    Address - Supplies the virtual address originally returned by the allocate
        routine.

    Size - Supplies the size in bytes of the originally allocated region.

Return Value:

    None.

--*/

typedef
KSTATUS
(*PIM_MAP_IMAGE_SEGMENT) (
    HANDLE AddressSpaceHandle,
    PVOID AddressSpaceAllocation,
    PIMAGE_FILE_INFORMATION File,
    ULONGLONG FileOffset,
    PIMAGE_SEGMENT Segment,
    PIMAGE_SEGMENT PreviousSegment
    );

/*++

Routine Description:

    This routine maps a section of the image to the given virtual address.

Arguments:

    AddressSpaceHandle - Supplies the handle used to claim the overall region
        of address space.

    AddressSpaceAllocation - Supplies the original lowest virtual address for
        this image.

    File - Supplies an optional pointer to the file being mapped. If this
        parameter is NULL, then a zeroed memory section is being mapped.

    FileOffset - Supplies the offset from the beginning of the file to the
        beginning of the mapping, in bytes.

    Segment - Supplies a pointer to the segment information to map. On output,
        the virtual address will contain the actual mapped address, and the
        mapping handle may be set.

    PreviousSegment - Supplies an optional pointer to the previous segment
        that was mapped, so this routine can handle overlap appropriately. This
        routine can assume that segments are always mapped in increasing order.

Return Value:

    Status code.

--*/

typedef
VOID
(*PIM_UNMAP_IMAGE_SEGMENT) (
    HANDLE AddressSpaceHandle,
    PIMAGE_SEGMENT Segment
    );

/*++

Routine Description:

    This routine maps unmaps an image segment.

Arguments:

    AddressSpaceHandle - Supplies the handle used to claim the overall region
        of address space.

    Segment - Supplies a pointer to the segment information to unmap.

Return Value:

    None.

--*/

typedef
KSTATUS
(*PIM_NOTIFY_IMAGE_LOAD) (
    PLOADED_IMAGE Image
    );

/*++

Routine Description:

    This routine notifies the primary consumer of the image library that an
    image has been loaded.

Arguments:

    Image - Supplies the image that has just been loaded. This image should
        be subsequently returned to the image library upon requests for loaded
        images with the given name.

Return Value:

    Status code. Failing status codes veto the image load.

--*/

typedef
VOID
(*PIM_NOTIFY_IMAGE_UNLOAD) (
    PLOADED_IMAGE Image
    );

/*++

Routine Description:

    This routine notifies the primary consumer of the image library that an
    image is about to be unloaded from memory. Once this routine returns, the
    image should not be referenced again as it will be freed.

Arguments:

    Image - Supplies the image that is about to be unloaded.

Return Value:

    None.

--*/

typedef
VOID
(*PIM_INVALIDATE_INSTRUCTION_CACHE_REGION) (
    PVOID Address,
    ULONG Size
    );

/*++

Routine Description:

    This routine invalidates an instruction cache region after code has been
    modified.

Arguments:

    Address - Supplies the virtual address of the revion to invalidate.

    Size - Supplies the number of bytes to invalidate.

Return Value:

    None.

--*/

typedef
PSTR
(*PIM_GET_ENVIRONMENT_VARIABLE) (
    PSTR Variable
    );

/*++

Routine Description:

    This routine gets an environment variable value for the image library.

Arguments:

    Variable - Supplies a pointer to a null terminated string containing the
        name of the variable to get.

Return Value:

    Returns a pointer to the value of the environment variable. The image
    library will not free or modify this value.

    NULL if the given environment variable is not set.

--*/

typedef
KSTATUS
(*PIM_FINALIZE_SEGMENTS) (
    HANDLE AddressSpaceHandle,
    PIMAGE_SEGMENT Segments,
    UINTN SegmentCount
    );

/*++

Routine Description:

    This routine applies the final memory protection attributes to the given
    segments. Read and execute bits can be applied at the time of mapping, but
    write protection may be applied here.

Arguments:

    AddressSpaceHandle - Supplies the handle used to claim the overall region
        of address space.

    Segments - Supplies the final array of segments.

    SegmentCount - Supplies the number of segments.

Return Value:

    Status code.

--*/

/*++

Structure Description:

    This structure stores pointers to all the functions the image library
    requires as imports.

Members:

    AllocateMemory - Stores a pointer to a function used by the image
        library to allocate memory.

    FreeMemory - Stores a pointer to a function used by the image library to
        free memory.

    OpenFile - Stores a pointer to a function used by the image library to
        open a handle to a file.

    CloseFile - Stores a pointer to a function used by the image library to
        close a handle to a file.

    LoadFile - Stores a pointer to a function used by the image library to
        load a file into memory.

    UnloadFile - Stores a pointer to a function used by the image library to
        unload a file buffer from memory.

    AllocateAddressSpace - Stores a pointer to a function used by the image
        library to allocate a section of virtual address space.

    FreeAddressSpace - Stores a pointer to a function used by the image
        library to free a section of virtual address space.

    MapImageSegment - Store a pointer to a function used by the image
        library to map a segment of a file into virtual memory.

    UnmapImageSegment - Stores a pointer to a function used by the image
        library to unmap segments from virtual memory.

    NotifyImageLoad - Stores a pointer to a function used by the image library
        to notify consumers that an image has been loaded.

    NotifyImageUnload - Stores a pointer to a function used by the image library
        to notify consumers that an image is about to be unloaded.

    InvalidateInstructionCacheRegion - Stores a pointer to a function that is
        called after a code region is modified.

    GetEnvironmentVariable - Stores an optional pointer to a function used to
        query the environment.

    FinalizeSegments - Stores an optional pointer to a function used to set the
        final permissions on all segments.

--*/

typedef struct _IM_IMPORT_TABLE {
    PIM_ALLOCATE_MEMORY AllocateMemory;
    PIM_FREE_MEMORY FreeMemory;
    PIM_OPEN_FILE OpenFile;
    PIM_CLOSE_FILE CloseFile;
    PIM_LOAD_FILE LoadFile;
    PIM_UNLOAD_FILE UnloadFile;
    PIM_ALLOCATE_ADDRESS_SPACE AllocateAddressSpace;
    PIM_FREE_ADDRESS_SPACE FreeAddressSpace;
    PIM_MAP_IMAGE_SEGMENT MapImageSegment;
    PIM_UNMAP_IMAGE_SEGMENT UnmapImageSegment;
    PIM_NOTIFY_IMAGE_LOAD NotifyImageLoad;
    PIM_NOTIFY_IMAGE_UNLOAD NotifyImageUnload;
    PIM_INVALIDATE_INSTRUCTION_CACHE_REGION InvalidateInstructionCacheRegion;
    PIM_GET_ENVIRONMENT_VARIABLE GetEnvironmentVariable;
    PIM_FINALIZE_SEGMENTS FinalizeSegments;
} IM_IMPORT_TABLE, *PIM_IMPORT_TABLE;

//
// -------------------------------------------------------------------- Globals
//

KSTATUS
ImInitialize (
    PIM_IMPORT_TABLE ImportTable
    );

/*++

Routine Description:

    This routine initializes the image library. It must be called before any
    other image library routines are called.

Arguments:

    ImportTable - Supplies a pointer to a table of functions that will be used
        by the image library to provide basic memory allocation and loading
        support. This memory must stick around, the given pointer is cached.

Return Value:

    STATUS_SUCCESS on success.

    STATUS_TOO_LATE if the image library has already been initialized.

    STATUS_INVALID_PARAMETER if one of the required functions is not
        implemented.

--*/

KSTATUS
ImGetExecutableFormat (
    PSTR BinaryName,
    PVOID SystemContext,
    PIMAGE_FILE_INFORMATION Information,
    PIMAGE_FORMAT Format
    );

/*++

Routine Description:

    This routine determines the executable format of a given image path.

Arguments:

    BinaryName - Supplies the name of the binary executable image to examine.

    SystemContext - Supplies the context pointer passed to the load executable
        function.

    Information - Supplies an optional pointer where the file handle and other
        information will be returned on success.

    Format - Supplies a pointer where the format will be returned on success.

Return Value:

    Status code.

--*/

KSTATUS
ImLoadExecutable (
    PLIST_ENTRY ListHead,
    PSTR BinaryName,
    PIMAGE_FILE_INFORMATION BinaryFile,
    PVOID SystemContext,
    ULONG Flags,
    ULONG ImportDepth,
    PLOADED_IMAGE *LoadedImage,
    PLOADED_IMAGE *Interpreter
    );

/*++

Routine Description:

    This routine loads an executable image into memory.

Arguments:

    ListHead - Supplies a pointer to the head of the list of loaded images.

    BinaryName - Supplies the name of the binary executable image to load. If
        this is NULL, then a pointer to the first (primary) image loaded, with
        a reference added.

    BinaryFile - Supplies an optional handle to the file information. The
        handle should be positioned to the beginning of the file. Supply NULL
        if the caller does not already have an open handle to the binary. On
        success, the image library takes ownership of the handle.

    SystemContext - Supplies an opaque token that will be passed to the
        support functions called by the image support library.

    Flags - Supplies a bitfield of flags governing the load. See
        IMAGE_LOAD_FLAG_* flags.

    ImportDepth - Supplies the import depth of the image. Supply 0 here.

    LoadedImage - Supplies an optional pointer where a pointer to the loaded
        image structure will be returned on success.

    Interpreter - Supplies an optional pointer where a pointer to the loaded
        interpreter structure will be returned on success.

Return Value:

    Status code.

--*/

KSTATUS
ImAddImage (
    PSTR BinaryName,
    PVOID Buffer,
    PLOADED_IMAGE *LoadedImage
    );

/*++

Routine Description:

    This routine adds the accounting structures for an image that has already
    been loaded into memory.

Arguments:

    ListHead - Supplies a pointer to the head of the list of loaded images.

    BinaryName - Supplies an optional pointer to the name of the image to use.
        If NULL, then the shared object name of the image will be extracted.

    Buffer - Supplies the base address of the loaded image.

    LoadedImage - Supplies an optional pointer where a pointer to the loaded
        image structure will be returned on success.

Return Value:

    Status code.

--*/

KSTATUS
ImLoadImports (
    PLIST_ENTRY ListHead
    );

/*++

Routine Description:

    This routine loads all import libraries for a given image list.

Arguments:

    ListHead - Supplies a pointer to the head of the list of loaded images to
        load import libraries for.

Return Value:

    Status code.

--*/

KSTATUS
ImRelocateImages (
    PLIST_ENTRY ListHead
    );

/*++

Routine Description:

    This routine relocates all images that have not yet been relocated on the
    given list.

Arguments:

    ListHead - Supplies a pointer to the head of the list of loaded images to
        apply relocations for.

Return Value:

    Status code.

--*/

VOID
ImImageAddReference (
    PLOADED_IMAGE Image
    );

/*++

Routine Description:

    This routine increments the reference count on an image.

Arguments:

    Image - Supplies a pointer to the loaded image.

Return Value:

    None.

--*/

VOID
ImImageReleaseReference (
    PLOADED_IMAGE Image
    );

/*++

Routine Description:

    This routine releases a reference on a loaded executable image from memory.
    If this is the last reference, the image will be unloaded.

Arguments:

    Image - Supplies a pointer to the loaded image.

Return Value:

    None.

--*/

KSTATUS
ImGetImageInformation (
    PVOID File,
    UINTN FileSize,
    PIMAGE_INFORMATION Information
    );

/*++

Routine Description:

    This routine gets various pieces of information about an image. This is the
    generic form that can get information from any supported image type.

Arguments:

    File - Supplies a pointer to the memory mapped file.

    FileSize - Supplies the size of the file.

    Information - Supplies a pointer to the information structure that will be
        filled out by this function. It is assumed the memory pointed to here
        is valid.

Return Value:

    STATUS_SUCCESS on success.

    STATUS_UNKNOWN_IMAGE_FORMAT if the image is unknown or corrupt.

--*/

BOOL
ImGetImageSection (
    PVOID File,
    UINTN FileSize,
    PSTR SectionName,
    PVOID *Section,
    PULONGLONG VirtualAddress,
    PULONG SectionSizeInFile,
    PULONG SectionSizeInMemory
    );

/*++

Routine Description:

    This routine gets a pointer to the given section in a PE image given a
    memory mapped file.

Arguments:

    File - Supplies a pointer to the image file mapped into memory.

    FileSize - Supplies the size of the memory mapped file, in bytes.

    SectionName - Supplies the name of the desired section.

    Section - Supplies a pointer where the pointer to the section will be
        returned.

    VirtualAddress - Supplies a pointer where the virtual address of the section
        will be returned, if applicable.

    SectionSizeInFile - Supplies a pointer where the size of the section as it
        appears in the file will be returned.

    SectionSizeInMemory - Supplies a pointer where the size of the section as it
        appears after being loaded in memory will be returned.

Return Value:

    TRUE on success.

    FALSE otherwise.

--*/

IMAGE_FORMAT
ImGetImageFormat (
    PVOID FileBuffer,
    UINTN FileBufferSize
    );

/*++

Routine Description:

    This routine determines the file format for an image mapped in memory.

Arguments:

    FileBuffer - Supplies a pointer to the memory mapped file.

    FileBufferSize - Supplies the size of the file.

Return Value:

    Returns the file format of the image.

--*/

KSTATUS
ImGetSymbolAddress (
    PLIST_ENTRY ListHead,
    PLOADED_IMAGE Image,
    PSTR SymbolName,
    BOOL Recursive,
    PVOID *Address
    );

/*++

Routine Description:

    This routine attempts to find an exported symbol with the given name in the
    given binary. This routine also looks through the image imports if the
    recursive flag is specified.

Arguments:

    ListHead - Supplies the head of the list of loaded images.

    Image - Supplies a pointer to the image to query.

    SymbolName - Supplies a pointer to the string containing the name of the
        symbol to search for.

    Recursive - Supplies a boolean indicating if the routine should recurse
        into imports or just query this binary.

    Address - Supplies a pointer where the address of the symbol will be
        returned on success, or NULL will be returned on failure.

Return Value:

    Status code.

--*/
