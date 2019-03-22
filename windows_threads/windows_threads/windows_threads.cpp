// This program shows off the basics of windows threads in C++
// By: Nick from CoffeeBeforeArch

#include <iostream>
#include <Windows.h>
#include <strsafe.h>

#define NUM_THREADS 10
#define BUF_SIZE 255

// Argument we will be passing to out spawned threads
struct Package {
	int tid;
	int number;
};

void display_message(HANDLE hScreen, int tid, int number, const char *message) {
	TCHAR message_buffer[BUF_SIZE];
	size_t cchStringSize;
	DWORD dwChars;

	// Print using thread-safe functions
	StringCchPrintf(message_buffer, BUF_SIZE, TEXT("Printing number %d from thread %d. %s\n"), tid, number, message);
	StringCchLength(message_buffer, BUF_SIZE, &cchStringSize);
	WriteConsole(hScreen, message_buffer, cchStringSize, &dwChars, NULL);

	Sleep(1000);
}

// DWORD is a Windows-specific type (Unsigned 32-bit)
// WINAPI is a macro that specifies the use of the WIndows calling
// convention
// LPVOID is just a void pointer
DWORD WINAPI boring_thread_function(LPVOID lpParam) {
	// Cast our void pointer to our defined struct
	Package local_package = *(Package*)lpParam;

	// Get Handle to the screen
	HANDLE hStdout = NULL;
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// Call our function with thread-safe print
	display_message(hStdout, local_package.tid, local_package.number, "Hello, there!");

	return 0;
}

int main() {
	HANDLE array_of_handles[NUM_THREADS] = { 0 };
	Package packages[NUM_THREADS];

	// Create threads
	for (int i = 0; i < NUM_THREADS; i++) {
		packages[i].tid = i;
		packages[i].number = i * i;
		array_of_handles[i] = CreateThread(NULL, 0, boring_thread_function, &packages[i], 0, NULL);
	}

	WaitForMultipleObjects(NUM_THREADS, array_of_handles, true, INFINITE);

	for (int i = 0; i < NUM_THREADS; i++) {
		CloseHandle(array_of_handles[i]);
	}

	return 0;
}