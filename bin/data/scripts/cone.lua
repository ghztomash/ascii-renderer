require("common.variables")
name = "cone"

----------------------------------------------------
function setup()
end

----------------------------------------------------
function update()
    of.setConeResolution(resolution,1,1)
end

----------------------------------------------------
function draw()
    -- cone
    of.drawCone(dimensions.x, dimensions.y)
end
