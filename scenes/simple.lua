function setup()
	-- tell Incarnate to show the surface normal
	incSetRenderer("normal")

	-- render with 8 threads
	incSetNumTasks(8)
end

function world()
	--setup world and camera
	local up = float3.new(0, 1, 0)

	incLookAt(float3.new(0,2,1),
		float3.new(0,0,0),
		float3.new(0,1,0))
	incSetWorldUp(up)

	-- construct a tetrahedron
	local verts = {float3.new(0,1,0)}
	for i=0,2 do
		local angle = i/3*2*math.pi
		verts[#verts+1] = float3.new(math.cos(angle),0,math.sin(angle))
	end
	local indices = {0,1,2, 0,1,3, 0,2,3, 1,2,3}
	incMesh(verts, indices)
end

t = 0

function update(dt)
	t = t + dt
	local angle = t*0.5
	-- rotate our view port
	incLookAt(float3.new(math.sin(angle),2,math.cos(angle)),
		float3.new(0,0,0),
		float3.new(0,1,0))

	--clear frame after update to render a new frame
	incClearFrame()
end
