# Contribution guidelines

## Basic guidelines

### Use of generative AI is banned

Generative AI uses training data [based on plagiarism and piracy](https://web.archive.org/web/20250000000000*/https://www.theatlantic.com/technology/archive/2025/03/libgen-meta-openai/682093/), has [significant environmental costs associated with it](https://doi.org/10.21428/e4baedd9.9070dfe7), and [generates fundamentally insecure code](https://doi.org/10.1007/s10664-024-10590-1). GenAI is not ethically built, ethical to use, nor safe to use for programming applications. When caught, you will be permanently banned from contributing to the project, and any prior contributions will be checked and potentially reverted. Any and all contributions you've made cannot be trusted if AI slop machines were involved.

## Development setup

First, you'll need the same dependencies that are listed in the build guide (`docs/build.md`). There are no special dependencies beyond this.

Run `./scripts/setup-dev.sh`. This ensures you get the pre-commit hook loaded in. This is useful for avoiding some problems with lua includes, which clangd _really_ likes to incorrectly import. Please beware that you have to include `lua.hpp` somewhere in the file yourself if you're using lua APIs. The pre-commit hook automagically removes any incorrect C header imports. Incorrectly using C headers can cause the entire program to stop building, because the C symbols from the system lua library are unable to link against the mangled names the C++ compiler produces with the C headers.

## Logging

Although GTK provides a logger, kitsune uses `minilog` instead. This is exclusively because error logging with `g_log` throws an exception, which defeats the entire purpose of error logging.

Error logging exists to indicate a non-critical error. If you have a critical error that requires the immediate termination of the GUI, that should be an `abort()`, or possibly an exception. If, however, it comes to this, you have done something wrong and need to reconsider your design choices.

Because we're operating on user-provided lua scripts, there's a 100% chance of errors occuring, and that we want to log as errors, but without terminating the entire application because of it. The error logging will come in addition to user-facing GUI error handling at some point, but because kitsune is a hybrid app, using the terminal is appropriate.

