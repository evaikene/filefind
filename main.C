#include "args.H"
#include "error.H"
#include "search.H"

#include "fmt/format.h"
#include "fmt/color.h"

#include <stdio.h>

int main(int argc, char ** argv)
{
    // Parse command line arguments
    Args args(argc, argv);
    if (args.exit()) {
        return args.valid() ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    try {
        Search::instance(args).search();
    }
    catch (Error const & e) {
        fmt::println(stderr, "{} {}",
                    fmt::styled("ERROR:", fmt::fg(fmt::color::red)),
                    e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
