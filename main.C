#include "args.H"
#include "error.H"
#include "search.H"

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
        fprintf(stderr, "\033[31mERROR:\033[0m %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
