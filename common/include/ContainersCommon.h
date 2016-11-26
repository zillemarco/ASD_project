#pragma once

#include <type_traits>

/** Default element destructor used by List. It does nothing */
template<typename T> struct ContainerDefaultElementDestructor
{
	typedef T& ReferenceType;
	inline void operator()(ReferenceType element) { }
};

/**
* Utility template to have a default value to use inside of some methods of the container
* The user have to define his own default value doing a template specialization of this struct.
* 
* For example, if the user defines a class A to be used by a List a possible template specialization is:
*	template<> struct ContainerElementDefaultValue<A>
*	{
*		static A Value() { return  A(); }
*	};
*/
template<typename T> struct ContainerElementDefaultValue
{
	//static_assert(false, "A default value wasn't specified for the type.");
};

/** Default value for basic types */
template<> struct ContainerElementDefaultValue<int> { inline static int Value() { return 0; } };
template<> struct ContainerElementDefaultValue<unsigned int> { inline static unsigned int Value() { return 0; } };
template<> struct ContainerElementDefaultValue<long> { inline static long Value() { return 0; } };
template<> struct ContainerElementDefaultValue<unsigned long> { inline static unsigned long Value() { return 0; } };
template<> struct ContainerElementDefaultValue<long long> { inline static long long Value() { return 0; } };
template<> struct ContainerElementDefaultValue<unsigned long long> { inline static unsigned long long Value() { return 0; } };
template<> struct ContainerElementDefaultValue<float> { inline static float Value() { return 0.0f; } };
template<> struct ContainerElementDefaultValue<double> { inline static double Value() { return 0; } };