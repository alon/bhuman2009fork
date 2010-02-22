/**
 * @file Platform/Win32/GTAssert.h
 *
 * This file contains macros for low level debugging.
 *
 * @author Thomas Röfer
 */
#ifndef __GTAssert_h_
#define __GTAssert_h_

#undef ASSERT
#undef VERIFY

#ifdef RELEASE

#define ASSERT(cond) ((void)0)
#define VERIFY(cond) ((void)(cond))

#else //RELEASE

/**
 * ASSERT prints a message if cond is false and NDEBUG is not defined.
 * ASSERT does not evaluate cond if NDEBUG is defined.
 * Note that printf will not work in early stages of the creation of an Aperios
 * process. Therefore, you will not get any feedback in such a case. If you are
 * not sure whether an assertion failure will be reported at a certain position
 * in the code, try ASSERT(false).
 * @param c The condition to be checked.
 */
#define ASSERT(c) if(!(c)) {__asm{int 3};}

/**
 * VERIFY prints a message if cond is false and NDEBUG is not defined.
 * VERIFY does evaluate cond even if NDEBUG is defined.
 * Note that printf will not work in early stages of the creation of an Aperios
 * process. Therefore, you will not get any feedback in such a case. If you are
 * not sure whether an assertion failure will be reported at a certain position
 * in the code, try ASSERT(false).
 * @param c The condition to be checked.
 */
#define VERIFY(c) if(!(c)) {__asm{int 3};}

#endif //RELEASE

#endif //__GTAssert_h_
