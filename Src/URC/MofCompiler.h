/**
* @file MofCompiler.h
*
* This file declares a single function to compile the motion net for special actions.
*
* @author Uwe Düffert
* @author Martin Lötzsch
*/

#ifndef __MofCompiler_h__
#define __MofCompiler_h__

/**
* The function compiles all mofs.
* @param buffer A buffer that receives any error message output. It may contain
*               several lines separated by \n.
* @param size The length of the buffer.
* @return Success of compilation.
*/
bool compileMofs(char* buffer, size_t size);
bool convertMofs(char* buffer, size_t size);

#endif // __MofCompiler_h__
