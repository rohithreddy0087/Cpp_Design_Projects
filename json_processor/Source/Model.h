//
// Created by Mark on 1/30/2024.
//

#pragma once

#include <string>
#include <optional>
#include <map>
#include <unordered_map>
#include <vector>
#include <stack>
#include <variant>
#include <memory>
#include <sstream>
#include <cctype>
#include <algorithm>

#include "JSONParser.h"

namespace ECE141 {

    class ModelNode; // Forward declare

    using object = std::map<std::string, ModelNode>; // alias for map container
    using list = std::vector<ModelNode>; // alias for list container
    using ModelNodeReturnType = std::variant<int, long int, float, double, bool, std::string, std::shared_ptr<object>, std::shared_ptr<list>>; // alias for ModelNode return type

    // Enum class for different ModelNode Types
    enum class NodeType { Null, Int, LongInt, Float, Double, Bool, String, List, Object};

    class ModelQuery; // Forward declare

	class ModelNode {
		// A node holds a basic value (null, bool, number, string), a list of other nodes (list), a collection key-value pairs, where the value is a node (an object)
    public:

        std::optional<ModelNodeReturnType> nodeValue; //stores Node value in the correct datatype format(int, long, float, etc..)
        NodeType nodeType; // stores the datatype
        std::string orgValue; // stores original string value

        ModelNode(); // default constructor
        ~ModelNode(); // default destructor
        ModelNode(const ModelNode& other); // copy constructor
        ModelNode(std::string value, Element aType); // Parametrized constructor with given value and type
        ModelNode(std::shared_ptr<object> mapPtr); // Parametrized constructor with given pointer to the object
        ModelNode(std::shared_ptr<list> arrayPtr); // Parametrized constructor with given pointer to the list

        ModelNode &operator=(const std::string& aValue); // overloaded assignment operator for string value
        ModelNode &operator=(std::shared_ptr<object> mapPtr); // overloaded assignment operator for object pointer
        ModelNode &operator=(std::shared_ptr<list> arrayPtr); // overloaded assignment operator for list pointer
        ModelNode &operator=(const ModelNode& aModelNode); // copy constructor

        ModelNode operator[](const std::string& key); // overloaded subscript operator with string key
        ModelNode operator[](const size_t index); // overloaded subscript operator with index

    private:

        // Helper functions to detect the type of value stored - defined in typechecker.cpp
        void detectValueType(std::string value);
        std::optional<int> intChecker(const std::string& value);
        std::optional<long> longChecker(const std::string& value);
        std::optional<float> floatChecker(const std::string& value);
        std::optional<double> doubleChecker(const std::string& value);
        std::optional<bool> boolChecker(const std::string& value);

	};

	class Model : public JSONListener {
    public:

		Model(); // default constructor
		~Model() override = default; // default destructor
		Model(const Model& aModel); // copy constructor
		Model &operator=(const Model& aModel); // overloaded assignment operator

		ModelQuery createQuery();
        const std::shared_ptr<object> &getRootObject() const; // Getter for root object stored in private

        void printObject(const object &obj, int indentationLevel = 0); // Debug functions to print a given object
        void printList(const list &lst, int indentationLevel = 0); // Debug functions to print a given list

	protected:
		// JSONListener methods
		bool addKeyValuePair(const std::string &aKey, const std::string &aValue, Element aType) override;
		bool addItem(const std::string &aValue, Element aType) override;
		bool openContainer(const std::string &aKey, Element aType) override;
		bool closeContainer(const std::string &aKey, Element aType) override;

    private:
        std::shared_ptr<object> rootObject = std::make_shared<object>(); // root object pointer
        std::stack<std::shared_ptr<object>> objectStack; // Stack of object pointers
        std::stack<std::shared_ptr<list>> listStack; // Stack of list pointers
        int stackCounter{0}; // To keep track of size of object stack
	};

	class ModelQuery {
	public:
		ModelQuery(Model& aModel);

		// ---Traversal---
		ModelQuery& select(const std::string& aQuery);

		// ---Filtering---
		ModelQuery& filter(const std::string&  aQuery);

		// ---Consuming---
		size_t count();
		double sum();
		std::optional<std::string> get(const std::string& aKeyOrIndex);

	protected:
		Model &model; // has-a model instance for querying
        std::shared_ptr<object> rootObject = model.getRootObject(); // root object obtained from model
        ModelNode selectedNode{rootObject}; // ModelNode object obtained from traversal, filtering methods to use in consuming methods

        // Helper functions to format object, list into a std::string
        std::string formatObject(const object& obj);
        std::string formatList(const list& array);

        // Helper function to Check if a string is number
        bool isnum(const std::string& s);

        // Helper function to split a given string into list given a delimiter
        std::vector<std::string> split(const std::string& aQuery, const char* delim);
	};


} // namespace ECE141