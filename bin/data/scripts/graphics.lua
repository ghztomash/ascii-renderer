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
    for i = 0, 200 do
        of.setColor(of.random(0, 255), of.random(0, 255), of.random(0, 255))
        of.drawRectangle(of.random(-150, 150), of.random(-150, 150),
            of.random(10, 20), of.random(10, 20))
    end
end
