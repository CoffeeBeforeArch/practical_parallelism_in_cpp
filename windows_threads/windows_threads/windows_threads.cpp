// This program shows off the basics of windows threads in C++
// By: Nick from CoffeeBeforeArch

#include <Windows.h>
#include <strsafe.h>

#define NUM_THREADS 10
#define BUF_SIZE 255

// Argument we will be passing to out spawned threads
struct Package {
  int tid;
  int number;
};

void display_message(const HANDLE hScreen, const int tid, const int number, const char *message) {
  // A character array
  TCHAR message_buffer[BUF_SIZE];
  size_t cchStringSize;
  // DWORD is a Windows-specific type (Unsigned 32-bit)
  DWORD dwChars;

  // Print using thread-safe functions
  // Print a string to the buffer and checks to make sure it doesn't exceed
  // length
  StringCchPrintf(message_buffer, BUF_SIZE,
                  TEXT("Printing number %d from thread %d. %s\n"), tid, number,
                  message);
  // Check for a buffer overrun
  StringCchLength(message_buffer, BUF_SIZE, &cchStringSize);
  // Write character string to console
  WriteConsole(hScreen, message_buffer, cchStringSize, &dwChars, NULL);

  // Just put the thread to sleep for a little
  Sleep(1000);
}

// WINAPI is a macro that specifies the use of the Windows calling
// convention
// LPVOID is just a void pointer
DWORD WINAPI boring_thread_function(const LPVOID lpParam) {
  // Cast our void pointer to our defined struct
  const Package &local_package = *(Package *)lpParam;

  // Get Handle to the screen
  HANDLE hStdout = NULL;
  // Get handle to stdout
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

  // Call our function with thread-safe print
  display_message(hStdout, local_package.tid, local_package.number,
                  "Hello, there!");

  return 0;
}

int main() {
  // Create an array of handles
  HANDLE array_of_handles[NUM_THREADS]{};
  // Create an array of structs as arguments
  Package packages[NUM_THREADS];

  // Create threads
  for (int i = 0; i < NUM_THREADS; i++) {
    packages[i].tid = i;
    packages[i].number = i * i;
    /*
            Arguments:
                    0.) Security Atrributes (default)
                    1.) Stack Size (default)
                    2.) Function
                    3.) Argument (converted to void pointer)
                    4.) Creation flag (0 = run immedately after creation)
                    5.) Pointer to variable to receive thread id
    */
    array_of_handles[i] =
        CreateThread(NULL, 0, boring_thread_function, &packages[i], 0, NULL);
  }

  // Wait for a number of threads to be in the signaled state
  // Infinite states to wait forever
  WaitForMultipleObjects(NUM_THREADS, array_of_handles, true, INFINITE);

  // Clean up by closing open object handles
  for (auto & array_of_handle : array_of_handles) {
    CloseHandle(array_of_handle);
  }

  return 0;
}
