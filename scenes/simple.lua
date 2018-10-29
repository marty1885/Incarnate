function setup()
	local up = float3.new(0, 1, 0)

	incLookAt(float3.new(0,0,1),
		float3.new(0,0,0),
		float3.new(0,1,0))
	incSetWorldUp(up)
	incSetNumTasks(8)

	local verts = {float3.new(-1,-0.5,0), float3.new(0, 0.5, 0), float3.new(1,-0.5,0)}
	local indices = {0,1,2}
	incMesh(verts, indices)
end

t = 0

function update()
	-- incClearFrame()
	t = t + 0.01
	incLookAt(float3.new(t,0,1),
		float3.new(t,0,0),
		float3.new(0,1,0))
	incClearFrame()
end
