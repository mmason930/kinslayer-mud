#include "SystemUtil.h"

#include <cassert>
#include <cerrno>
#include <climits>
#include <csignal>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    assert(SystemUtil::processExists(getpid()));
    assert(!SystemUtil::processExists(0));
    assert(!SystemUtil::processExists(UINT_MAX));

    const pid_t child = fork();
    assert(child >= 0);
    if (child == 0)
    {
        raise(SIGSTOP);
        _exit(0);
    }

    int status;
    assert(waitpid(child, &status, WUNTRACED) == child);
    assert(WIFSTOPPED(status));
    assert(SystemUtil::processExists(child));
    assert(kill(child, SIGCONT) == 0);

    // Observe an exited child without reaping it: reproduce the outage state.
    siginfo_t info{};
    assert(waitid(P_PID, child, &info, WEXITED | WNOWAIT) == 0);
    assert(!SystemUtil::processExists(child));
    assert(waitpid(child, &status, 0) == child);
    assert(!SystemUtil::processExists(child));

    for (int i = 0; i < 50; ++i)
    {
        assert(SystemUtil::processCommand("printf 'hello'; exit 7") == "hello");
        errno = 0;
        assert(waitpid(-1, &status, WNOHANG) == -1);
        assert(errno == ECHILD); // pclose must collect the command's child.
    }
    assert(SystemUtil::processCommand("printf 'a\\000b'") == std::string("a\0b", 3));
    std::cout << "Process liveness and subprocess cleanup passed\n";
}
