/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ANDROID_LOG_H
#define _ANDROID_LOG_H

/******************************************************************
 *
 * IMPORTANT NOTICE:
 *
 *   This file is part of Android's set of stable system headers
 *   exposed by the Android NDK (Native Development Kit) since
 *   platform release 1.5
 *
 *   Third-party source AND binary code relies on the definitions
 *   here to be FROZEN ON ALL UPCOMING PLATFORM RELEASES.
 *
 *   - DO NOT MODIFY ENUMS (EXCEPT IF YOU ADD NEW 32-BIT VALUES)
 *   - DO NOT MODIFY CONSTANTS OR FUNCTIONAL MACROS
 *   - DO NOT CHANGE THE SIGNATURE OF FUNCTIONS IN ANY WAY
 *   - DO NOT CHANGE THE LAYOUT OR SIZE OF STRUCTURES
 */

/*
 * Support routines to send messages to the Android in-kernel log buffer,
 * which can later be accessed through the 'logcat' utility.
 *
 * Each log message must have
 *   - a priority
 *   - a log tag
 *   - some text
 *
 * The tag normally corresponds to the component that emits the log message,
 * and should be reasonably small.
 *
 * Log message text may be truncated to less than an implementation-specific
 * limit (e.g. 1023 characters max).
 *
 * Note that a newline character ("\n") will be appended automatically to your
 * log message, if not already there. It is not possible to send several messages
 * and have them appear on a single line in logcat.
 *
 * PLEASE USE LOGS WITH MODERATION:
 *
 *  - Sending log messages eats CPU and slow down your application and the
 *    system.
 *
 *  - The circular log buffer is pretty small (<64KB), sending many messages
 *    might push off other important log messages from the rest of the system.
 *
 *  - In release builds, only send log messages to account for exceptional
 *    conditions.
 *
 * NOTE: These functions MUST be implemented by /system/lib/liblog.so
 */

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This file uses ", ## __VA_ARGS__" zero-argument token pasting to
 * work around issues with debug-only syntax errors in assertions
 * that are missing format strings.  See commit
 * 19299904343daf191267564fe32e6cd5c165cd42
 */
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#ifndef __predict_false
#define __predict_false(exp) __builtin_expect((exp) != 0, 0)
#endif

/*
 * LOG_TAG is the local tag used for the following simplified
 * logging macros.  You must set this preprocessor definition,
 * or more tenuously supply a variable definition, before using
 * the macros.
 */

/*
 * Normally we strip the effects of ALOGV (VERBOSE messages),
 * LOG_FATAL and LOG_FATAL_IF (FATAL assert messages) from the
 * release builds be defining NDEBUG.  You can modify this (for
 * example with "#define LOG_NDEBUG 0" at the top of your source
 * file) to change that behavior.
 */

#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

/*
 * Android log priority values, in ascending priority order.
 */
#ifndef __android_LogPriority_defined
#define __android_LogPriority_defined
typedef enum android_LogPriority {
    ANDROID_LOG_UNKNOWN = 0,
    ANDROID_LOG_DEFAULT,    /* only for SetMinPriority() */
    ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG,
    ANDROID_LOG_INFO,
    ANDROID_LOG_WARN,
    ANDROID_LOG_ERROR,
    ANDROID_LOG_FATAL,
    ANDROID_LOG_SILENT,     /* only for SetMinPriority(); must be last */
} android_LogPriority;
#endif

/*
 * Send a simple string to the log.
 */
int __android_log_write(int prio, const char* tag, const char* text);

#define android_writeLog(prio, tag, text) \
    __android_log_write(prio, tag, text)

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 */
int __android_log_print(int prio, const char* tag,  const char* fmt, ...)
#if defined(__GNUC__)
#ifdef __USE_MINGW_ANSI_STDIO
#if __USE_MINGW_ANSI_STDIO
    __attribute__ ((format(gnu_printf, 3, 4)))
#else
    __attribute__ ((format(printf, 3, 4)))
#endif
#else
    __attribute__ ((format(printf, 3, 4)))
#endif
#endif
    ;

#define android_printLog(prio, tag, ...) \
    __android_log_print(prio, tag, __VA_ARGS__)

/*
 * Log macro that allows you to specify a number for the priority.
 */
#ifndef LOG_PRI
#define LOG_PRI(priority, tag, ...) \
    android_printLog(priority, tag, __VA_ARGS__)
#endif

/*
 * A variant of __android_log_print() that takes a va_list to list
 * additional parameters.
 */
int __android_log_vprint(int prio, const char* tag,
                         const char* fmt, va_list ap)
#if defined(__GNUC__)
#ifdef __USE_MINGW_ANSI_STDIO
#if __USE_MINGW_ANSI_STDIO
    __attribute__ ((format(gnu_printf, 3, 0)))
#else
    __attribute__ ((format(printf, 3, 0)))
#endif
#else
    __attribute__ ((format(printf, 3, 0)))
#endif
#endif
    ;

#define android_vprintLog(prio, cond, tag, ...) \
    __android_log_vprint(prio, tag, __VA_ARGS__)

/*
 * Log macro that allows you to pass in a varargs ("args" is a va_list).
 */
#ifndef LOG_PRI_VA
#define LOG_PRI_VA(priority, tag, fmt, args) \
    android_vprintLog(priority, NULL, tag, fmt, args)
#endif

/*
 * Log an assertion failure and abort the process to have a chance
 * to inspect it if a debugger is attached. This uses the FATAL priority.
 */
void __android_log_assert(const char* cond, const char* tag,
                          const char* fmt, ...)
#if defined(__GNUC__)
    __attribute__ ((__noreturn__))
#ifdef __USE_MINGW_ANSI_STDIO
#if __USE_MINGW_ANSI_STDIO
    __attribute__ ((format(gnu_printf, 3, 4)))
#else
    __attribute__ ((format(printf, 3, 4)))
#endif
#else
    __attribute__ ((format(printf, 3, 4)))
#endif
#endif
    ;

/* XXX Macros to work around syntax errors in places where format string
 * arg is not passed to ALOG_ASSERT, LOG_ALWAYS_FATAL or LOG_ALWAYS_FATAL_IF
 * (happens only in debug builds).
 */

/* Returns 2nd arg.  Used to substitute default value if caller's vararg list
 * is empty.
 */
#define __android_second(dummy, second, ...)     second

/* If passed multiple args, returns ',' followed by all but 1st arg, otherwise
 * returns nothing.
 */
#define __android_rest(first, ...)               , ## __VA_ARGS__

#define android_printAssert(cond, tag, ...) \
    __android_log_assert(cond, tag, \
        __android_second(0, ## __VA_ARGS__, NULL) __android_rest(__VA_ARGS__))

/*
 * Log a fatal error.  If the given condition fails, this stops program
 * execution like a normal assertion, but also generating the given message.
 * It is NOT stripped from release builds.  Note that the condition test
 * is -inverted- from the normal assert() semantics.
 */
#ifndef LOG_ALWAYS_FATAL_IF
#define LOG_ALWAYS_FATAL_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)android_printAssert(#cond, LOG_TAG, ## __VA_ARGS__)) \
    : (void)0 )
#endif

#ifndef LOG_ALWAYS_FATAL
#define LOG_ALWAYS_FATAL(...) \
    ( ((void)android_printAssert(NULL, LOG_TAG, ## __VA_ARGS__)) )
#endif

/*
 * Versions of LOG_ALWAYS_FATAL_IF and LOG_ALWAYS_FATAL that
 * are stripped out of release builds.
 */

#if LOG_NDEBUG

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, ...) ((void)0)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(...) ((void)0)
#endif

#else

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, ...) LOG_ALWAYS_FATAL_IF(cond, ## __VA_ARGS__)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(...) LOG_ALWAYS_FATAL(__VA_ARGS__)
#endif

#endif

/*
 * Assertion that generates a log message when the assertion fails.
 * Stripped out of release builds.  Uses the current LOG_TAG.
 */
#ifndef ALOG_ASSERT
#define ALOG_ASSERT(cond, ...) LOG_FATAL_IF(!(cond), ## __VA_ARGS__)
#endif

/* --------------------------------------------------------------------- */

/*
 * C/C++ logging functions.  See the logging documentation for API details.
 *
 * We'd like these to be available from C code (in case we import some from
 * somewhere), so this has a C interface.
 *
 * The output will be correct when the log file is shared between multiple
 * threads and/or multiple processes so long as the operating system
 * supports O_APPEND.  These calls have mutex-protected data structures
 * and so are NOT reentrant.  Do not use LOG in a signal handler.
 */

/* --------------------------------------------------------------------- */

/*
 * Simplified macro to send a verbose log message using the current LOG_TAG.
 */
#ifndef ALOGV
#define __ALOGV(...) ((void)ALOG(LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#if LOG_NDEBUG
#define ALOGV(...) do { if (0) { __ALOGV(__VA_ARGS__); } } while (0)
#else
#define ALOGV(...) __ALOGV(__VA_ARGS__)
#endif
#endif

#ifndef ALOGV_IF
#if LOG_NDEBUG
#define ALOGV_IF(cond, ...)   ((void)0)
#else
#define ALOGV_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_VERBOSE, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif
#endif

/*
 * Simplified macro to send a debug log message using the current LOG_TAG.
 */
#ifndef ALOGD
#define ALOGD(...) ((void)ALOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGD_IF
#define ALOGD_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send an info log message using the current LOG_TAG.
 */
#ifndef ALOGI
#define ALOGI(...) ((void)ALOG(LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGI_IF
#define ALOGI_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_INFO, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send a warning log message using the current LOG_TAG.
 */
#ifndef ALOGW
#define ALOGW(...) ((void)ALOG(LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGW_IF
#define ALOGW_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_WARN, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send an error log message using the current LOG_TAG.
 */
#ifndef ALOGE
#define ALOGE(...) ((void)ALOG(LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGE_IF
#define ALOGE_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_ERROR, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/* --------------------------------------------------------------------- */

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * verbose priority.
 */
#ifndef IF_ALOGV
#if LOG_NDEBUG
#define IF_ALOGV() if (false)
#else
#define IF_ALOGV() IF_ALOG(LOG_VERBOSE, LOG_TAG)
#endif
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * debug priority.
 */
#ifndef IF_ALOGD
#define IF_ALOGD() IF_ALOG(LOG_DEBUG, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * info priority.
 */
#ifndef IF_ALOGI
#define IF_ALOGI() IF_ALOG(LOG_INFO, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * warn priority.
 */
#ifndef IF_ALOGW
#define IF_ALOGW() IF_ALOG(LOG_WARN, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * error priority.
 */
#ifndef IF_ALOGE
#define IF_ALOGE() IF_ALOG(LOG_ERROR, LOG_TAG)
#endif

/* --------------------------------------------------------------------- */

/*
 * Basic log message macro.
 *
 * Example:
 *  ALOG(LOG_WARN, NULL, "Failed with error %d", errno);
 *
 * The second argument may be NULL or "" to indicate the "global" tag.
 */
#ifndef ALOG
#define ALOG(priority, tag, ...) \
    LOG_PRI(ANDROID_##priority, tag, __VA_ARGS__)
#endif

/*
 * Conditional given a desired logging priority and tag.
 */
#ifndef IF_ALOG
#define IF_ALOG(priority, tag) \
    if (android_testLog(ANDROID_##priority, tag))
#endif

/* --------------------------------------------------------------------- */

/*
 *    IF_ALOG uses android_testLog, but IF_ALOG can be overridden.
 *    android_testLog will remain constant in its purpose as a wrapper
 *        for Android logging filter policy, and can be subject to
 *        change. It can be reused by the developers that override
 *        IF_ALOG as a convenient means to reimplement their policy
 *        over Android.
 */

#ifndef __ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE
#ifndef __ANDROID_API__
#define __ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE 2
#elif __ANDROID_API__ > 24 /* > Nougat */
#define __ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE 2
#elif __ANDROID_API__ > 22 /* > Lollipop */
#define __ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE 1
#else
#define __ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE 0
#endif
#endif

#if __ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE

/*
 * Use the per-tag properties "log.tag.<tagname>" to generate a runtime
 * result of non-zero to expose a log. prio is ANDROID_LOG_VERBOSE to
 * ANDROID_LOG_FATAL. default_prio if no property. Undefined behavior if
 * any other value.
 */
int __android_log_is_loggable(int prio, const char* tag, int default_prio);

#if __ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE > 1
#include <sys/types.h>

int __android_log_is_loggable_len(int prio, const char* tag, size_t len,
                                  int default_prio);

#if LOG_NDEBUG /* Production */
#define android_testLog(prio, tag) \
    (__android_log_is_loggable_len(prio, tag, (tag && *tag) ? strlen(tag) : 0, \
                                   ANDROID_LOG_DEBUG) != 0)
#else
#define android_testLog(prio, tag) \
    (__android_log_is_loggable_len(prio, tag, (tag && *tag) ? strlen(tag) : 0, \
                                   ANDROID_LOG_VERBOSE) != 0)
#endif

#else

#if LOG_NDEBUG /* Production */
#define android_testLog(prio, tag) \
    (__android_log_is_loggable(prio, tag, ANDROID_LOG_DEBUG) != 0)
#else
#define android_testLog(prio, tag) \
    (__android_log_is_loggable(prio, tag, ANDROID_LOG_VERBOSE) != 0)
#endif

#endif

#else /* __ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE */

#define android_testLog(prio, tag) (1)

#endif /* !__ANDROID_USE_LIBLOG_LOGGABLE_INTERFACE */

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ANDROID_LOG_H */
