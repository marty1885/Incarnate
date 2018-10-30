function setup()
	incSetRenderer("normal")
	incSetNumTasks(8)
end

function world()
	local up = float3.new(0, 1, 0)

	incLookAt(float3.new(0,0,1),
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

function update()
	t = t + 0.01
	incLookAt(float3.new(math.sin(t),1,math.cos(t)),
		float3.new(0,0,0),
		float3.new(0,1,0))
	incClearFrame()
end
