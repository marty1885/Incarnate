#pragma once

#include <CL/cl.hpp>

#include <Incarnate/Core/Error.hpp>

namespace Inc
{
class DeviceBufferImpl;

class ComputeAPI
{
public:
	virtual DeviceBufferImpl* allocateMemory(size_t num_bytes) = 0;
	virtual void init() = 0;
	virtual void writeToDevice(DeviceBufferImpl* memory, void* data, bool async) = 0;
	virtual void readFromDevie(const DeviceBufferImpl* memory, bool async) = 0;
};

class CPUCompluteAPI
{
public:
};

class OpenCLAPI : public ComputeAPI
{
public:
	OpenCLAPI() = default;
	void init() override
	{
		std::vector<cl::Platform> all_platforms;
		cl::Platform::get(&all_platforms);

		if(all_platforms.size() == 0)
			throw IncError("No OpenCL device found when attempting to initalizing the OpenCL API.");
	}

protected:
	cl::Platform platform_;
	cl::Device device_;
	cl::Context context_;
	cl::CommandQueue queue_;
};

class HostAPI : public ComputeAPI
{
public:

};

class DeviceBufferImpl
{
public:
	void read(void* dest_ptr, bool async) {}
	void write(const void* dest_ptr, bool async) {}
protected:
};

class OpenCLBufferImpl : public DeviceBufferImpl
{
public:
protected:
};

}
