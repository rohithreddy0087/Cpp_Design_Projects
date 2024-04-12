//
// Created by rrr on 2/20/24.
//

#include "Model.h"

namespace ECE141 {
    void Model::printObject(const object &obj, int indentationLevel) {
        std::string indentation(indentationLevel * 2, ' ');

        std::cout << indentation << "{\n";
        for (const auto &entry: obj) {
            std::cout << indentation << "  \"" << entry.first << "\": ";

            const ModelNode &node = entry.second;
            switch (node.nodeType) {
                case NodeType::Int:
                    std::cout << std::get<int>(node.nodeValue.value()) << ",\n";
                    break;
                case NodeType::LongInt:
                    std::cout << std::get<long int>(node.nodeValue.value()) << ",\n";
                    break;
                case NodeType::Float:
                    std::cout << std::get<float>(node.nodeValue.value()) << ",\n";
                    break;
                case NodeType::Double:
                    std::cout << std::get<double>(node.nodeValue.value()) << ",\n";
                    break;
                case NodeType::Bool:
                    std::cout << (std::get<bool>(node.nodeValue.value()) ? "true" : "false") << ",\n";
                    break;
                case NodeType::String:
                    std::cout << "\"" << std::get<std::string>(node.nodeValue.value()) << "\",\n";
                    break;
                case NodeType::List:
                    printList(*std::get<std::shared_ptr<list>>(node.nodeValue.value()), indentationLevel + 1);
                    break;
                case NodeType::Object:
                    printObject(*std::get<std::shared_ptr<object>>(node.nodeValue.value()), indentationLevel + 1);
                    break;
                default:
                    break;
            }
        }
        std::cout << indentation << "},\n";
    }

    void Model::printList(const list &lst, int indentationLevel) {
        std::string indentation(indentationLevel * 2, ' ');

        std::cout << indentation << "[\n";
        for (const auto &node: lst) {
            switch (node.nodeType) {
                case NodeType::Int:
                    std::cout << indentation << "  " << std::get<int>(node.nodeValue.value()) << ",\n";
                    break;
                case NodeType::LongInt:
                    std::cout << indentation << "  " << std::get<long int>(node.nodeValue.value()) << ",\n";
                    break;
                case NodeType::Float:
                    std::cout << indentation << "  " << std::get<float>(node.nodeValue.value()) << ",\n";
                    break;
                case NodeType::Double:
                    std::cout << indentation << "  " << std::get<double>(node.nodeValue.value()) << ",\n";
                    break;
                case NodeType::Bool:
                    std::cout << indentation << "  " << (std::get<bool>(node.nodeValue.value()) ? "true" : "false")
                              << ",\n";
                    break;
                case NodeType::String:
                    std::cout << indentation << "  \"" << std::get<std::string>(node.nodeValue.value()) << "\",\n";
                    break;
                case NodeType::List:
                    printList(*std::get<std::shared_ptr<list>>(node.nodeValue.value()), indentationLevel + 1);
                    break;
                case NodeType::Object:
                    printObject(*std::get<std::shared_ptr<object>>(node.nodeValue.value()), indentationLevel + 1);
                    break;
                default:
                    break;
            }
        }
        std::cout << indentation << "],\n";
    }
}