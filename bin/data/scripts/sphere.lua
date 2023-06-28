require("common.variables")
name = "sphere"

----------------------------------------------------
function setup()
end

----------------------------------------------------
function update()
    of.setSphereResolution(resolution)
end

----------------------------------------------------
function draw()
    -- sphere
    of.drawSphere(dimensions.x)
end
