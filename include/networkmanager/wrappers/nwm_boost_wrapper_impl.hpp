#ifndef __NWM_BOOST_WRAPPER_IMPL_HPP__
#define __NWM_BOOST_WRAPPER_IMPL_HPP__

#include "networkmanager/wrappers/nwm_boost_wrapper.hpp"

template<class TBoostBase>
	nwm::TBoostWrapperCopyable<TBoostBase>::TBoostWrapperCopyable(const TBoostWrapperCopyable<TBoostBase> &o)
{
	this->m_boostBaseObject = impl::unique_void<TBoostBase>(new TBoostBase(o.GetBoostObject()));
}
template<class TBoostBase>
	nwm::TBoostWrapperCopyable<TBoostBase> &nwm::TBoostWrapperCopyable<TBoostBase>::operator=(const TBoostWrapperCopyable<TBoostBase> &other)
{
	return operator=(other.GetBoostObject());
}
template<class TBoostBase>
	nwm::TBoostWrapperCopyable<TBoostBase> &nwm::TBoostWrapperCopyable<TBoostBase>::operator=(const TBoostBase &o)
{
	this->m_boostBaseObject = impl::unique_void<TBoostBase>(new TBoostBase(o));
	return *this;
}

#endif
