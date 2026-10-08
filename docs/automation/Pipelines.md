---
{
    "title": "Kitsune pipelines"
}
---
# Kitsune pipelines

Pipelines are the main automation feature in kitsune. Kitsune's pipelines differ from most other uses of the concept, in that they're exclusively intended for local, interactive use.

## Why not a shell script?

There are many good cases for using a shell script instead. If you can, I would suggest doing using a shell script. Shell scripts can be very powerful automation tools, and offer much more flexibility when written correctly. If all you care about is sequential, terminating steps that end in a single terminating or non-terminating job, or where starting individual commands in separate terminals is acceptable, a shell script is almost certainly going to be better. This is alrgely because you don't have a whole extra layer of abstraction between you and standard low-level automation, like you do with kitsune pipelines (or anything that isn't directly shell-based).

Shell scripts do suffer the second you have parallel or non-terminating commands you'd like to execute. This can be made easier by chaining a shell script with an automatable terminal multiplexer (like zellij with layouts), but this is no longer pure shell scripting.

There's also plenty of cases where make or make-emulating shell scripts are better. Make still suffers from most of the same problems around interactivity and log visibility.

## Example

```lua
local pipelines = require("kitsune.pipelines")
-- A file can define multiple pipelines, but the convention is one file per pipeline
local pipeline = pipelines.new("Some recognizable name")
pipeline:task("build", {
    command = { "/usr/bin/zsh" }
}) -- See below for full signature
```

## API

TODO: this is written purely off the top of my head without verifying if the lua is syntactically valid

```lua
task(
  -- The task ID is referred to in other fields, including in `depends` lists. This should be short purely because it'll be annoying to write otherwise
  -- It also must be unique among `task`s
  "task-id", {
    -- Optional display name. If not supplied, the task ID is used directly.
    name = "Display name",
    -- Mutually exclusive: only command, shellCommand, or luaCommand may be supplied

    -- Defines the command to run
    command = function(executor, inputs) 
      -- @param executor - Used to run things in the shell. Tasks and shells are 1:1, so you should only
      -- run one command and return
      -- @param inputs - the inputs to the function (see the `inputs` arg to `task`)
      executor:shell("fastfetch") -- Execute in shell
      executor:shell("fastfetch", "/usr/bin/bash") -- Execute in specific shell
      executor:command({"/usr/bin/local/kitsune", "whatever"}) -- Execute specific directly
    end

    -- Defines whether or not the command has dependencies. Pretty self-explanatory.
    depends = { "other-task-id" },

    -- Defines when to run the command. Valid values are:
    -- - Auto: Similar to OnDemand, but also automatically runs when the whole task tree is spawned.
    -- - OnDemand (default): Only runs when invoked, or when a dependency needs it to run
    -- - DependsOnly: No direct running is allowed. This command only runs as an intermediate
    run = Run.OnDemand,

    -- Similar to run, but defines when to rerun a task. A spawned task tree is reusable,
    -- and not all tasks need to be rerun. This field is ignored if `watch` is defined
    -- Allowed values are:
    -- - Always: Always rerun, even if the task has to be killed first to do it.
    --   Note that unrelated leaf commands are not touched. Only the active tree is touched. If you
    --   need to restart an entire tree, you'll want to combine this with a pseudo-node (non-task node)
    -- - IfCompleted (default): Only rerun if it isn't currently running
    -- - Never: Never rerun for the rest of this session. Manually running the task is still allowed
    -- - <function>: a function that returns `true` if a rerun is necessary. This is only offered as a last resort
    rerun = Rerun.IfCompleted,

    -- A liveness probe. Only required for and only used by non-terminating tasks.
    -- If no liveness probe is provided, it's assumed that the command must terminate.
    -- If you can't provide one, a simple `return true` could work, but beware of race conditions.
    -- Some built-in probes are provided; see the Probes section
    liveness = function () return true end

    -- Extra environment variables
    env = {
      "SOME_ENV_VAR": "static value",
      -- Env variables can also be loaded with functions. Can be useful for stuff requiring auth, but
      -- where the token isn't fixed enough that it can be done in the pipeline preamble.
      "SOME_OTHER_ENV_VAR": function getValue() return "whatever" end
    },

    hooks = {
      -- Called when the service is live. Only called when liveness is a non-nil function
      onLive = function onLive() end
      -- success (boolean): whether or not the process quit with exit code 0 or not
      onTerminated = function onTerminated(success) end
    },

    -- The command's working directory. Defaults to the working directory at the time kitsune was started
    -- Relative working directories are supported. Relative working directories are relative to
    -- kitsune's start directory.
    workingDirectory = "/some/folder"

    -- Inputs can be used to modify commands, provided the `command` makes use of them. 
    -- If an input is defined, `inputs["tests"]` in the `command` can be used to provide additional command line args. 
    inputs = {
      tests = "Specific tests to run"
    },

    -- Whether or not to send desktop notifications. Legal values:
    -- - nil (default): No notifications
    -- - OnError: send notification if the process exits with a non-zero status code
    -- - OnExit: send a notification if the process exits at all
    notify = Notify.OnError,
  })
```

In addition, the following pseudo-task is allowed:
```lua
task("node-id", {
  name = "Some node"
  depends = { "a", "b", "c" }
})
```

This allows several tasks to be run at once without itself being a task. This task does not spawn a terminal, but just sets N tasks to rerun at the same time. This only makes sense if you have multiple `depends`

### Reporters

Reporters are an additional set of API functions that may be used by the hooks to provide additional information in the GUI about the run. The reporters run on structured data or the stdout/stderr to essentially give an abridged version of the stdout.

This is likely only going to be useful for tests.

TODO: how do we get the stdout/stderr into here? It can't exclusively be passed to `onTerminated`, or we'll have lifetime problems (?). Might need to be params to the reportTest callback as well.
... will even that live long enough?? Idfk, I'll figure this out when I get to it in a While:tm:

#### `task:reportTest(() => TestReport)`

`reportTest` takes a callback that provides a [`TestReport`](/automation/api/TestReport.md) (lazily generated if the user requests the test report be generated).


## Probes

The following builtin probes are provided:

### `Probes.HttpProbe("http://localhost:8080"[, expected = 200])`

Params:
* URL (string, required)
* expected (int, optional, default: 200): A status code. This should be 200 unless your server is cursed

Note: SSL certificates are NOT verified for HTTPS connections. There's therefore no way nor a need to specify certificates. It's just a probe


## Example

### Linear pipeline

`.kitsune/pipelines/run.lua`
```lua
task("build", {
  name = "Build", -- Optional, the raw ID is used otherwise
  -- Commands can be standard shell syntax. These run in your default shell, unless otherwise specified.
  shellCommand = "make -j $(nproc)",
  rerun = Rerun.Always,
  -- shell = "/usr/bin/bash"
})
task("test", {
  command = { "/usr/bin/make", "test" },
  rerun = Rerun.Always,
  depends = {"build"}
})
task("run", {
  command = { "./bin/kitsune" },
  depends = {"test"}
})
```

### Complex pipeline

This example is based on the needs of LunarWatcher/obsidian-webdav-sync.

`.kitsune/pipelines/run.sh`
```lua
-- npm run dev is non-terminating
task("build", {
  name = "Build",
  shellCommand = "npm run dev",
  liveness = function() return true end,
  rerun = Rerun.OnError
})
task("test", {
  name = "Test",
  shellCommand = "npm run test",
  rerun = Rerun.Always,
  notify = Notify.OnError,
  depends = { "build" }
})
-- In this case, e2e-test.sh does more heavy lifting that includes linear steps. Shell script
-- mixing with kitsune is strongly suggested.
task("integration-test", {
  name = "Integration tests",
  shellCommand = "./scripts/e2e-test.sh",
  shell = "bash",
  depends = { "test" },
  notify = Notify.OnError,
  run = Run.OnDemand,
})
```

