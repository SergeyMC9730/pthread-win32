/*
 * implement.h
 *
 * Definitions that don't need to be public.
 *
 * Keeps all the internals out of pthread.h
 *
 * --------------------------------------------------------------------------
 *
 *      pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999-2021 pthreads-win32 / pthreads4w contributors
 *
 *      Homepage1: http://sourceware.org/pthreads-win32/
 *      Homepage2: http://sourceforge.net/projects/pthreads4w/
 *
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 * 
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * 
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 * 
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * --------------------------------------------------------------------------
 */
#pragma once
#if !defined(_IMPLEMENT_H)
#define _IMPLEMENT_H

#if !defined (PTW32_CONFIG_H)
#pragma message("config.h was not #included")
#include "config.h"
#endif

#include "_ptw32.h"

#include <windows.h>
#include <sys/types.h>
/*
 * In case windows.h doesn't define it (e.g. WinCE perhaps)
 */
#if defined(WINCE)
typedef VOID (APIENTRY *PAPCFUNC)(DWORD dwParam);
#endif

/*
 * Designed to allow error values to be set and retrieved in builds where
 * MSCRT libraries are statically linked to DLLs.
 *
 * This does not handle the case where a static pthreads4w lib is linked
 * to a static linked app. Compiling and linking pthreads.c with app.c
 * as one does work with the right macros defined. See tests/Makefile
 * for clues or just "cd tests && nmake clean VC-static".
 */
#if ! defined(WINCE) && \
    (( defined(__MINGW32__) && __MSVCRT_VERSION__ >= 0x0800 ) || \
    ( defined(_MSC_VER) && _MSC_VER >= 1400 ))  /* MSVC8+ */
#  if defined(__MINGW32__)
__attribute__((unused))
#  endif
static int ptw32_get_errno(void) { int err = 0; _get_errno(&err); return err; }
#  define PTW32_GET_ERRNO() ptw32_get_errno()
#  if defined(__MINGW32__)
__attribute__((unused))
#  endif
static void ptw32_set_errno(int err) { _set_errno(err); SetLastError(err); }
#    define PTW32_SET_ERRNO(err) ptw32_set_errno(err)
#else
#  define PTW32_GET_ERRNO() (errno)
#  if defined(__MINGW32__)
__attribute__((unused))
#  endif
static void ptw32_set_errno(int err) { errno = err; SetLastError(err); }
#    define PTW32_SET_ERRNO(err) ptw32_set_errno(err)
#endif

/*
 * note: ETIMEDOUT is correctly defined in winsock.h or errno.h
 */
#include <winsock.h>
#include <errno.h>

/*
 * In case ETIMEDOUT hasn't been defined above somehow.
 */
#if !defined(ETIMEDOUT)
# define ETIMEDOUT 10060	/* This is the value in winsock.h. */
#endif

#if !defined(malloc)
# include <malloc.h>
#endif

#if defined(PTW32_CLEANUP_C)
# include <setjmp.h>
#endif

#if !defined(INT_MAX)
# include <limits.h>
#endif

/* _tcsncat_s() et al: mapping to the correct TCHAR prototypes: */
#include <tchar.h>

#if defined(__BORLANDC__) && !defined(_tcsncat_s)
  #define _tcsncat_s(arg1, arg2, arg3, arg4) _tcsncat(arg1, arg3, (arg2 < arg4 ? arg2 : arg4))
#endif

/* use local include files during development */
#include "semaphore.h"
#include "sched.h"

/* MSVC 7.1 doesn't like complex #if expressions */
#define INLINE
#if defined(PTW32_BUILD_INLINED)
#  if defined(HAVE_C_INLINE) || defined(__cplusplus)
#    undef INLINE
#    define INLINE inline
#  endif
#endif

#if defined(PTW32_CONFIG_MSVC6)
/*
 * MSVC 6 does not use the "volatile" qualifier
 */
# define PTW32_INTERLOCKED_VOLATILE
#else
# define PTW32_INTERLOCKED_VOLATILE volatile
#endif

#define PTW32_INTERLOCKED_LONG long
#if defined(_M_IA64)
#define PTW32_INTERLOCKED_SIZE LONG64
#elif defined(_M_AMD64)
#define PTW32_INTERLOCKED_SIZE LONG64
#elif defined(_WIN64)
#define PTW32_INTERLOCKED_SIZE LONGLONG
#else
#define PTW32_INTERLOCKED_SIZE LONG
#endif
#define PTW32_INTERLOCKED_PVOID PVOID
#define PTW32_INTERLOCKED_LONGPTR PTW32_INTERLOCKED_VOLATILE long*
#if defined(_M_IA64)
#define PTW32_INTERLOCKED_SIZEPTR PTW32_INTERLOCKED_VOLATILE LONG64*
#elif defined(_M_AMD64)
#define PTW32_INTERLOCKED_SIZEPTR PTW32_INTERLOCKED_VOLATILE LONG64*
#elif defined(_WIN64)
#define PTW32_INTERLOCKED_SIZEPTR PTW32_INTERLOCKED_VOLATILE LONGLONG*
#else
#define PTW32_INTERLOCKED_SIZEPTR PTW32_INTERLOCKED_VOLATILE LONG*
#endif
#define PTW32_INTERLOCKED_PVOID_PTR PTW32_INTERLOCKED_VOLATILE PVOID*

#if defined(__MINGW32__)
#  include <stdint.h>
#elif defined(__BORLANDC__)
#  define int64_t ULONGLONG
#else
#  define int64_t _int64
#  if defined(PTW32_CONFIG_MSVC6)
     typedef long intptr_t;
#  endif
#endif

/*
 * Don't allow the linker to optimize away dll.obj (dll.o) in static builds.
 */
#if defined (PTW32_STATIC_LIB) && defined (PTW32_BUILD) && !defined (PTW32_TEST_SNEAK_PEEK)
PTW32_BEGIN_C_DECLS
  void ptw32_autostatic_anchor(void);
# if defined(__GNUC__)
    __attribute__((unused, used))
# endif
  static void (*local_autostatic_anchor)(void) = ptw32_autostatic_anchor;
PTW32_END_C_DECLS
#endif

typedef enum
{
  /*
   * This enumeration represents the state of the thread;
   * The thread is still valid if the numeric value of the
   * state is greater or equal "PThreadStateRunning".
   */
  PThreadStateInitial = 0,	/* Thread not running                   */
  PThreadStateReuse,            /* In reuse pool.                       */
  PThreadStateRunning,		/* Thread alive & kicking               */
  PThreadStateSuspended,	/* Thread alive but suspended           */
  PThreadStateCancelPending,	/* Thread alive but                     */
                                /* has cancellation pending.            */
  PThreadStateCanceling,	/* Thread alive but is                  */
                                /* in the process of terminating        */
                                /* due to a cancellation request        */
  PThreadStateExiting,		/* Thread alive but exiting             */
                                /* due to an exception                  */
  PThreadStateLast              /* All handlers have been run and now   */
                                /* final cleanup can be done.           */
}
PThreadState;

typedef struct ptw32_mcs_node_t_     ptw32_mcs_local_node_t;
typedef struct ptw32_mcs_node_t_*    ptw32_mcs_lock_t;
typedef struct ptw32_robust_node_t_  ptw32_robust_node_t;
typedef struct ptw32_thread_t_       ptw32_thread_t;

#ifdef _MSC_VER
  // Suppress warnings about padding changes due to alignment.
  #pragma warning(push)
  #pragma warning(disable: 4324)
#endif // _MSC_VER

struct ptw32_thread_t_
{
  unsigned __int64 seqNumber;	/* Process-unique thread sequence number */
  HANDLE threadH;		/* Win32 thread handle - POSIX thread is invalid if threadH == 0 */
  pthread_t ptHandle;		/* This thread's permanent pthread_t handle */
  ptw32_thread_t * prevReuse;	/* Links threads on reuse stack; sentinel is PTW32_THREAD_REUSE_EMPTY */
  volatile PThreadState state;
  ptw32_mcs_lock_t threadLock;	/* Used for serialised access to public thread state */
  ptw32_mcs_lock_t stateLock;	/* Used for async-cancel safety */
  HANDLE cancelEvent;
  void *exitStatus;
  void *parms;
  void *keys;
  void *nextAssoc;
#if defined(PTW32_CLEANUP_C)
  jmp_buf start_mark;		/* Jump buffer follows void* so should be aligned */
#endif				/* PTW32_CLEANUP_C */
#if defined(HAVE_SIGSET_T)
  sigset_t sigmask;
#endif				/* HAVE_SIGSET_T */
  ptw32_mcs_lock_t
              robustMxListLock; /* robustMxList lock */
  ptw32_robust_node_t*
                  robustMxList; /* List of currenty held robust mutexes */
  int ptErrno;
  int detachState;
  int sched_priority;		/* As set, not as currently is */
  int cancelState;
  int cancelType;
  unsigned int implicit:1;
  DWORD thread;			/* Windows thread ID */
#if defined(HAVE_CPU_AFFINITY)
  size_t cpuset;		/* Thread CPU affinity set */
#endif
  char * name;                  /* Thread name */
#if defined(_UWIN)
  DWORD dummy[5];
#endif
  size_t align;			/* Force alignment if this struct is packed */
};

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER


/*
 * Special value to mark attribute objects as valid.
 */
#define PTW32_ATTR_VALID ((unsigned long) 0xC4C0FFEE)

struct pthread_attr_t_
{
  unsigned long valid;
  void *stackaddr;
  size_t stacksize;
  int detachstate;
  struct sched_param param;
  int inheritsched;
  int contentionscope;
  size_t cpuset;
  char * thrname;
#if defined(HAVE_SIGSET_T)
  sigset_t sigmask;
#endif				/* HAVE_SIGSET_T */
};


/*
 * ====================
 * ====================
 * Semaphores, Mutexes and Condition Variables
 * ====================
 * ====================
 */

struct sem_t_
{
  int value;
  ptw32_mcs_lock_t lock;
  HANDLE sem;
#if defined(NEED_SEM)
  int leftToUnblock;
#endif
};

#define PTW32_OBJECT_AUTO_INIT ((void *)(size_t) -1)
#define PTW32_OBJECT_INVALID   NULL

struct pthread_mutex_t_
{
  LONG lock_idx;		/* Provides exclusive access to mutex state
				   via the Interlocked* mechanism.
				    0: unlocked/free.
				    1: locked - no other waiters.
				   -1: locked - with possible other waiters.
				*/
  int recursive_count;		/* Number of unlocks a thread needs to perform
				   before the lock is released (recursive
				   mutexes only). */
  int kind;			/* Mutex type. */
  pthread_t ownerThread;
  HANDLE event;			/* Mutex release notification to waiting
				   threads. */
  ptw32_robust_node_t*
                    robustNode; /* Extra state for robust mutexes  */
};

enum ptw32_robust_state_t_
{
  PTW32_ROBUST_CONSISTENT,
  PTW32_ROBUST_INCONSISTENT,
  PTW32_ROBUST_NOTRECOVERABLE
};

typedef enum ptw32_robust_state_t_   ptw32_robust_state_t;

/*
 * Node used to manage per-thread lists of currently-held robust mutexes.
 */
struct ptw32_robust_node_t_
{
  pthread_mutex_t mx;
  ptw32_robust_state_t stateInconsistent;
  ptw32_robust_node_t* prev;
  ptw32_robust_node_t* next;
};

struct pthread_mutexattr_t_
{
  int pshared;
  int kind;
  int robustness;
};

/*
 * Possible values, other than PTW32_OBJECT_INVALID,
 * for the "interlock" element in a spinlock.
 *
 * In this implementation, when a spinlock is initialised,
 * the number of cpus available to the process is checked.
 * If there is only one cpu then "interlock" is set equal to
 * PTW32_SPIN_USE_MUTEX and u.mutex is an initialised mutex.
 * If the number of cpus is greater than 1 then "interlock"
 * is set equal to PTW32_SPIN_UNLOCKED and the number is
 * stored in u.cpus. This arrangement allows the spinlock
 * routines to attempt an InterlockedCompareExchange on "interlock"
 * immediately and, if that fails, to try the inferior mutex.
 *
 * "u.cpus" isn't used for anything yet, but could be used at
 * some point to optimise spinlock behaviour.
 */
#define PTW32_SPIN_INVALID     (0)
#define PTW32_SPIN_UNLOCKED    (1)
#define PTW32_SPIN_LOCKED      (2)
#define PTW32_SPIN_USE_MUTEX   (3)

struct pthread_spinlock_t_
{
  long interlock;		/* Locking element for multi-cpus. */
  union
  {
    int cpus;			/* No. of cpus if multi cpus, or   */
    pthread_mutex_t mutex;	/* mutex if single cpu.            */
  } u;
};

/*
 * MCS lock queue node - see ptw32_MCS_lock.c
 */
struct ptw32_mcs_node_t_
{
  struct ptw32_mcs_node_t_ **lock;        /* ptr to tail of queue */
  struct ptw32_mcs_node_t_  *next;        /* ptr to successor in queue */
  HANDLE                     readyFlag;   /* set after lock is released by
                                             predecessor */
  HANDLE                     nextFlag;    /* set after 'next' ptr is set by
                                             successor */
};


struct pthread_barrier_t_
{
  unsigned int nCurrentBarrierHeight;
  unsigned int nInitialBarrierHeight;
  int pshared;
  sem_t semBarrierBreeched;
  ptw32_mcs_lock_t lock;
  ptw32_mcs_local_node_t proxynode;
};

struct pthread_barrierattr_t_
{
  int pshared;
};

struct pthread_key_t_
{
  DWORD key;
  void (PTW32_CDECL *destructor) (void *);
  ptw32_mcs_lock_t keyLock;
  void *threads;
};


typedef struct ThreadParms ThreadParms;

struct ThreadParms
{
  pthread_t tid;
  void * (PTW32_CDECL *start) (void *);
  void *arg;
};


struct pthread_cond_t_
{
  long nWaitersBlocked;		/* Number of threads blocked            */
  long nWaitersGone;		/* Number of threads timed out          */
  long nWaitersToUnblock;	/* Number of threads to unblock         */
  sem_t semBlockQueue;		/* Queue up threads waiting for the     */
  /*   condition to become signalled      */
  sem_t semBlockLock;		/* Semaphore that guards access to      */
  /* | waiters blocked count/block queue  */
  /* +-> Mandatory Sync.LEVEL-1           */
  pthread_mutex_t mtxUnblockLock;	/* Mutex that guards access to          */
  /* | waiters (to)unblock(ed) counts     */
  /* +-> Optional* Sync.LEVEL-2           */
  pthread_cond_t next;		/* Doubly linked list                   */
  pthread_cond_t prev;
};


struct pthread_condattr_t_
{
  int pshared;
};

#define PTW32_RWLOCK_MAGIC 0xfacade2

struct pthread_rwlock_t_
{
  pthread_mutex_t mtxExclusiveAccess;
  pthread_mutex_t mtxSharedAccessCompleted;
  pthread_cond_t cndSharedAccessCompleted;
  int nSharedAccessCount;
  int nExclusiveAccessCount;
  int nCompletedSharedAccessCount;
  int nMagic;
};

struct pthread_rwlockattr_t_
{
  int pshared;
};

typedef union
{
  char cpuset[CPU_SETSIZE/8];
  size_t _cpuset;
} _sched_cpu_set_vector_;

typedef struct ThreadKeyAssoc ThreadKeyAssoc;

struct ThreadKeyAssoc
{
  /*
   * Purpose:
   *      This structure creates an association between a thread and a key.
   *      It is used to implement the implicit invocation of a user defined
   *      destroy routine for thread specific data registered by a user upon
   *      exiting a thread.
   *
   *      Graphically, the arrangement is as follows, where:
   *
   *         K - Key with destructor
   *            (head of chain is key->threads)
   *         T - Thread that has called pthread_setspecific(Kn)
   *            (head of chain is thread->keys)
   *         A - Association. Each association is a node at the
   *             intersection of two doubly-linked lists.
   *
   *                 T1    T2    T3
   *                 |     |     |
   *                 |     |     |
   *         K1 -----+-----A-----A----->
   *                 |     |     |
   *                 |     |     |
   *         K2 -----A-----A-----+----->
   *                 |     |     |
   *                 |     |     |
   *         K3 -----A-----+-----A----->
   *                 |     |     |
   *                 |     |     |
   *                 V     V     V
   *
   *      Access to the association is guarded by two locks: the key's
   *      general lock (guarding the row) and the thread's general
   *      lock (guarding the column). This avoids the need for a
   *      dedicated lock for each association, which not only consumes
   *      more handles but requires that the lock resources persist
   *      until both the key is deleted and the thread has called the
   *      destructor. The two-lock arrangement allows those resources
   *      to be freed as soon as either thread or key is concluded.
   *
   *      To avoid deadlock, whenever both locks are required both the
   *      key and thread locks are acquired consistently in the order
   *      "key lock then thread lock". An exception to this exists
   *      when a thread calls the destructors, however, this is done
   *      carefully (but inelegantly) to avoid deadlock.
   *
   *      An association is created when a thread first calls
   *      pthread_setspecific() on a key that has a specified
   *      destructor.
   *
   *      An association is destroyed either immediately after the
   *      thread calls the key destructor function on thread exit, or
   *      when the key is deleted.
   *
   * Attributes:
   *      thread
   *              reference to the thread that owns the
   *              association. This is actually the pointer to the
   *              thread struct itself. Since the association is
   *              destroyed before the thread exits, this can never
   *              point to a different logical thread to the one that
   *              created the assoc, i.e. after thread struct reuse.
   *
   *      key
   *              reference to the key that owns the association.
   *
   *      nextKey
   *              The pthread_t->keys attribute is the head of a
   *              chain of associations that runs through the nextKey
   *              link. This chain provides the 1 to many relationship
   *              between a pthread_t and all pthread_key_t on which
   *              it called pthread_setspecific.
   *
   *      prevKey
   *              Similarly.
   *
   *      nextThread
   *              The pthread_key_t->threads attribute is the head of
   *              a chain of associations that runs through the
   *              nextThreads link. This chain provides the 1 to many
   *              relationship between a pthread_key_t and all the
   *              PThreads that have called pthread_setspecific for
   *              this pthread_key_t.
   *
   *      prevThread
   *              Similarly.
   *
   * Notes:
   *      1)      As soon as either the key or the thread is no longer
   *              referencing the association, it can be destroyed. The
   *              association will be removed from both chains.
   *
   *      2)      Under WIN32, an association is only created by
   *              pthread_setspecific if the user provided a
   *              destroyRoutine when they created the key.
   *
   *
   */
  ptw32_thread_t * thread;
  pthread_key_t key;
  ThreadKeyAssoc *nextKey;
  ThreadKeyAssoc *nextThread;
  ThreadKeyAssoc *prevKey;
  ThreadKeyAssoc *prevThread;
};


#if defined(PTW32_CLEANUP_SEH)
/*
 * --------------------------------------------------------------
 * MAKE_SOFTWARE_EXCEPTION
 *      This macro constructs a software exception code following
 *      the same format as the standard Win32 error codes as defined
 *      in WINERROR.H
 *  Values are 32 bit values laid out as follows:
 *
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  +---+-+-+-----------------------+-------------------------------+
 *  |Sev|C|R|     Facility          |               Code            |
 *  +---+-+-+-----------------------+-------------------------------+
 *
 * Severity Values:
 */
#define SE_SUCCESS              0x00
#define SE_INFORMATION          0x01
#define SE_WARNING              0x02
#define SE_ERROR                0x03

#define MAKE_SOFTWARE_EXCEPTION( _severity, _facility, _exception ) \
( (DWORD) ( ( (_severity) << 30 ) |     /* Severity code        */ \
            ( 1 << 29 ) |               /* MS=0, User=1         */ \
            ( 0 << 28 ) |               /* Reserved             */ \
            ( (_facility) << 16 ) |     /* Facility Code        */ \
            ( (_exception) <<  0 )      /* Exception Code       */ \
            ) )

/*
 * We choose one specific Facility/Error code combination to
 * identify our software exceptions vs. WIN32 exceptions.
 * We store our actual component and error code within
 * the optional information array.
 */
#define EXCEPTION_PTW32_SERVICES        \
     MAKE_SOFTWARE_EXCEPTION( SE_ERROR, \
                              PTW32_SERVICES_FACILITY, \
                              PTW32_SERVICES_ERROR )

#define PTW32_SERVICES_FACILITY         0xBAD
#define PTW32_SERVICES_ERROR            0xDEED

#endif /* PTW32_CLEANUP_SEH */

/*
 * Services available through EXCEPTION_PTW32_SERVICES
 * and also used [as parameters to ptw32_throw()] as
 * generic exception selectors.
 */

#define PTW32_EPS_EXIT                  (1)
#define PTW32_EPS_CANCEL                (2)


/* Useful macros */
#define PTW32_MAX(a,b)  ((a)<(b)?(b):(a))
#define PTW32_MIN(a,b)  ((a)>(b)?(b):(a))


PTW32_BEGIN_C_DECLS

/* Declared in pthread_cancel.c */
extern DWORD (*ptw32_register_cancellation) (PAPCFUNC, HANDLE, DWORD);

PTW32_END_C_DECLS

/* Thread Reuse stack bottom marker. Must not be NULL or any valid pointer to memory. */
#define PTW32_THREAD_REUSE_EMPTY ((ptw32_thread_t *)(size_t) 1)

PTW32_BEGIN_C_DECLS

extern int ptw32_processInitialized;
extern ptw32_thread_t * ptw32_threadReuseTop;
extern ptw32_thread_t * ptw32_threadReuseBottom;
extern pthread_key_t ptw32_selfThreadKey;
extern pthread_key_t ptw32_cleanupKey;
extern pthread_cond_t ptw32_cond_list_head;
extern pthread_cond_t ptw32_cond_list_tail;

extern int ptw32_mutex_default_kind;

extern unsigned __int64 ptw32_threadSeqNumber;

extern int ptw32_concurrency;

extern int ptw32_features;

extern ptw32_mcs_lock_t ptw32_thread_reuse_lock;
extern ptw32_mcs_lock_t ptw32_mutex_test_init_lock;
extern ptw32_mcs_lock_t ptw32_cond_list_lock;
extern ptw32_mcs_lock_t ptw32_cond_test_init_lock;
extern ptw32_mcs_lock_t ptw32_rwlock_test_init_lock;
extern ptw32_mcs_lock_t ptw32_spinlock_test_init_lock;

#if defined(_UWIN)
extern int pthread_count;
#endif

PTW32_END_C_DECLS

PTW32_BEGIN_C_DECLS

/*
 * =====================
 * =====================
 * Forward Declarations
 * =====================
 * =====================
 */

  int ptw32_is_attr (const pthread_attr_t * attr);

  int ptw32_cond_check_need_init (pthread_cond_t * cond);
  int ptw32_mutex_check_need_init (pthread_mutex_t * mutex);
  int ptw32_rwlock_check_need_init (pthread_rwlock_t * rwlock);
  int ptw32_spinlock_check_need_init (pthread_spinlock_t * lock);

  int ptw32_robust_mutex_inherit(pthread_mutex_t * mutex);
  void ptw32_robust_mutex_add(pthread_mutex_t* mutex, pthread_t self);
  void ptw32_robust_mutex_remove(pthread_mutex_t* mutex, ptw32_thread_t* otp);

  DWORD
    ptw32_RegisterCancellation (PAPCFUNC callback,
			       HANDLE threadH, DWORD callback_arg);

  int ptw32_processInitialize (void);

  void ptw32_processTerminate (void);

  void ptw32_threadDestroy (pthread_t tid);

  void ptw32_pop_cleanup_all (int execute);

  pthread_t ptw32_new (void);

  pthread_t ptw32_threadReusePop (void);

  void ptw32_threadReusePush (pthread_t thread);

  int ptw32_getprocessors (int *count);

  int ptw32_setthreadpriority (pthread_t thread, int policy, int priority);

  void PTW32_CDECL ptw32_rwlock_cancelwrwait (void *arg); /* matches type ptw32_cleanup_callback_t this way */

#if ! defined (__MINGW32__) || (defined (__MSVCRT__) && ! defined (__DMC__))
  unsigned __stdcall
#else
  void
#endif
    ptw32_threadStart (void *vthreadParms);

  void ptw32_callUserDestroyRoutines (pthread_t thread);

  int ptw32_tkAssocCreate (ptw32_thread_t * thread, pthread_key_t key);

  void ptw32_tkAssocDestroy (ThreadKeyAssoc * assoc);

  int ptw32_semwait (sem_t * sem);

  DWORD ptw32_relmillisecs (const struct timespec * abstime);

  void ptw32_mcs_lock_acquire (ptw32_mcs_lock_t * lock, ptw32_mcs_local_node_t * node);

  int ptw32_mcs_lock_try_acquire (ptw32_mcs_lock_t * lock, ptw32_mcs_local_node_t * node);

  void ptw32_mcs_lock_release (ptw32_mcs_local_node_t * node);

  void ptw32_mcs_node_transfer (ptw32_mcs_local_node_t * new_node, ptw32_mcs_local_node_t * old_node);

  void ptw32_timespec_to_filetime (const struct timespec *ts, FILETIME * ft);

  void ptw32_filetime_to_timespec (const FILETIME * ft, struct timespec *ts);

/* Declared in pthw32_calloc.c */
#if defined(NEED_CALLOC)
#define calloc(n, s) ptw32_calloc(n, s)
  void *ptw32_calloc (size_t n, size_t s);
#endif

char *ptw32_strdup (const char *s);

/* Declared in ptw32_throw.c */
#if defined(_MSC_VER)
/*
 * Ignore the warning:
 * "C++ exception specification ignored except to indicate that
 * the function is not __declspec(nothrow)."
 */
#pragma warning(disable:4290)
#endif
void ptw32_throw (DWORD exception)
#if defined(__cplusplus)
#if __cplusplus <= 201402L
    throw(ptw32_exception_cancel,ptw32_exception_exit)
#else
	noexcept(false)
#endif
#endif
;

PTW32_END_C_DECLS

#if defined(_UWIN_)
#   if defined(_MT)

PTW32_BEGIN_C_DECLS

  _CRTIMP unsigned long __cdecl _beginthread (void (__cdecl *) (void *),
					      unsigned, void *);
  _CRTIMP void __cdecl _endthread (void);
  _CRTIMP unsigned long __cdecl _beginthreadex (void *, unsigned,
						unsigned (__stdcall *) (void *),
						void *, unsigned, unsigned *);
  _CRTIMP void __cdecl _endthreadex (unsigned);

PTW32_END_C_DECLS

#   endif
#else
#   if ! defined(WINCE)
#     include <process.h>
#   endif
#endif


/*
 * Use intrinsic versions wherever possible. VC will do this
 * automatically where possible and GCC define these if available:
 * __GCC_HAVE_SYNC_COMPARE_AND_SWAP_1
 * __GCC_HAVE_SYNC_COMPARE_AND_SWAP_2
 * __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
 * __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8
 * __GCC_HAVE_SYNC_COMPARE_AND_SWAP_16
 *
 * The full set of Interlocked intrinsics in GCC are (check versions):
 * type __sync_fetch_and_add (type *ptr, type value, ...)
 * type __sync_fetch_and_sub (type *ptr, type value, ...)
 * type __sync_fetch_and_or (type *ptr, type value, ...)
 * type __sync_fetch_and_and (type *ptr, type value, ...)
 * type __sync_fetch_and_xor (type *ptr, type value, ...)
 * type __sync_fetch_and_nand (type *ptr, type value, ...)
 * type __sync_add_and_fetch (type *ptr, type value, ...)
 * type __sync_sub_and_fetch (type *ptr, type value, ...)
 * type __sync_or_and_fetch (type *ptr, type value, ...)
 * type __sync_and_and_fetch (type *ptr, type value, ...)
 * type __sync_xor_and_fetch (type *ptr, type value, ...)
 * type __sync_nand_and_fetch (type *ptr, type value, ...)
 * bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval, ...)
 * type __sync_val_compare_and_swap (type *ptr, type oldval type newval, ...)
 * __sync_synchronize (...) // Full memory barrier
 * type __sync_lock_test_and_set (type *ptr, type value, ...) // Acquire barrier
 * void __sync_lock_release (type *ptr, ...) // Release barrier
 *
 * These are all overloaded and take 1,2,4,8 byte scalar or pointer types.
 *
 * The above aren't available in Mingw32 as of gcc 4.5.2 so define our own.
 */
#if defined(__cplusplus)
# define PTW32_TO_VLONG64PTR(ptr) reinterpret_cast<volatile LONG64 *>(ptr)
#else
# define PTW32_TO_VLONG64PTR(ptr) (ptr)
#endif

#if defined(__GNUC__)
# if defined(_WIN64)
# define PTW32_INTERLOCKED_COMPARE_EXCHANGE_64(location, value, comparand) \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "cmpxchgq      %2,(%1)"                                            \
        :"=a" (_result)                                                    \
        :"r"  (location), "r" (value), "a" (comparand)                     \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_EXCHANGE_64(location, value)                    \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "xchgq	 %0,(%1)"                                                  \
        :"=r" (_result)                                                    \
        :"r" (location), "0" (value)                                       \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_EXCHANGE_ADD_64(location, value)                \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddq	 %0,(%1)"                                                  \
        :"=r" (_result)                                                    \
        :"r" (location), "0" (value)                                       \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_INCREMENT_64(location)                          \
    ({                                                                     \
      PTW32_INTERLOCKED_LONG _temp = 1;                                    \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddq	 %0,(%1)"                                                  \
        :"+r" (_temp)                                                      \
        :"r" (location)                                                    \
        :"memory", "cc");                                                  \
      ++_temp;                                                             \
    })
# define PTW32_INTERLOCKED_DECREMENT_64(location)                          \
    ({                                                                     \
      PTW32_INTERLOCKED_LONG _temp = -1;                                   \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddq	 %2,(%1)"                                                  \
        :"+r" (_temp)                                                      \
        :"r" (location)                                                    \
        :"memory", "cc");                                                  \
      --_temp;                                                             \
    })
#endif
# define PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG(location, value, comparand) \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "cmpxchgl       %2,(%1)"                                           \
        :"=a" (_result)                                                    \
        :"r"  (location), "r" (value), "a" (comparand)                     \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_EXCHANGE_LONG(location, value)                  \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "xchgl	 %0,(%1)"                                                  \
        :"=r" (_result)                                                    \
        :"r" (location), "0" (value)                                       \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_EXCHANGE_ADD_LONG(location, value)              \
    ({                                                                     \
      __typeof (value) _result;                                            \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddl	 %0,(%1)"                                                  \
        :"=r" (_result)                                                    \
        :"r" (location), "0" (value)                                       \
        :"memory", "cc");                                                  \
      _result;                                                             \
    })
# define PTW32_INTERLOCKED_INCREMENT_LONG(location)                        \
    ({                                                                     \
      PTW32_INTERLOCKED_LONG _temp = 1;                                    \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddl	 %0,(%1)"                                                  \
        :"+r" (_temp)                                                      \
        :"r" (location)                                                    \
        :"memory", "cc");                                                  \
      ++_temp;                                                             \
    })
# define PTW32_INTERLOCKED_DECREMENT_LONG(location)                        \
    ({                                                                     \
      PTW32_INTERLOCKED_LONG _temp = -1;                                   \
      __asm__ __volatile__                                                 \
      (                                                                    \
        "lock\n\t"                                                         \
        "xaddl	 %0,(%1)"                                                  \
        :"+r" (_temp)                                                      \
        :"r" (location)                                                    \
        :"memory", "cc");                                                  \
      --_temp;                                                             \
    })
# define PTW32_INTERLOCKED_COMPARE_EXCHANGE_PTR(location, value, comparand) \
    PTW32_INTERLOCKED_COMPARE_EXCHANGE_SIZE((PTW32_INTERLOCKED_SIZEPTR)location, \
                                            (PTW32_INTERLOCKED_SIZE)value, \
                                            (PTW32_INTERLOCKED_SIZE)comparand)
# define PTW32_INTERLOCKED_EXCHANGE_PTR(location, value) \
    PTW32_INTERLOCKED_EXCHANGE_SIZE((PTW32_INTERLOCKED_SIZEPTR)location, \
                                    (PTW32_INTERLOCKED_SIZE)value)
#else
# if defined(_WIN64)
#   define PTW32_INTERLOCKED_COMPARE_EXCHANGE_64(p,v,c) InterlockedCompareExchange64(PTW32_TO_VLONG64PTR(p),(v),(c))
#   define PTW32_INTERLOCKED_EXCHANGE_64(p,v) InterlockedExchange64(PTW32_TO_VLONG64PTR(p),(v))
#   define PTW32_INTERLOCKED_EXCHANGE_ADD_64(p,v) InterlockedExchangeAdd64(PTW32_TO_VLONG64PTR(p),(v))
#   define PTW32_INTERLOCKED_INCREMENT_64(p) InterlockedIncrement64(PTW32_TO_VLONG64PTR(p))
#   define PTW32_INTERLOCKED_DECREMENT_64(p) InterlockedDecrement64(PTW32_TO_VLONG64PTR(p))
# endif
# if defined(PTW32_CONFIG_MSVC6) && !defined(_WIN64)
#  define PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG(location, value, comparand) \
      ((LONG)InterlockedCompareExchange((PVOID *)(location), (PVOID)(value), (PVOID)(comparand)))
# else
#  define PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG InterlockedCompareExchange
# endif
# define PTW32_INTERLOCKED_EXCHANGE_LONG(p,v) InterlockedExchange((p),(v))
# define PTW32_INTERLOCKED_EXCHANGE_ADD_LONG(p,v) InterlockedExchangeAdd((p),(v))
# define PTW32_INTERLOCKED_INCREMENT_LONG(p) InterlockedIncrement((p))
# define PTW32_INTERLOCKED_DECREMENT_LONG(p) InterlockedDecrement((p))
# if defined(PTW32_CONFIG_MSVC6) && !defined(_WIN64)
#  define PTW32_INTERLOCKED_COMPARE_EXCHANGE_PTR InterlockedCompareExchange
#  define PTW32_INTERLOCKED_EXCHANGE_PTR(location, value) \
    ((PVOID)InterlockedExchange((LPLONG)(location), (LONG)(value)))
# else
#  define PTW32_INTERLOCKED_COMPARE_EXCHANGE_PTR(p,v,c) InterlockedCompareExchangePointer((p),(v),(c))
#  define PTW32_INTERLOCKED_EXCHANGE_PTR(p,v) InterlockedExchangePointer((p),(v))
# endif
#endif
#if defined(_WIN64)
#   define PTW32_INTERLOCKED_COMPARE_EXCHANGE_SIZE(p,v,c) PTW32_INTERLOCKED_COMPARE_EXCHANGE_64(PTW32_TO_VLONG64PTR(p),(v),(c))
#   define PTW32_INTERLOCKED_EXCHANGE_SIZE(p,v) PTW32_INTERLOCKED_EXCHANGE_64(PTW32_TO_VLONG64PTR(p),(v))
#   define PTW32_INTERLOCKED_EXCHANGE_ADD_SIZE(p,v) PTW32_INTERLOCKED_EXCHANGE_ADD_64(PTW32_TO_VLONG64PTR(p),(v))
#   define PTW32_INTERLOCKED_INCREMENT_SIZE(p) PTW32_INTERLOCKED_INCREMENT_64(PTW32_TO_VLONG64PTR(p))
#   define PTW32_INTERLOCKED_DECREMENT_SIZE(p) PTW32_INTERLOCKED_DECREMENT_64(PTW32_TO_VLONG64PTR(p))
#else
#   define PTW32_INTERLOCKED_COMPARE_EXCHANGE_SIZE(p,v,c) PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG((p),(v),(c))
#   define PTW32_INTERLOCKED_EXCHANGE_SIZE(p,v) PTW32_INTERLOCKED_EXCHANGE_LONG((p),(v))
#   define PTW32_INTERLOCKED_EXCHANGE_ADD_SIZE(p,v) PTW32_INTERLOCKED_EXCHANGE_ADD_LONG((p),(v))
#   define PTW32_INTERLOCKED_INCREMENT_SIZE(p) PTW32_INTERLOCKED_INCREMENT_LONG((p))
#   define PTW32_INTERLOCKED_DECREMENT_SIZE(p) PTW32_INTERLOCKED_DECREMENT_LONG((p))
#endif

#if defined(NEED_CREATETHREAD)

/*
 * Macro uses args so we can cast start_proc to LPTHREAD_START_ROUTINE
 * in order to avoid warnings because of return type
 */

#define _beginthreadex(security, \
                       stack_size, \
                       start_proc, \
                       arg, \
                       flags, \
                       pid) \
        CreateThread(security, \
                     stack_size, \
                     (LPTHREAD_START_ROUTINE) start_proc, \
                     arg, \
                     flags, \
                     pid)

#define _endthreadex ExitThread

#endif				/* NEED_CREATETHREAD */


#endif				/* _IMPLEMENT_H */
