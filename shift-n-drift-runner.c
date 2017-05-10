#pragma comment( lib, "kernel32.lib" )

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	char applicationName[256];
	char commandLine[256];
} ConfigValues, *PConfigValues;

int ReadConfig(PConfigValues configValues)
{
	FILE *filePointer = fopen("config.cfg", "r");
	if (filePointer == NULL) {
		perror("Error opening config file");
		return 1;
	}

	char line[128];
	fgets(line, 64, filePointer);
	strcpy(&configValues->applicationName, line);
	configValues->applicationName[strcspn(configValues->applicationName, "\n")] = 0;

	fgets(line, 128, filePointer);
	strcpy(&configValues->commandLine, line);
	configValues->commandLine[strcspn(configValues->commandLine, "\n")] = 0;

	fclose(filePointer);
	return 0;
}

void StartProcess(LPPROCESS_INFORMATION processInfo)
{
	ConfigValues configValues = { 0 };
	if (ReadConfig(&configValues))
	{
		return;
	}

	STARTUPINFOA startupInfo = { 0 };
	startupInfo.cb = sizeof(startupInfo);

	if (!CreateProcessA(&configValues.applicationName,
		                &configValues.commandLine,
		                NULL, NULL, FALSE, 0, NULL, NULL,
		                &startupInfo, processInfo))
	{
		printf("CreateProcess failed (error: %d)  '%s%s'\n\n",
			   GetLastError(), configValues.applicationName, configValues.commandLine);
		return;
	}
	else
	{
		printf("Started application: '%s%s'\n\n",
			   configValues.applicationName, configValues.commandLine);
	}

	return ERROR_SUCCESS;
}

int main(int argc, char **argv)
{
	PROCESS_INFORMATION processInfo = { 0 };
	while (1)
	{
		WORD exitCode;
		GetExitCodeProcess(processInfo.hProcess, &exitCode);
		if (exitCode != STILL_ACTIVE)
		{
			StartProcess(&processInfo);
		}
		Sleep(1000);
	}
	return EXIT_SUCCESS;
}
