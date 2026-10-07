local pipelines = require("kitsune.pipelines");

local pipeline = pipelines.new(
    "Dev pipeline"
)

print("Good girl :3")
print(pipeline)

pipeline:createTask(
    "build",
    {

    }
);
