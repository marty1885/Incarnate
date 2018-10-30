#include <Incarnate/Accelerator.hpp>
#include <Incarnate/Scene.hpp>
#include <Incarnate/Core/Error.hpp>
#include <Incarnate/Core/Math.hpp>

#include <omp.h>

using namespace Inc;

void EmbreeAccelerator::addMesh(Mesh* mesh, const float4x4& transform_matrix, unsigned int mesh_id)
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
	rtcAttachGeometryByID(embree_scene_,geom, mesh_id);
	rtcReleaseGeometry(geom);
}

void EmbreeAccelerator::intersect(Buffer<PathState>& paths)
{
	#pragma omp parallel
	{
		size_t thread_id = omp_get_thread_num();
		size_t start_id = (float)thread_id/omp_get_num_threads()*paths.size();
		size_t end_id = (float)(thread_id+1)/omp_get_num_threads()*paths.size();
		size_t size = end_id - start_id;
		RTCRayHit* embree_rays = new RTCRayHit[size];

		for(size_t i=0;i<size;i++)
		{
			RTCRayHit &embree_ray = embree_rays[i];
			embree_ray.ray.org_x = paths[i+start_id].rayhit.ray.orig.x;
			embree_ray.ray.org_y = paths[i+start_id].rayhit.ray.orig.y;
			embree_ray.ray.org_z = paths[i+start_id].rayhit.ray.orig.z;

			embree_ray.ray.dir_x = paths[i+start_id].rayhit.ray.dir.x;
			embree_ray.ray.dir_y = paths[i+start_id].rayhit.ray.dir.y;
			embree_ray.ray.dir_z = paths[i+start_id].rayhit.ray.dir.z;

			embree_ray.ray.tnear = 0.0f;
			embree_ray.ray.tfar = FLT_MAX;

			embree_ray.hit.geomID = RTC_INVALID_GEOMETRY_ID;
			embree_ray.hit.primID = RTC_INVALID_GEOMETRY_ID;
			embree_ray.ray.mask = -1;
			embree_ray.ray.time = 0;
		}

		rtcIntersect1M(embree_scene_,&context_,embree_rays,size,sizeof(RTCRayHit));

		for(size_t i=0;i<size;i++)
		{
			if(embree_rays[i].hit.primID != RTC_INVALID_GEOMETRY_ID)
			{
				PathState& path = paths[i+start_id];
				path.rayhit.hit.t = embree_rays[i].ray.tfar;
				path.rayhit.hit.mesh_id = embree_rays[i].hit.geomID;
				path.rayhit.hit.geom_id = embree_rays[i].hit.primID;
				path.rayhit.hit.tex_u = embree_rays[i].hit.u;
				path.rayhit.hit.tex_v = embree_rays[i].hit.v;
			}
		}
		delete [] embree_rays;
	}
}
