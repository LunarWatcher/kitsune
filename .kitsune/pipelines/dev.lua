local pipelines = require("kitsune.pipelines");

local pipeline = pipelines.new(
    "Dev pipeline"
)

print("Good girl :3")
print(pipeline)

pipeline:task(
    "build",
    {
        command = function(executor)
            executor:shell("make -j $(nproc)")
        end
    }
)
pipeline:task(
    "test",
    {
        command = function(executor)
            executor:shell("make -j $(nproc) test")
        end,
        depends = { "build" }
    }
)
pipeline:task(
    "run",
    {
        command = function(executor)
            executor:command({"./bin/kitsune"})
        end,
    }
)
