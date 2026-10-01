# Problems with the pipeline design

This doc will likely turn into issues once the obvious immediate research candidates are handled

## Docker commands

One of my biggest use-cases for the pipelines is docker commands. Though it's mostly not applicable, it would be nice if we could use the docker health checks to essentially do

```
docker compose -f whatever/docker-compose.yaml up -d
# On complete: start next task
# On complete: run in same terminal (not sure if this is the right command)
docker compose -f whatever/docker-compose.yaml logs
```

Basically, for each terminal to be reusable for an independent command to be run on exit. This way, we don't need a liveness probe for docker commands, which is especially useful because docker already has this exact functionality.

## Input design

I do really like the disposable aspect of the command inputs. One of the biggest limitations of zellij imo is that there's no way to modify commands without modifying the layout. If I want to run a parameterized command differently, or want to provide an extra env variable to modify a command, that requires exiting zellij, then rerunning all of zellij with the extra env variable. 

I do, however, struggle with seeing where the line should go. _The_ work use-case I have here is a sync command that takes an env variable to control whether the old service or the new service should be in charge of specific data hierarchies. It requires an env variable that makes the input sort of mandatory.

The input system also doesn't account for env vars. I think I could work around this with a collapsible panel that shows both inputs and env vars. I do suspect env vars and inputs need to be a more permanent fixture _somewhere_ in the UI, but I also want to avoid it being very clicky or whatever.

More research is required.

## General command automation

I briefly had a concept doc for what got the temp name "kitsune commands", which is similar to pipelines but with none of the lifecycle stuff. They can be dumped into normal, non-automated terminals.

However, I don't believe these are a standalone feature necessarily. Some of the commands I have in mind are one-off non-lifecycle commands like `docker compose -f ... --profile ... down -v -t 0`. They are more attached to a task tree than anything else. The API needs to be reworked to be object-based, so there's a clear hierarchy owner. That way, `command` can be attached to it instead, and take all the same features, but run in a reused shell terminal. A separate scope command terminal or something would make sense in that regard. 

## Working directories

The main plan is that kitsune will respect the cwd at the time it's spawned, so it's more of a derivative in a terminal than a full standalone entry-point terminal. However, it might still make sense to add working directories to the API. I don't know how to do this. We'd need to define a user-independent project root. Having like a `$GIT_ROOT` available to the `cwd` command might make sense too.

