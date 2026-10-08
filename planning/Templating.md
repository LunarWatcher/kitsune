# Templating

The initial plan for kitsune was to build a templating-based system, inspired somewhat by the setup that umbra has. While writing exploratory docs for templating, however, I realized that this is unnecessary.

Kitsune sources two `.lua` files on boot, both of which have access to the full API, as well as to lua packages installed on the system in general. This complicates and simplifies things.

The main use-case was with `pipelineLookups`, where `pipelineLookups({ "/path/to/somewhere/{{cwd.folder_name}}/.kitsune" })` could be used. The reason umbra doesn't do this is because it doesn't have a lua API, or any other way to script. It takes its input from environment variables, so the env variables have to be able to dynamically adapt as a feature of umbra. With kitsune though, there's nothing preventing `pipelineLookups({ "/path/to/somewhere/" .. getCurrentFolderName() .. "/.kitsune" })` or something to that effect.

This is functionally indentical, but means no template processing is required.

Where this runs into problems is in commands in the pipeline API: the idea was to allow the command strings (or array parts) to have template args to allow dynamic shit based on stuff like inputs. But, what if, `command = function(executor, inputs) ... end`, where `executor:shell("string")` and `executor({ "/usr/bin/bash", "..." })` so the strings can be dynamically built? As I'm writing this, I'm strongly enough convinced that this is better that I'll be rewriting that part as soon as I'm done here.

Scripting powerful, big shock, but is there still a place for templates? I think callbacks can replace quite a lot of stuff, and allow a _lot_ more flexibility, at least with good API design.
