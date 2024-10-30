(in-package :core)

(defpackage "CLASP-POSIX"
  (:use)
  (:import-from "CORE"
                "ARGC" "ARGV" "RMDIR" "MKSTEMP" "CHMOD" "GETPID"
                "FORK" "FORK-REDIRECT" "SIGCHLD-COUNT"
                "WAIT" "WIFEXITED" "WIFSIGNALED" "WTERMSIG"
                "LSEEK" "READ-FD" "CLOSE-FD" "MKSTEMP-FD"
                "SIGSET" "SIGSET-SIGADDSET"
                "SIGNAL-SIGSTOP" "SIGNAL-SIGUSR1" "SIGNAL-SIGTTIN"
                "SIGNAL-SIGILL" "SIGNAL-SIGXCPU" "SIGNAL-SIGQUIT"
                "SIGNAL-SIGSEGV" "SIGNAL-SIGSYS" "SIGNAL-SIGXFSZ"
                "SIGNAL-SIGTRAP" "SIGNAL-SIGPIPE" "SIGNAL-SIGURG"
                "SIGNAL-SIGUSR2" "SIGNAL-SIGPROF" "SIGNAL-SIGHUP"
                "SIGNAL-SIGTERM" "SIGNAL-SIGBUS" "SIGNAL-SIGABRT"
                "SIGNAL-SIGTSTP" "SIGNAL-SIGCONT" "SIGNAL-SIGCHLD"
                "SIGNAL-SIGKILL" "SIGNAL-SIGINT" "SIGNAL-SIGFPE"
                "SIGNAL-SIGVTALRM" "SIGNAL-SIGTTOU" "SIGNAL-SIGALRM")
  (:export "ARGC" "ARGV" "RMDIR" "MKSTEMP" "CHMOD" "GETPID"
           "FORK" "FORK-REDIRECT" "SIGCHLD-COUNT"
           "WAIT" "WIFEXITED" "WIFSIGNALED" "WTERMSIG"
           "LSEEK" "READ-FD" "CLOSE-FD" "MKSTEMP-FD"
           "SIGSET" "SIGSET-SIGADDSET"
           "SIGNAL-SIGSTOP" "SIGNAL-SIGUSR1" "SIGNAL-SIGTTIN"
           "SIGNAL-SIGILL" "SIGNAL-SIGXCPU" "SIGNAL-SIGQUIT"
           "SIGNAL-SIGSEGV" "SIGNAL-SIGSYS" "SIGNAL-SIGXFSZ"
           "SIGNAL-SIGTRAP" "SIGNAL-SIGPIPE" "SIGNAL-SIGURG"
           "SIGNAL-SIGUSR2" "SIGNAL-SIGPROF" "SIGNAL-SIGHUP"
           "SIGNAL-SIGTERM" "SIGNAL-SIGBUS" "SIGNAL-SIGABRT"
           "SIGNAL-SIGTSTP" "SIGNAL-SIGCONT" "SIGNAL-SIGCHLD"
           "SIGNAL-SIGKILL" "SIGNAL-SIGINT" "SIGNAL-SIGFPE"
           "SIGNAL-SIGVTALRM" "SIGNAL-SIGTTOU" "SIGNAL-SIGALRM"))
