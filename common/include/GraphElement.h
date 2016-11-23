#pragma once

#include "List.h"

#include <string>

// Forward declaration of used types
class Graph;

class GraphElement
{
public:
	/** Data structure to store an attribute of the graph element */
	struct Attribute
	{
		/** Default constructor */
		Attribute(const std::string& name = "", const std::string& value = "")
			: _name(name)
			, _value(value)
		{ }

		/** Name of the attribute */
		std::string _name;

		/** Value of the attribute */
		std::string _value;
	};

	typedef List<Attribute> AttributeList;

protected:
	/**
	* Protected constructor since a graph element should never be instantiated
	* but only its derived classes
	*/

	/** Default constructor */
	GraphElement();
	
public:
	/** Copy constructor */
	GraphElement(const GraphElement& src);

	/** Move constructor */
	GraphElement(GraphElement&& src);

	/** Destructor */
	virtual ~GraphElement();

public:
	/** Assign operator */
	GraphElement& operator=(const GraphElement& src);

	/** Move operator */
	GraphElement& operator=(GraphElement&& src);

private:
	/** Utility function to use with copy constructor and assign operator */
	void Copy(const GraphElement& src);

public:
	/**
	* Sets the value of an attribute of the graph element.
	* If the attribute with the given name doesn't exist it is added.
	* name: name of the attribute which the user wants to set
	* value: value of the attribute which the user wants to set
	*/
	GraphElement& SetAttribute(const std::string& name, const std::string& value);

	/** Returns true if this graph element has an attribute with the name as the one given */
	bool HasAttribute(const std::string& attributeName) const;
	
	/**
	* Searches for an attribute with the given name and returns its value
	* If the attribute isn't found, returns the given alternative
	*/
	const std::string& GetAttribute(const std::string& attributeName, const std::string& alternative) const;
	
private:
	/** List of all the attributes of the graph element */
	AttributeList _attributes;
};

template<> struct ListElementDefaultValue<GraphElement::Attribute> { static GraphElement::Attribute Value() { return GraphElement::Attribute(); } };