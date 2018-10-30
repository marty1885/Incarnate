#include <Incarnate/Renderer.hpp>

using namespace Inc;

void NormalRenderer::render(FrameBuffer& frame_buffer, Scene* scene, const PerspectiveCamera& camera)
{
	camera_[0] = camera;
	//Stage 1, generate all camera rays
	genCameraRays((int2){frame_buffer.width(), frame_buffer.height()}, num_works_);

	accelerator_->intersect(path_states_);

	//Sage 2: fill with normal if hit
	float new_sample_weight = 1.f- (float)(frame_buffer.current_sample_)/(frame_buffer.current_sample_+1);
	#pragma omp parallel for num_threads(num_works_)
	for(size_t i=0;i<path_states_.size();i++) {
		auto& s = path_states_[i];
		float4 new_sample_color(0);
		if(s.rayhit.hit.t > 0) {
			float3 normal = float3(tirangle_normal(scene->meshes()[s.rayhit.hit.mesh_id]->triangle(s.rayhit.hit.geom_id)));
			new_sample_color = float4(abs(normal), 0);
		}
		else
			s.phase = PathPhase::Done;
		frame_buffer[i] = new_sample_weight*new_sample_color + (1.f-new_sample_weight)*frame_buffer[i];
	}
	frame_buffer.current_sample_ += 1;
}