#ifndef __NWM_BOOST_WRAPPER_HPP__
#define __NWM_BOOST_WRAPPER_HPP__

#include "networkmanager/wrappers/nwm_unique_void.hpp"

namespace nwm
{
	template<class TBoostBase>
		class TBoostWrapper
	{
	public:
		using UnderlyingType = TBoostBase;
		TBoostWrapper();
		TBoostWrapper(const TBoostBase &o);
		~TBoostWrapper();

		TBoostBase &GetBoostObject();
		const TBoostBase &GetBoostObject() const;
		const TBoostBase *operator->() const;
		TBoostBase *operator->();
		const TBoostBase &operator*() const;
		TBoostBase &operator*();
	protected:
		impl::unique_void_ptr m_boostBaseObject;
	};
	template<class TBoostBase>
		class TBoostWrapperCopyable
			: public TBoostWrapper<TBoostBase>
	{
	public:
		using TBoostWrapper<TBoostBase>::TBoostWrapper;
		TBoostWrapperCopyable(const TBoostWrapperCopyable<TBoostBase> &o);
		TBoostWrapperCopyable<TBoostBase> &operator=(const TBoostWrapperCopyable<TBoostBase> &other);
		TBoostWrapperCopyable<TBoostBase> &operator=(const TBoostBase &o);
	};
};

template<class TBoostBase>
	nwm::TBoostWrapper<TBoostBase>::TBoostWrapper()
		: m_boostBaseObject{nullptr,[](void const*) {}}
{}
template<class TBoostBase>
	nwm::TBoostWrapper<TBoostBase>::~TBoostWrapper()
{}
		
template<class TBoostBase>
	const TBoostBase &nwm::TBoostWrapper<TBoostBase>::GetBoostObject() const
{
	return const_cast<TBoostWrapper*>(this)->GetBoostObject();
}
template<class TBoostBase>
	TBoostBase &nwm::TBoostWrapper<TBoostBase>::GetBoostObject()
{
	return *static_cast<TBoostBase*>(m_boostBaseObject.get());
}
template<class TBoostBase>
	const TBoostBase *nwm::TBoostWrapper<TBoostBase>::operator->() const
{
	return const_cast<TBoostWrapper*>(this)->operator->();
}
template<class TBoostBase>
	TBoostBase *nwm::TBoostWrapper<TBoostBase>::operator->()
{
	return &GetBoostObject();
}
template<class TBoostBase>
	const TBoostBase &nwm::TBoostWrapper<TBoostBase>::operator*() const
{
	return const_cast<TBoostWrapper*>(this)->operator*();
}
template<class TBoostBase>
	TBoostBase &nwm::TBoostWrapper<TBoostBase>::operator*()
{
	return GetBoostObject();
}

template<class TBoostBase>
	nwm::TBoostWrapperCopyable<TBoostBase>::TBoostWrapperCopyable(const TBoostWrapperCopyable<TBoostBase> &o)
{
	m_boostBaseObject = impl::unique_void<TBoostBase>(new TBoostBase(o.GetBoostObject()));
}
template<class TBoostBase>
	nwm::TBoostWrapperCopyable<TBoostBase> &nwm::TBoostWrapperCopyable<TBoostBase>::operator=(const TBoostWrapperCopyable<TBoostBase> &other)
{
	return operator=(other.GetBoostObject());
}
template<class TBoostBase>
	nwm::TBoostWrapperCopyable<TBoostBase> &nwm::TBoostWrapperCopyable<TBoostBase>::operator=(const TBoostBase &o)
{
	m_boostBaseObject = impl::unique_void<TBoostBase>(new TBoostBase(o));
	return *this;
}

#endif
