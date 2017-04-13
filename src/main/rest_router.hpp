/**
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Oscar Asterkrans
 * All rights reserved.
 */

#pragma once

namespace Net {
    namespace Rest {
        class Router;
    }
}

// Register the handlers for /start and /move requests from server.
// Called from main.
Net::Rest::Router get_router();

