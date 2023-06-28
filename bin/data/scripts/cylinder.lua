require("common.variables")
name = "cylinder"

----------------------------------------------------
function setup()
end

----------------------------------------------------
function update()
    of.setCylinderResolution(resolution,1)
end

----------------------------------------------------
function draw()
    -- cylinder
    of.drawCylinder(dimensions.x, dimensions.y)
end
