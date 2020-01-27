#pragma once

#ifdef GUPPY
    #include "board/guppy/System.hpp"
#endif

#ifdef SALMON
    #include "board/salmon/System.hpp"
#endif

extern System sys;