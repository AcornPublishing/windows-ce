#include <windows.h>

#define METHOD_BUFFERED                 0
#define FILE_ANY_ACCESS                 0
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define IOCTL_SAMPLE_FIRST	CTL_CODE( 0, 0x800, 0, 0 )

int WINAPI
main (
    HINSTANCE hInstance, 
    HINSTANCE hInstPrev, 
    LPWSTR pCmdLine,
    int nCmdShow)
{
    HANDLE handle;
	DWORD ret;
	unsigned char Buffer[10]="AAAAAAAAAA";

	handle = CreateFile(
		L"SAM1:", 
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	WriteFile( handle, Buffer, 10, &ret, NULL );

	DeviceIoControl( handle, IOCTL_SAMPLE_FIRST, NULL, 0, NULL, 0, &ret, NULL );

	CloseHandle( handle );

    return(0);
}
