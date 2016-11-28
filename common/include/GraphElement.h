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
		Attribute(const std::string& name = "", const std::string& value = "", bool encloseNameInDoubleQuotes = false, bool encloseValueInDoubleQuotes = false)
			: _name(name)
			, _value(value)
			, _encloseNameInDoubleQuotes(encloseNameInDoubleQuotes)
			, _encloseValueInDoubleQuotes(encloseValueInDoubleQuotes)
		{ }

		/** Name of the attribute */
		std::string _name;

		/** Value of the attribute */
		std::string _value;

		/** If true then a writer needs to enclose the name of this attribute between double quotes */
		bool _encloseNameInDoubleQuotes;

		/** If true then a writer needs to enclose the value of this attribute between double quotes */
		bool _encloseValueInDoubleQuotes;
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
	* encloseNameInDoubleQuotes: if true then a writer needs to enclose the name of this attribute between double quotes
	* encloseValueInDoubleQuotes: if true then a writer needs to enclose the value of this attribute between double quotes
	*/
	GraphElement& SetAttribute(const std::string& name, const std::string& value, bool encloseNameInDoubleQuotes, bool encloseValueInDoubleQuotes);

	/** Returns true if this graph element has an attribute with the name as the one given */
	bool HasAttribute(const std::string& attributeName) const;

	/** Returns true if this graph element has some attributes */
	bool HasAttributes() const { return _attributes.GetSize() > 0; }
	
	/**
	* Searches for an attribute with the given name and returns its value
	* If the attribute isn't found, returns the given alternative
	*/
	const std::string& GetAttribute(const std::string& attributeName, const std::string& alternative) const;

	/** Returns the list of the attributes of this graph element */
	const AttributeList& GetAttributes() const { return _attributes; }

	/** Copies the attributes from the source graph elemento into this one */
	void CopyAttributes(const GraphElement& src) { _attributes = src._attributes; }
	
private:
	/** List of all the attributes of the graph element */
	AttributeList _attributes;
};

template<> struct ContainerElementDefaultValue<GraphElement*> { static GraphElement* Value() { return nullptr; } };
template<> struct ContainerElementDefaultValue<GraphElement::Attribute> { static GraphElement::Attribute Value() { return GraphElement::Attribute(); } };