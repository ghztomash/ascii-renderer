require("common.variables")
name = "circle"

----------------------------------------------------
function setup()
end

----------------------------------------------------
function update()
    of.setCircleResolution(resolution)
end

----------------------------------------------------
function draw()
    -- circle
    of.drawCircle(0, 0, dimensions.x/2.0)
end
