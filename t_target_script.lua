-- Create a new alias
function targetAlias()
    -- Create the alias that triggers on "t <mob name>"
    tempAlias = tempAlias or {}
    if tempAlias.targetAlias then
        killAlias(tempAlias.targetAlias)
    end
    
    tempAlias.targetAlias = tempRegexTrigger("^t (.+)$", 
        function(_, name)
            -- Save the target name to a variable
            target = name
            
            -- Send "CON <mob name>"
            send("CON " .. name)
            
            -- Send "gt TARGET=<mob name>"
            send("gt TARGET=" .. name)
            
            -- Optional: Echo confirmation to your screen
            echo("\n\r")
            cecho("<yellow>Target set to: <white>" .. name .. "\n\r")
        end
    )
end

-- Run the function to create the alias
targetAlias()