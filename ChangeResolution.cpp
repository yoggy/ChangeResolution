//
//	ChangeResolution.exe - resolution change tool for windows (CUI tool)
//
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <vector>

typedef std::pair<int, int> Resolution;

bool resolution_cmp(const Resolution& a, const Resolution& b)
{
	if (a.first < b.first) {
		return true;
	}
	else if (a.first > b.first) {
		return false;
	}
	else {
		if (a.second < b.second) {
			return true;
		}
		else if (a.second > b.second) {
			return false;
		}
		else {
			return false;
		}
	}
}

//
//	see also... http://msdn.microsoft.com/ja-jp/library/cc428506.aspx
//
void enum_resolution()
{
	int idx = 0;
	DEVMODE devmode;
	BOOL rv;
	std::vector<Resolution> resolutions;

	while(true) {
		rv = ::EnumDisplaySettings(NULL, idx, &devmode);
		if (rv == FALSE) break;

		Resolution res = Resolution(devmode.dmPelsWidth, devmode.dmPelsHeight);
		if (std::find(resolutions.begin(), resolutions.end(), res) == resolutions.end()) {
			resolutions.push_back(res);
		}
		idx ++;
	}

	std::sort(resolutions.begin(), resolutions.end(), resolution_cmp);

	printf("    resolution\n");
	std::vector<Resolution>::iterator it;
	for (it = resolutions.begin(); it != resolutions.end(); ++it) {
		printf("        %4d, %4d\n", it->first, it->second);
	}
}

//
//	see also... http://msdn.microsoft.com/ja-jp/library/cc428309.aspx
//
bool change_resolution(const int &width, const int &height)
{
	LONG  rv;
	DEVMODE devmode;

	memset(&devmode, 0, sizeof(DEVMODE));
	devmode.dmSize       = sizeof(DEVMODE);
	devmode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
	devmode.dmPelsWidth  = width;
	devmode.dmPelsHeight = height;

	// test
	rv = ::ChangeDisplaySettingsA(&devmode, CDS_TEST);
	if (rv != DISP_CHANGE_SUCCESSFUL) {
		return false;
	}

	// change resolution & write settings to registory
	rv = ::ChangeDisplaySettingsA(&devmode, CDS_UPDATEREGISTRY | CDS_GLOBAL);
	if (rv != DISP_CHANGE_SUCCESSFUL) {
		return false;
	}


	return true;
}

void usage()
{
	printf("usage : ChangeResolution.exe [width] [height]\n");
	printf("\n");
	printf("    example\n");
	printf("        ChangeResolution.exe 1024 768\n");
	printf("\n");

	enum_resolution();
	printf("\n");

	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc != 3) usage();

	int width  = atoi(argv[1]);
	int height = atoi(argv[2]);

	if (width <= 0 || height <= 0) usage();

	printf("change resolution width=%d, height=%d\n", width, height);

	if (change_resolution(width, height) == false) {
		printf("failed...\n");
		return 1;
	}

	printf("success!\n");

	return 0;
}