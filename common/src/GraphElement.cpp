#include "GraphElement.h"

/** Function-object used to find attribute inside the attributes list */
struct AttributeFinder
{
	/**
	* Default constructor
	* name: name of the attribute to look for
	*/
	AttributeFinder(const std::string& name)
		: _name(name)
	{ }

	inline bool operator()(const GraphElement::Attribute& attribute)
	{
		if (attribute._name == _name)
			return true;
		return false;
	}

	/** Name of the attribute to look for */
	std::string _name;
};

/** Default constructor */
GraphElement::GraphElement()
{ }

/** Copy constructor */
GraphElement::GraphElement(const GraphElement& src)
	: _attributes(src._attributes)
{ }

/** Move constructor */
GraphElement::GraphElement(GraphElement&& src)
	: _attributes(std::move(src._attributes))
{ }

/** Destructor */
GraphElement::~GraphElement()
{
	_attributes.Clear();
}

/** Assign operator */
GraphElement& GraphElement::operator=(const GraphElement& src)
{
	if (this != &src)
		Copy(src);
	return *this;
}

/** Move operator */
GraphElement& GraphElement::operator=(GraphElement&& src)
{
	if (this != &src)
	{
		_attributes.Clear();
		_attributes = std::move(src._attributes);
	}

	return *this;
}

/** Utility function to use with copy constructor and assign operator */
void GraphElement::Copy(const GraphElement& src)
{
	_attributes = src._attributes;
}

/**
* Sets the value of an attribute of the graph element.
* If the attribute with the given name doesn't exist it is added.
* name: name of the attribute which the user wants to set
* value: value of the attribute which the user wants to set
*/
GraphElement& GraphElement::SetAttribute(const std::string& name, const std::string& value)
{
	// Find the attribute with the given name
	bool found = false;
	Attribute& attribute = _attributes.FindElement(AttributeFinder(name), found);

	// If the attribute was found change its value
	if (found)
		attribute._value = value;
	// Otherwise add a new attribute to the list with the given name and value
	else
		_attributes.Add(Attribute(name, value));

	return *this;
}

/** Returns true if this graph element has an attribute with the name as the one given */
bool GraphElement::HasAttribute(const std::string& attributeName) const
{
	return _attributes.Find(AttributeFinder(attributeName)) != -1;
}

/**
* Searches for an attribute with the given name and returns its value
* If the attribute isn't found, returns the given alternative
*/
const std::string& GraphElement::GetAttribute(const std::string& attributeName, const std::string& alternative) const
{
	bool found = false;
	const Attribute& attribute = _attributes.FindElement(AttributeFinder(attributeName), found);

	if (found == true)
		return attribute._value;
	return alternative;
}