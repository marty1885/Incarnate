#include <Incarnate/Accelerator.hpp>
#include <Incarnate/Scene.hpp>
#include <Incarnate/Core/Error.hpp>
#include <Incarnate/Core/Math.hpp>

using namespace Inc;

void EmbreeAccelerator::addMesh(Mesh* mesh, const float4x4& transform_matrix)
{
	RTCGeometry geom = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetGeometryBuildQuality(geom,RTC_BUILD_QUALITY_HIGH);
	rtcSetGeometryTimeStepCount(geom,1);

	float3* vertices = (float3*)rtcSetNewGeometryBuffer(geom,RTC_BUFFER_TYPE_VERTEX,0,RTC_FORMAT_FLOAT3,sizeof(float3),mesh->vertices_.size());
	int* indices = (int*)rtcSetNewGeometryBuffer(geom,RTC_BUFFER_TYPE_INDEX,0,RTC_FORMAT_UINT3,sizeof(int3),mesh->indices_.size()/3);

	for(size_t i=0;i<mesh->indices_.size();i++)
		indices[i] = mesh->indices_[i];
	for(size_t i=0;i<mesh->vertices_.size();i++) {
		float4 v = mesh->vertices_[i];
		v.w = 1.0f;
		vertices[i] = float3(transform_matrix*v);
	}

	rtcCommitGeometry(geom);
	auto id = rtcAttachGeometry(embree_scene_,geom);
	rtcReleaseGeometry(geom);
}

void EmbreeAccelerator::intersect(Buffer<PathState>& paths)
{
	RTCRayHit* embree_rays = new RTCRayHit[paths.size()];
	for(size_t i=0;i<paths.size();i++)
	{
		RTCRayHit &embree_ray = embree_rays[i];
		embree_ray.ray.org_x = paths[i].orig.x;
		embree_ray.ray.org_y = paths[i].orig.y;
		embree_ray.ray.org_z = paths[i].orig.z;

		embree_ray.ray.dir_x = paths[i].dir.x;
		embree_ray.ray.dir_y = paths[i].dir.y;
		embree_ray.ray.dir_z = paths[i].dir.z;

		embree_ray.ray.tnear = 0.0f;
		embree_ray.ray.tfar = FLT_MAX;

		embree_ray.hit.geomID = RTC_INVALID_GEOMETRY_ID;
		embree_ray.hit.primID = RTC_INVALID_GEOMETRY_ID;
		embree_ray.ray.mask = -1;
		embree_ray.ray.time = 0;
	}

	rtcIntersect1M(embree_scene_,&context_,embree_rays,paths.size(),sizeof(RTCRayHit));

	for(size_t i=0;i<paths.size();i++)
	{
		if(embree_rays[i].hit.primID != RTC_INVALID_GEOMETRY_ID)
		{
			PathState& path = paths[i];
			path.t = embree_rays[i].ray.tfar;
			path.mesh_id = embree_rays[i].hit.geomID;
			path.geom_id = embree_rays[i].hit.primID;
			path.tex_u = embree_rays[i].hit.u;
			path.tex_v = embree_rays[i].hit.v;
		}
	}
	delete [] embree_rays;
}
