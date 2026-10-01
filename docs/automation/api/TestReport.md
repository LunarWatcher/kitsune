---
{
    "title": "TestReport"
}
---
# TestReport

Dev notes: this struct is based on https://codeberg.org/LunarWatcher/umbra/commit/a32ce38cd3f7632ce322bfa9aa4bf557a0d87cc5, which has associated code that needs to be ported

`TestReport` is an object with the following schema:
```lua
{ -- List
  { -- TestCase object
    name = "Test case name",
    -- A class name associated with the test. May be null for non-OOP test setups
    className = "string | nil",
    -- Legal values: Passed, Skipped, Failed
    overallResult = TestResult.Passed,
    runs = { -- list
      { -- TestRun
        runNumber = 1,
        extraNames = [ "Additional test names. usually present with `describe`-style tests (or nested sections in Catch2)" ],
        stdOut = "string | nil",
        stdErr = "string | nil",
        durationSecs = 12.34,
        result = TestResult.Passed, -- As TestCase.overallResult
        -- Message string, usually present on failure
        message = "string | nil",

        -- Results for the failed assertion. This may be one of:
        -- - nil: no failure or no data could be parsed
        -- - string: a raw assertion error message
        -- - object (see below): intended for catch2 where there's two states (the raw expression, and the expression with values substituted)
        failedAssertion = { expression = "str", expanded = "str" }
      }
    }
  },
}
```
