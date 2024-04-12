//
// Created by Mark on 1/30/2024.
//

#include "Model.h"
#include "Debug.h"
#include <typeinfo>
#include <numeric>

namespace ECE141 {

    // ---Model Node---

    // Default Constructor, initializing nodeValue, nodeType and orgValue to null/empty
    ModelNode::ModelNode(): nodeValue(std::nullopt), nodeType(NodeType::Null), orgValue("~~empty~~") {};

    /**
     * @brief Constructs a ModelNode instance with the given string value and element type.
     *
     * @param value The string value to be assigned to the ModelNode.
     * @param aType The element type, specifying whether the value is a constant or a string.
     */
    ModelNode::ModelNode(std::string value, Element aType){
        // If the element type is 'constant', it checks for special constant values like 'null'
        if(aType == Element::constant){
            if(value == "null"){
                nodeValue = std::nullopt;
                nodeType = NodeType::Null;
                orgValue = value;
            }else {
                detectValueType(value);
            }
        } else { // Otherwise, it treats the value as a string and sets the nodeType to NodeType::String.
            nodeValue = value;
            nodeType = NodeType::String;
            orgValue = value;
        }
    }

    // Constructs a ModelNode instance with the given pointer to object
    ModelNode::ModelNode(std::shared_ptr<object> mapPtr) : nodeValue(mapPtr), nodeType(NodeType::Object) {};

    // Constructs a ModelNode instance with the given pointer to list
    ModelNode::ModelNode(std::shared_ptr<list> arrayPtr) : nodeValue(arrayPtr), nodeType(NodeType::Object) {};

    // Copy Constructor
    ModelNode::ModelNode(const ModelNode &other) {
        nodeType = other.nodeType;
        nodeValue = other.nodeValue;
        orgValue = other.orgValue;
    }

    // Overloaded assignment operator for a given string value
    ModelNode &ModelNode::operator=(const std::string& aValue){
        detectValueType(aValue);
        return *this;
    }

    // Overloaded assignment operator for a given object pointer
    ModelNode &ModelNode::operator=(std::shared_ptr<object> mapPtr) {
        nodeValue = mapPtr;
        nodeType = NodeType::Object;
        return *this;
    }

    // Overloaded assignment operator for a given list pointer
    ModelNode &ModelNode::operator=(std::shared_ptr<list> arrayPtr) {
        nodeValue = arrayPtr;
        nodeType = NodeType::List;
        return *this;
    }

    // Overloaded assignment operator for a ModelNode object
    ModelNode &ModelNode::operator=(const ModelNode& aModelNode) {
        nodeType = aModelNode.nodeType;
        nodeValue = aModelNode.nodeValue;
        orgValue = aModelNode.orgValue;
        return *this;
    }

    /**
     * @brief Accesses the value associated with the given key in a ModelNode representing an object.
     *
     * @param key The string key to look up in the object.
     * @return ModelNode The ModelNode associated with the provided key.
     * @throw std::out_of_range Thrown if the key is not found in the object.
     * @throw std::runtime_error Thrown if the node is not of type NodeType::Object.
     */
    ModelNode ModelNode::operator[](const std::string &key) {
        if (nodeValue && nodeType == NodeType::Object) {
            // gets the object from nodeValue
            auto& mapValue = *std::get<std::shared_ptr<object>>(nodeValue.value());
            // finds if key is present in the object and returns the corresponding ModelNode
            auto it = mapValue.find(key);
            if (it != mapValue.end()) {
                return it->second;
            } else {
                throw std::out_of_range("Key not found in the map.");
            }
        }else {
            throw std::runtime_error("Trying to index a non-object node.");
        }
    }

    /**
     * @brief Accesses the value at the specified index in a ModelNode representing a list.
     *
     * @param index The index to look up in the list.
     * @return ModelNode The ModelNode at the provided index or an empty ModelNode if out of bounds.
     * @throw std::runtime_error Thrown if the node is not of type NodeType::List.
     */
    ModelNode ModelNode::operator[](const size_t index) {
        if (nodeValue && nodeType == NodeType::List) {
            // gets the list from nodeValue
            auto& vector = *std::get<std::shared_ptr<list>>(nodeValue.value());
            // finds if index is present in the vector and returns the corresponding ModelNode
            if (index < vector.size()) {
                return vector[index];
            } else {
                // returns ~~empty~~ when index is out of bounds
                return {};
            }
        }else {
            throw std::runtime_error("Trying to index a non-object node.");
        }
    }

    // Default Destructor
    ModelNode::~ModelNode() = default;

    // ---Model---

    // Default Constructor
	Model::Model() {
        objectStack.push(rootObject); //Pushes the rootObject into an empty stack
	}

    // Copy Constructor using assignment operator
	Model::Model(const Model& aModel) {
		*this = aModel;
	}

    // Overloaded assignment operator
	Model &Model::operator=(const Model& aModel) {
		rootObject = aModel.rootObject;
        objectStack = aModel.objectStack;
        listStack = aModel.listStack;
        stackCounter = aModel.stackCounter;
		return *this;
	}

    // Getter of rootObject
    const std::shared_ptr<object> &Model::getRootObject() const {
        return rootObject;
    }

	ModelQuery Model::createQuery() {
		return ModelQuery(*this);
	}

    /**
     * @brief Adds a key-value pair to the topmost object in the object stack.
     *
     * @param aKey The key to be added.
     * @param aValue The value to be associated with the key.
     * @param aType The element type indicating the type of the value.
     * @return true if the key-value pair was successfully added, false otherwise.
     */
	bool Model::addKeyValuePair(const std::string& aKey, const std::string& aValue, Element aType) {

        if(aType == Element::constant || aType == Element::quoted){
            if (!objectStack.empty()) {
                // Get the shared pointer to the top object in the stack
                std::shared_ptr<object> latestObject = objectStack.top();
                // Add the key-value pair to the object using ModelNode constructor
                (*latestObject)[aKey] = ModelNode(aValue, aType);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Adds an item to the topmost list in the list stack.
     *
     * @param aValue The value to be added as an item.
     * @param aType The element type indicating the nature of the value.
     * @return true if the item was successfully added, false otherwise.
     */
	bool Model::addItem(const std::string& aValue, Element aType) {
        if(aType == Element::constant || aType == Element::quoted){
            if (!listStack.empty()) {
                // Get the shared pointer to the top list in the stack
                std::shared_ptr<list> latestList = listStack.top();
                // Add the value as an item to the list using ModelNode constructor
                (*latestList).push_back(ModelNode(aValue, aType));
                return true;
            }
        }
		return false;
	}

    /**
     * @brief Opens a new container in the model hierarchy.
     *
     *
     * @param aContainerName The name to be assigned to the new container.
     * @param aType The element type indicating whether the container is an object
     *              or an array.
     * @return true if the container was successfully opened, false otherwise.
     */
	bool Model::openContainer(const std::string& aContainerName, Element aType) {

        if (aType == Element::object){
            // Creates a shared pointer to a new object
            std::shared_ptr<object> newObject = std::make_shared<object>();
            // Checks if the object stack is not empty and stackCounter is positive, to avoid pushing a new pointer for first time
            if (!objectStack.empty() && stackCounter>0) {
                if(!aContainerName.empty()){
                    //Assign the top of stack object with key(aContainerName) and value as newObject
                    objectStack.top()->operator[](aContainerName) = newObject;
                }
                objectStack.push(newObject);
            }
            ++stackCounter;
        } else if (aType == Element::array){
            // Create a shared pointer to a new list
            std::shared_ptr<list> newList = std::make_shared<list>();
            if (!objectStack.empty()){
                //Assign the top of stack object with key(aContainerName) and value as newList
                objectStack.top()->operator[](aContainerName) = newList;
                listStack.push(newList);
            }
        } else{
            // returns false if it neither { or [
            return false;
        }
		return true;
	}

    /**
     * @brief Closes the current container in the model hierarchy.
     *
     * @param aContainerName The name of the container to be closed. If empty, the
     *                       function closes the most recent container.
     * @param aType The element type indicating whether the container to be closed
     *              is an object or an array.
     * @return true if the container was successfully closed, false otherwise.
     */
	bool Model::closeContainer(const std::string& aContainerName, Element aType) {
        if (aType == Element::object){
            if(aContainerName.empty()) {
                // Closes the top of the liststack, if an empty containername is found and pops it from the stack
                if (!objectStack.empty() && !listStack.empty()) {
                    std::shared_ptr<list> latestList = listStack.top();
                    (*latestList).push_back(objectStack.top());
                    if (objectStack.size() > 1) objectStack.pop();
                }
            }else{
            if (!objectStack.empty()) {
                // the top object is poped out of stack expect from rootNode
                if(objectStack.size() > 1) objectStack.pop();
            }}
        } else if (aType == Element::array){
            if (!listStack.empty()) {
                listStack.pop();
            }
        } else{
            return false;
        }
        return true;
	}


	// ---ModelQuery---

    // Constructor for ModelQuery
	ModelQuery::ModelQuery(Model &aModel) : model(aModel) {}

    /**
     * @brief Select a node in the model based on the provided query.
     *
     * @param aQuery The query string specifying the path in the model.
     * @return Reference to the ModelQuery object for chaining.
     */
	ModelQuery& ModelQuery::select(const std::string& aQuery) {
        const char *delim = ".";
        // Splits the query into list
        std::vector<std::string> keysAndIndices = split(aQuery, delim);
         try{
            for (const auto& element : keysAndIndices) {
                if (isnum(element)) {
                    // if element is a index, we use subscript to get the ModelNode and assign it to selectedNode iteratively
                    selectedNode = selectedNode[std::stoi(element)];
                } else {
                    // Similary if element is a string
                    selectedNode = selectedNode[element];
                }
            }
        } catch (const std::exception& e){
             // if an excpetion occurs, we assign it as an empty ModelNode
            selectedNode = ModelNode();
        }
		return *this;
	}

    /**
     * @brief Filter the selected node based on the specified query.
     *
     * This function interprets the query string to determine the filtering method.
     * Currently, it supports filtering based on index or key. For index-based
     * filtering, it compares the elements in the list with the specified index
     * using various operators (==, !=, >=, <=, >, <) and creates a new list with
     * matching elements. For key-based filtering, it searches for keys containing
     * a specific value and creates a new object with matching key-value pairs.
     *
     * @param aQuery The query string specifying the filtering method and criteria.
     * @return Reference to the ModelQuery object for chaining.
     * @throws std::runtime_error if an unrecognized filtering method is requested.
     */
	ModelQuery& ModelQuery::filter(const std::string& aQuery) {
        const char *delim = " ";
        std::vector<std::string> keysAndIndices = split(aQuery, delim);
        if(keysAndIndices[0] == "index"){
            list tmp = *std::get<std::shared_ptr<list>>(selectedNode.nodeValue.value());
            int index = stoi(keysAndIndices[2]);
            std::string op = keysAndIndices[1];
            // List to copy filtered indices ModelNodes
            list indicesToFilter;
            if (op == "=="){
                // fill all the indices that are == index from 0 to tmp.size()-1
                std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(indicesToFilter), [index, &tmp](const ModelNode& node) {
                    return &node == &tmp[index];
                });
            } else if (op == "!=") {
                // fill all the indices that are != index from 0 to tmp.size()-1
                std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(indicesToFilter), [index, &tmp](const ModelNode& node) {
                    return &node != &tmp[index];
                });
            } else if (op == ">=") {
                // fill all the indices that are >= index from 0 to tmp.size()-1
                std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(indicesToFilter), [index, &tmp](const ModelNode& node) {
                    return &node >= &tmp[index];
                });
            } else if (op == "<=") {
                // fill all the indices that are <= index from 0 to tmp.size()-1
                std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(indicesToFilter), [index, &tmp](const ModelNode& node) {
                    return &node <= &tmp[index];
                });
            } else if (op == ">") {
                // fill all the indices that are > index from 0 to tmp.size()-1
                std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(indicesToFilter), [index, &tmp](const ModelNode& node) {
                    return &node > &tmp[index];
                });
            } else if (op == "<") {
                // fill all the indices that are < index from 0 to tmp.size()-1
                std::copy_if(tmp.begin(), tmp.end(), std::back_inserter(indicesToFilter), [index, &tmp](const ModelNode& node) {
                    return &node < &tmp[index];
                });
            } else {
                throw std::runtime_error("Unrecognized operator");
            }
            // Assign the filtered indices list to the selectedNode
            selectedNode = std::make_shared<list>(indicesToFilter);
        } else if (keysAndIndices[0] == "key"){
            object tmp = *std::get<std::shared_ptr<object>>(selectedNode.nodeValue.value());
            std::string action = keysAndIndices[1];
            std::string value = keysAndIndices[2];
            // Object to copy filtered keys
            object keysToFilter;

            for (const auto &entry: tmp) {
                std::string mainString = entry.first;
                // Checks is the given value is present in the object key's
                size_t found = mainString.find(value);
                if (found != std::string::npos) {
                    keysToFilter[mainString] = entry.second;
                }
            }
            // Assign the filtered objects to the selectedNode
            selectedNode = std::make_shared<object>(keysToFilter);

        } else {
            throw std::runtime_error("Unknown filtering method requested.");
        }

		return *this;
	}

    /**
     * Retrieves the number of elements in the selected node.
     *
     * @return The number of elements in the selected node. If the node is not a list or an object, returns 0.
     */
	size_t ModelQuery::count() {
        // Checks if the nodetype is a list or object, then computes the size
        if(selectedNode.nodeType == NodeType::List){
            list tmp = *std::get<std::shared_ptr<list>>(selectedNode.nodeValue.value());
            return tmp.size();
        } else if (selectedNode.nodeType == NodeType::Object){
            object tmp = *std::get<std::shared_ptr<object>>(selectedNode.nodeValue.value());
            return tmp.size();
        }
		return 0;
	}

    /**
     * Calculates the sum of numeric elements in the selected node if it is a list.
     *
     * @return The sum of numeric elements in the selected node. If the node is not a list or if it contains non-numeric
     * elements, returns 0.0.
     * @throws std::runtime_error if attempting to sum a list containing object, list, bool, string, or null elements.
     */
	double ModelQuery::sum() {
        if(selectedNode.nodeType == NodeType::List){
           list tmp = *std::get<std::shared_ptr<list>>(selectedNode.nodeValue.value());
           double total = 0.0;
            for (const auto &node: tmp) {
                // Checks if the list is made of Ints, Long Ints, Floats or Double, then only computes the sum
                if(node.nodeType != NodeType::Object && node.nodeType != NodeType::List && node.nodeType != NodeType::Bool
                        && node.nodeType != NodeType::String && node.nodeType != NodeType::Null){
                    switch (node.nodeType) {
                        case NodeType::Int:
                            // typecast into double and add
                            total += static_cast<double>(std::get<int>(node.nodeValue.value()));
                            break;
                        case NodeType::LongInt:
                            total += static_cast<double>(std::get<long int>(node.nodeValue.value()));
                            break;
                        case NodeType::Float:
                            total += static_cast<double>(std::get<float>(node.nodeValue.value()));
                            break;
                        case NodeType::Double:
                            total += std::get<double>(node.nodeValue.value());
                            break;
                        default:
                            break;
                    }
                }else{
                    throw std::runtime_error("Trying to sum a object list.");
                }
            }
            return total;
        }
		return 0.0;
	}

    /**
     * Retrieves the value associated with the specified key or index in the selected node.
     *
     * @param aKeyOrIndex The key or index to retrieve from the selected node. If aKeyOrIndex is a numeric string,
     * it retrieves the element at the corresponding index. If aKeyOrIndex is "*", returns the entire selected node.
     * If aKeyOrIndex is a quoted string, it retrieves the value associated with the specified key.
     * @return An optional string containing the retrieved value. If the specified key or index is not found, or if
     * an error occurs during retrieval, returns std::nullopt.
     */
	std::optional<std::string> ModelQuery::get(const std::string& aKeyOrIndex) {
        try {
            // Retrives the ModelNode based on the given key or index
            ModelNode finalNode;
            if (isnum(aKeyOrIndex)) {
                finalNode = selectedNode[std::stoi(aKeyOrIndex)];
            } else if (aKeyOrIndex == "*"){
                finalNode = selectedNode;
            } else{
                std::string newKeyOrIndex = aKeyOrIndex.substr(1, aKeyOrIndex.size() - 2);
                finalNode = selectedNode[newKeyOrIndex];
            }

            // Formats the ModelNode into string based on the node type
            if(finalNode.nodeType == NodeType::Object){
                return formatObject(*std::get<std::shared_ptr<object>>(finalNode.nodeValue.value()));
            } else if(finalNode.nodeType == NodeType::List){
                return formatList(*std::get<std::shared_ptr<list>>(finalNode.nodeValue.value()));
            }else if(finalNode.nodeType == NodeType::String){
                std::stringstream output;
                output << "\"" << finalNode.orgValue<< "\"";
                return output.str();
            }else{
                return finalNode.orgValue;
            }
        } catch (const std::exception& e){
            return std::nullopt;
        }
	}

    /**
     * Checks if a given string represents a numeric value.
     *
     * @param s The input string to be checked for numeric content.
     * @return True if the string is non-empty and contains only numeric characters, otherwise false.
     */
    bool ModelQuery::isnum(const std::string& s){
        return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
    }

    /**
     * Splits a string into a vector of substrings based on a specified delimiter.
     *
     * @param aQuery The input string to be split.
     * @param delim The delimiter character used for splitting the string.
     * @return A vector of substrings obtained by splitting the input string.
     */
    std::vector<std::string> ModelQuery::split(const std::string& aQuery, const char* delim){
        std::vector<std::string> keysAndIndices;
        std::istringstream iss(aQuery);
        std::string token;
        while (std::getline(iss, token, *delim)) {
            // Check for and remove surrounding quotes from the token
            if (!token.empty() && (token.front() == '\'' || token.front() == '\"')) {
                token.erase(0, 1); // Erase the opening quote
                if (!token.empty() && (token.back() == '\'' || token.back() == '\"')) {
                    token.pop_back(); // Erase the closing quote
                }
            }
            keysAndIndices.push_back(token);
        }
        return keysAndIndices;
    }

    /**
     * Formats an object into a JSON-like string.
     * Recursively processes nested objects and lists.
     *
     * @param obj The object to format.
     * @return A formatted string representing the object in a JSON-like format.
     */
    std::string ModelQuery::formatObject(const object& obj){
        std::stringstream output;
        output << "{";
        size_t count = 0;
        for (const auto &entry: obj) {
            output << "  \"" << entry.first << "\": ";
            const ModelNode &node = entry.second;
            if(node.nodeType == NodeType::Object){
                output<<formatObject(*std::get<std::shared_ptr<object>>(node.nodeValue.value()));
            } else if(node.nodeType == NodeType::List){
                output<<formatList(*std::get<std::shared_ptr<list>>(node.nodeValue.value()));
            } else if(node.nodeType == NodeType::String){
                output << "\"" << node.orgValue<< "\"";
            }else{
                output<<node.orgValue;
            }
            // Doesn't add comma to last element in the object
            if (count < obj.size()-1) {
                output << ",";
            }
            ++count;
        }
        output << "}";

        return output.str();
    }

    /**
     * Formats a list into a JSON-like string.
     * Recursively processes nested objects and lists.
     *
     * @param array The list to format.
     * @return A formatted string representing the list in a JSON-like format.
     */
    std::string ModelQuery::formatList(const list& array){
        std::stringstream output;
        output << "[";
        size_t count = 0;
        for (const auto &node: array) {
            if(node.nodeType == NodeType::Object){
                output<<formatObject(*std::get<std::shared_ptr<object>>(node.nodeValue.value()));
            } else if(node.nodeType == NodeType::List){
                output<<formatList(*std::get<std::shared_ptr<list>>(node.nodeValue.value()));
            } else if(node.nodeType == NodeType::String){
                output << "\"" << node.orgValue<< "\"";
            } else{
                output<<node.orgValue;
            }
            // Doesn't add comma to last element in the olist
            if (count < array.size()-1) {
                output << ",";
            }
            ++count;
        }
        output << "]";
        return output.str();
    }

} // namespace ECE141