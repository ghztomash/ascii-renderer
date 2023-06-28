require("common.variables")
name = "cube"

----------------------------------------------------
function setup()
end

----------------------------------------------------
function update()
    of.setBoxResolution(resolution)
end

----------------------------------------------------
function draw()
    -- cube
    of.drawBox(dimensions.x, dimensions.y, dimensions.z)
end
