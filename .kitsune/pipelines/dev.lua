local pipelines = require("kitsune.pipelines");

local pipeline = pipelines.new(
    "Dev pipeline"
)

pipeline:task(
    "build",
    {
        command = function(executor, inputs)
            executor:shell("make -j $(nproc)")
        end
    }
)
pipeline:task(
    "test",
    {
        command = function(executor, inputs)
            executor:shell("make -j $(nproc) test")
        end,
        depends = { "build" },
        env = {
            test = "value",
            test2 = function() return "trans rights" end
        }
    }
)
pipeline:task(
    "run",
    {
        command = function(executor, inputs)
            executor:command({"./bin/kitsune"})
        end,
        run = Run.Auto,
    }
)
