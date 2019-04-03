#ifndef __NWM_UNIQUE_VOID_HPP__
#define __NWM_UNIQUE_VOID_HPP__

#include <memory>

namespace nwm
{
	namespace impl
	{
		// See https://stackoverflow.com/a/39288979/2482983
		using unique_void_ptr = std::unique_ptr<void, void(*)(void const*)>;
		template<typename T>
			auto unique_void(T * ptr) -> unique_void_ptr
		{
			return unique_void_ptr(ptr, [](void const * data) {
				 T const * p = static_cast<T const*>(data);
				 delete p;
			});
		}
	};
};

#endif
