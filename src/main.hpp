#pragma once

#ifdef GUPPY
    #include "guppy/System.hpp"
#endif

#ifdef SALMON
    #include "salmon/System.hpp"
#endif

#ifdef QUAIL
    #include "quail/System.hpp"
#endif

#ifdef GROUND
    #include "ground/System.hpp"
#endif

extern System sys;