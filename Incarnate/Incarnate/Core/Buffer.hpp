#pragma once

#include <vector>
#include <memory>
#include <iostream>

#include <Incarnate/Core/ComputeAPI.hpp>

namespace Inc
{

template <typename T>
class Buffer
{
public:
	Buffer() = default;
	Buffer(size_t num_elements) : host_data_(num_elements)
	{}

	Buffer(size_t num_elements, std::unique_ptr<DeviceBufferImpl> device_data) : host_data_(num_elements), device_data_(std::move(device_data))
	{}

	size_t size() const
	{
		return host_data_.size();
	}

	void reallocate(size_t size)
	{
		host_data_.resize(size);
	}

	void setDeivceDirty()
	{
		device_dirty_ = true;
	}

	void setHostDirty()
	{
		host_dirty_ = true;
	}

	bool device_dirty() const
	{
		return device_dirty_;
	}

	bool host_dirty() const
	{
		return host_dirty_;
	}

	void sync()
	{
		if(device_dirty() == true && host_dirty() == true)
			std::cerr << "Warrning: Both data on host and device are flaged as dirty. Performing the sync operation is not defined.\n";
	}

	T* data()
	{
		return host_data_.data();
	}

	const T* data() const
	{
		return host_data_.data();
	}

	enum DeviceType
	{
		Host,
		OpenCL,
		Vulkan
	};

	inline T& operator[] (size_t i)
	{
		return host_data_[i];
	}

	inline const T& operator[] (size_t i) const
	{
		return host_data_[i];
	}

protected:
	std::vector<T> host_data_;
	std::unique_ptr<DeviceBufferImpl> device_data_;
	bool host_dirty_ = false;
	bool device_dirty_ = false;
};

}
