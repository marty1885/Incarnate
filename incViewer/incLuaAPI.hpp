#pragma once
#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <queue>
#include <mutex>
#include <memory>
#include <stack>

#include <Incarnate/Incarnate.hpp>

#include <sol.hpp>

class IncLuaAPI
{
	IncLuaAPI()
	{
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

		//setup the basic engines
	}
	sol::state lua;
};
