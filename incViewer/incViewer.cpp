#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <queue>
#include <mutex>
#include <memory>
#include <stack>

#include <SFML/Graphics.hpp>

#include <Incarnate/Incarnate.hpp>

#include "GLSLSource.hpp"
#include "Shader.hpp"
#include "Surface.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

#include <sol.hpp>

using namespace Inc;

//Global variables for store states
int window_width = 1280;
int window_height = 720;
float3 world_up_vector = float3(0,1,0);
size_t num_tasks = 1;

//Renderer stuff
PerspectiveCamera camera;
FrameBuffer render_buffer;
std::unique_ptr<Renderer> renderer;
Scene scene;
std::unique_ptr<Accelerator> accelerator;
std::string renderer_type = "normal";

//Data managment stuff
std::vector<std::unique_ptr<Mesh>> meshes;
std::vector<std::unique_ptr<SceneNode>> scene_nodes;
SceneNode* scene_root_node;
SceneNode* current_node;
std::stack<SceneNode*> node_stack;


//Application stuff
std::atomic<bool> frame_updated;
std::atomic<float> frame_time = -1;
std::queue<sf::Event> sfml_events;
std::mutex event_access_lock;
std::atomic<bool> frame_clear_request;
std::map<Mesh const*, size_t> mesh_id_map;
std::vector<float4> frame_data;

sol::state lua;

void renderLoop(sf::Window* window)
{
	using namespace std::chrono;

	auto start = high_resolution_clock::now();
	while(window->isOpen()) {
		event_access_lock.lock();
		while(sfml_events.empty() == false) {
			sf::Event event = sfml_events.back();
			sfml_events.pop();

			if(event.type == sf::Event::Resized) {
				int window_width = event.size.width;
				int window_height = event.size.height;

				camera.aspect_ratio = (float)window_width/window_height;

				render_buffer.create(window_width, window_height);
				render_buffer.clear();
				renderer->setup(render_buffer, num_tasks);
				frame_data.resize(window_width*window_height);
			}
		}
		event_access_lock.unlock();

		renderer->render(render_buffer, &scene, camera);
		for(int i=0;i<render_buffer.width()*render_buffer.height();i++)
			frame_data[i] = render_buffer.constBuffer()[i];
		frame_updated = true;

		auto end = high_resolution_clock::now();
		frame_time = duration_cast<duration<double>>(end - start).count();

		//Update scene
		std::function<void(double)> update = lua["update"];
		update(frame_time);
		if(frame_clear_request == true) {
			frame_clear_request = false;
			render_buffer.clear();
		}
		start = end;
	}
}

void incMainLoop()
{
	using namespace std::chrono;

	//Create SFML Window with OpenGL 3.3
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 1;
	settings.majorVersion = 3;
	settings.minorVersion = 3;

	sf::RenderWindow window;
	window.create(sf::VideoMode(window_width, window_height), "Incarnate Viewer", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);

	//Setup OpenGL with the extensions
	GLuint err = glewInit();
	if(err != GLEW_OK)
		throw IncError("GLEW init failed");

	Shader shader;
	bool success = true;
	success &= shader.addVertexShader(Window::basic2DVertexShaderSource);
	success &= shader.addFragmentShader(Window::gammaCorrectionFragmantShaderSource);
	success &= shader.compile();
	if(success == false)
		throw IncError("failed to compile shader");
	shader.bind();
	shader.setParameter("gamma",2.2f);

	Surface surface;

	//Setup renderer
	scene.add(scene_root_node);
	scene.commit();
	render_buffer.create(window_width, window_height);
	frame_data.resize(window_width*window_height);
	renderer->setup(render_buffer, num_tasks);
	camera.aspect_ratio = (float)window_width/window_height;

	//Use a seprate thread rendering the frames to maintain high GUI FPS
	std::thread render_thread(renderLoop, &window);

	sf::Clock deltaClock;
	auto start = high_resolution_clock::now();
	while (window.isOpen()) {
		sf::Event event;

		//SFML event loop
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if(event.type == sf::Event::Resized) {
				window_width = event.size.width;
				window_height = event.size.height;

				window.setView(sf::View(sf::FloatRect(0, 0, window_width, window_height)));
			}

			//Send event to the render thread
			event_access_lock.lock();
			sfml_events.push(event);
			event_access_lock.unlock();

			ImGui::SFML::ProcessEvent(event);
		}

		//Update GUI
		ImGui::SFML::Update(window, deltaClock.restart());

		window.clear();

		window.pushGLStates();
			//Update if frame is updated
			if(frame_updated == true) {
				const float* raw_frame = (const float*)frame_data.data();
				surface.setTexture(raw_frame, render_buffer.width(), render_buffer.height());
				frame_updated = false;
			}

			//Draw the frame
			shader.bind();
			surface.render();
		window.popGLStates();

		//Draw the GUI elements
		ImGui::Begin("System Info");
		ImGui::Text(("Ray Tracing FPS: " + std::to_string(1.f/frame_time)).c_str());

		auto end = high_resolution_clock::now();
		float time_diff = duration_cast<duration<double>>(end - start).count();
		ImGui::Text(("GUI FPS: " + std::to_string(1.f/time_diff)).c_str());
		start = end;

		ImGui::End();
		ImGui::SFML::Render(window);
		window.display();
	}
	render_thread.join();
}

void incSetWorldUp(float3 up)
{
	world_up_vector = up;
}

void incLookAt(float3 pos, float3 target, float3 up)
{
	float3 dir = normalize(target - pos);

	camera.pos = float4(pos, 0);
	float3 right = normalize(cross(dir, world_up_vector));
	camera.up = float4(normalize(cross(right, dir)), 0);
	camera.dir = float4(dir, 0);
	camera.right = float4(right, 0);
}

int incMesh(std::vector<float3> vertices, std::vector<unsigned int> indices)
{
	//Mesh data sanity check
	for(auto index : indices) {
		if(index >= vertices.size())
			throw IncError("Mesh indices " + std::to_string(index) + " is larger then the number of vertices");
	}

	std::vector<float4> verts(vertices.size());
	for(size_t i=0;i<vertices.size();i++)
		verts[i] = float4(vertices[i], 0);

	auto mesh = std::make_unique<Mesh>();
	mesh->setVertices(&verts[0], verts.size());
	mesh->setIndices(indices.data(), indices.size());
	auto mesh_node = std::make_unique<SceneMeshNode>(mesh.get());
	current_node->addChild(mesh_node.get());

	int id = mesh_id_map.size();
	mesh_id_map[mesh.get()] = id;

	meshes.push_back(std::move(mesh));
	scene_nodes.push_back(std::move(mesh_node));

	return id;
}

void incSetNumTasks(size_t num)
{
	num_tasks = num;
}

size_t incNumTasks()
{
	return num_tasks;
}

void incSetRenderer(std::string type)
{
	renderer_type = type;
}

void incSetup()
{
	if(renderer_type == "normal")
		renderer = std::make_unique<NormalRenderer>();
	else
		throw IncError("Renderer type " + renderer_type + "is not supported.");
	auto node = std::make_unique<SceneNode>();
	scene_root_node = node.get();
	scene_nodes.push_back(std::move(node));
	node_stack.push(scene_root_node);
	current_node = scene_root_node;

	accelerator = std::make_unique<EmbreeAccelerator>();
	scene.setAccelerator(accelerator.get());
	renderer->setAccelerator(accelerator.get());
}

void incClearFrame()
{
	frame_clear_request = true;
}

int main(int argc, char** argv)
{
	if(argc == 1) {
		std::cout << "Usage: incView <scene script path>\n";
		return 0;
	}

	//Initalize the Lua engine. Lua is used for easy scripting/setting up the scene
	//Better than using the old PBRTv2 format... LOL
	lua.open_libraries(sol::lib::base, sol::lib::math);
	lua.new_usertype<float3>( "float3",
		sol::constructors<float3(), float3(float), float3(float, float, float)>(),
		"x", &float3::x,
		"y", &float3::y,
		"z", &float3::z);
	lua["incLookAt"] = incLookAt;
	lua["incSetWorldUp"] = incSetWorldUp;
	lua["incSetNumTasks"] = incSetNumTasks;
	lua["incNumTasks"] = incNumTasks;
	lua["incMesh"] = incMesh;
	lua["_INC_VERSION"] = "0.0.1 prealpha";
	lua["incClearFrame"] = incClearFrame;
	lua["incSetRenderer"] = incSetRenderer;

	//Execute the script (set up the scene)
	lua.script_file(argv[1]);

	lua.script("setup()");
	incSetup();
	lua.script("world()");

	incMainLoop();

}
