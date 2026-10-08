---@meta

--- Meta-class primarily used in the command callback
--- @class Executor
Executor = {}

--- Tells kitsune to run a command in a shell. Defaults to using the default shell, but this can be overridden with the
--- second arg.
--- @param command string The command to run
--- @param shell? string The shell to run in. System default shell if undefined
function Executor:shell(command, shell) end

--- Tells kitsune to run a command directly, bypassing shell evaluation. This is the less error-prone option, though at
--- the expense of removing a lot of the conveniences of having a shell.
--- @param command table<string> The command to execute
function Executor:command(command) end

-- TODO: do we want inputs to be table<string, table<string>> to better allow for raw command line inputs in :command
-- execution?
---@alias CommandCallback fun(executor: Executor, inputs: table<string, string>)
---@alias LivenessProbe fun(): boolean
---@alias LoadEnvVar fun(): string
---@alias OnLive fun()
---@alias OnTerminated fun(success: boolean)
---@alias RerunFunc fun(): boolean

-- TODO: sync values with the actual enum
---@enum Run
local Run = {
    Auto = 0,
    OnDemand = 1,
    DependsOnly = 2
}

---@enum Rerun
local Rerun = {
    Always = 0,
    IfCompleted = 1,
    Never = 2,
}

---@enum Notify
local Notify = {
    OnError = 0,
    OnExit = 1
}

---@class PipelineHooks
---@field onLive OnLive? Called when the task is live (liveness-probe returns true for the first time)
---@field onTerminated OnTerminated? Called when the task is terminated
local PipelineHooks = {}

---@class Task
---@field name? string Optional display name. Uses the ID if not specified
---@field command CommandCallback
---@field depends? table<string> The dependencies this task has, if any
---@field run? Run When to run the command. Defaults to OnDemand
---@field rerun? Rerun | RerunFunc Whether or not to rerun the command
---@field liveness? LivenessProbe Whether the process has come up yet. Required by non-terminating processes, otherwise optional.
---@field env? table<string, string | LoadEnvVar> Extra environment variables to set
---@field workingDirectory? string Used to override the working directory. Defaults to the kitsune process' cwd
---@field inputs? table<string, string> Used to specify inputs to the task. Inputs are always optional
---@field notify? Notify Whether or not to notify on exit or similar
---@field hooks? PipelineHooks
local Task = {}

--- Represents a pipeline.
--- @class Pipeline
Pipeline = {}

--- Creates a new task
--- @param id string The task ID. Must be unique in the pipeline
--- @param params Task
function Pipeline:task(id, params) end

pipelines = {}

--- @param name string The name of the pipeline
--- @return Pipeline
function pipelines.new(name) end

return pipelines
