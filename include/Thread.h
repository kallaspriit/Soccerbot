#ifndef THREAD_H
#define THREAD_H

#include <memory>
#include <windows.h>

// http://www.bogotobogo.com/cplusplus/multithreaded2.php

class Runnable {
	public:
		virtual void* run() = 0;
		virtual ~Runnable() = 0;
};

class Thread {
	public:
		Thread(std::auto_ptr<Runnable> run);
		Thread();
		virtual ~Thread();
		void start();
		void* join();

	private:
		Thread(const Thread&);
		const Thread& operator=(const Thread&);
		void setCompleted();
		virtual void* run() { return 0; }
		static unsigned WINAPI startThreadRunnable(LPVOID pVoid);
		static unsigned WINAPI startThread(LPVOID pVoid);
		void printError(LPTSTR lpszFunction, LPSTR fileName, int lineNumber);

		HANDLE hThread;
		unsigned wThreadID;
		std::auto_ptr<Runnable> runnable;
		void* result;
};

#endif // THREAD_H