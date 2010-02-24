/* -*- C++ -*- Tell emacs this is a C++ file (despite it's .in extension) */

/**
 * @author Jerome Monceaux
 * Aldebaran Robotics (c) 2007 All Rights Reserved - This file is confidential.
 *
 * Version : $Id$
 */

#ifndef CONFIG_CORE_H
#define CONFIG_CORE_H

// .:: System Headers ::

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: Typedefs :::..

// ::::::::::::::::::::::::::::::::::::::::::::::::::::: Platform variables :::..
# define TARGET_HOST_UNKNOWN    0
# define TARGET_HOST_LINUX      1
# define TARGET_HOST_WINDOWS    2
# define TARGET_HOST_MACOSX     3

# define CPACK_PACKAGE_VERSION_MAJOR 1
# define CPACK_PACKAGE_VERSION_MINOR 3
# define CPACK_PACKAGE_VERSION_PATCH 13
# define CPACK_BUILD_TYPE "academics"


# define TARGET_HOST TARGET_HOST_LINUX

# if TARGET_HOST == TARGET_HOST_UNKNOWN
#   error "Target host not supported, or badly detected !"
# endif // TARGET_HOST == TARGET_HOST_UNKNOWN

// :::::::::::::::::::::::::::::::::::::::::::::::::::::: Options variables :::..

#endif // !CONFIG_H

