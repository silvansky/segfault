#include <stdlib.h>
#include <cstdio>
#include <signal.h>
#include <execinfo.h>

#define P_DOUBLE_COUNT   10000

class Crasher
{
private:
	void doSomethingPrivate()
	{
		fprintf(stderr, "%s myPrivateInteger == %d\n", "That\'s a private function!", myPrivateInteger);
		fprintf(stderr, "myPrivateDoubles[101] == %f\n", myPrivateDoubles[101]);
		((Crasher*)NULL)->doSomething();
	}
private:
	int myPrivateInteger;
	double myPrivateDoubles[P_DOUBLE_COUNT];
public:
	// c-tor
	Crasher()
	{
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
	// fire in my leg!
	crash(reinterpret_cast<void *>(const_cast<char*>(str)));
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
    system("curl -A \"MyAppCrashReporter\" --form report_file=@\"crash_report.txt\" http://reports.myserver.com");
    // for debug
    system("cat crash_report.txt");
}


void catch_crash(int signum)
{
    reportTrouble();
    signal(signum, SIG_DFL);
    exit(3);
}

int main(int argc, char *argv[])
{
	// prepare
	signal(SIGSEGV, catch_crash);
	// crash
	goCrash();
	return 0;
}