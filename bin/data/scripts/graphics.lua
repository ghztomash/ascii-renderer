----------------------------------------------------
require("common.variables")
name = "graphics"

----------------------------------------------------
function setup()
    print("lua script setup")
end

----------------------------------------------------
function update()
end

----------------------------------------------------
function draw()
    -- RECTANGLES
    local c = of.Color(color.r, color.g, color.b, color.a)

    for i = 0, particles do
        c:setBrightness(of.random(32, 255))
        c:setSaturation(of.random(32, 255))
        of.setColor(c)
        of.drawRectangle(of.random(-dimensions.z, dimensions.z), of.random(-dimensions.z, dimensions.z),
            of.random(10, dimensions.x), of.random(10, dimensions.y))
    end
end
