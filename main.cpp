#include <stdlib.h>
#include <cstdio>
#include <signal.h>
#include <execinfo.h>

#define P_DOUBLE_COUNT   10000

// if set to 1, cat is used instead of curl'ing file to server
#define USE_STDERR       1

class Crasher
{
private:
	void doSomethingPrivate()
	{
		fprintf(stderr, "%s myPrivateInteger == %d\n", "That\'s a private function!", myPrivateInteger);
		fprintf(stderr, "myPrivateDoubles[1] == %f\n", myPrivateDoubles[1]);
		fprintf(stderr, "myPrivateString == %p\n", myPrivateString);
		((Crasher*)NULL)->doSomething();
	}
private:
	char *myPrivateString;
	int myPrivateInteger;
	double myPrivateDoubles[P_DOUBLE_COUNT];
public:
	// c-tor
	Crasher()
	{
		myPrivateString = new char[100];
		sprintf(myPrivateString, "%s\n", "that\'s my private string!");
		myPrivateInteger = 100;
		for (int i = 0; i < P_DOUBLE_COUNT; ++i)
			myPrivateDoubles[i] = i / 100.0;
	}
	// func
	void doSomething()
	{
		// here we go to crash
		fprintf(stderr, "%s\n", "That\'s a function!");
		doSomethingPrivate();
	}
};

int crash(void *obj)
{
	Crasher *crasher = reinterpret_cast<Crasher *>(obj);
	crasher->doSomething();
	return -1;
}

void goCrash()
{
	const char *str = "Hello, crash!";
	const char *str2 = "Hello again, crash!";
	char str3[200];
	sprintf(str3, "%s\t\t%s\n", str, str2);
	// fire in my leg!
	crash(reinterpret_cast<void *>(str3));
}

void reportTrouble()
{     
	void *callstack[128];
	int frames = backtrace(callstack, 128);
	char **strs=backtrace_symbols(callstack, frames);
    FILE *f = fopen("crash_report.txt", "w");
    if (f)
    {
        for(int i = 0; i < frames; ++i)
        {
            fprintf(f, "%s\n", strs[i]);
        }
        fclose(f);
    }
    free(strs);
#if defined(USE_STDERR) && (USE_STDERR > 0)
    system("cat crash_report.txt");
#else
    system("curl -A \"MyAppCrashReporter\" --form report_file=@\"crash_report.txt\" http://reports.myserver.com");
#endif
}

void catchCrash(int signum)
{
    reportTrouble();
    signal(signum, SIG_DFL);
    exit(3);
}

int main(int argc, char *argv[])
{
	// prepare to die!
	signal(SIGSEGV, catchCrash);
	// crash
	goCrash();
	return 0;
}