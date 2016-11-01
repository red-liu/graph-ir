//
// Created by alex on 27/10/16.
//

#include "metadiff.h"

namespace md{
    namespace api {

        Node tensor_as(Node node, std::string name) {
            if (name == "") {
                name = node->name + "_clone";
            }
            return node->g()->tensor4(node->data_type, node->shape, name);
        }

//        Node tensor4(DataType data_type,
//                     std::array<SymInt, 4> shape,
//                     std::string name,
//                     Graph g) {
//            auto op = std::make_shared<op::Input>(g.get(), data_type, shape);
//            auto result = std::make_shared<NodeData>(g, g->nodes.size(),
//                    name, g->props.default_device,
//                    op, 0, g->current_group);
//            g->nodes.push_back(result);
//            result->op->owner = result;
//            return result;
//        }
//
//        Node tensor4(DataType data_type,
//                     SymInt shape0,
//                     SymInt shape1,
//                     SymInt shape2,
//                     SymInt shape3,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {shape0, shape1, shape2, shape3}, name, g);
//        }
//
//        Node tensor4(DataType data_type,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {new_sym(), new_sym(), new_sym(), new_sym()}, name, g);
//        }
//
//        Node tensor3(DataType data_type,
//                     std::array<SymInt, 3> shape,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {shape[0], shape[1], shape[2], 1}, name, g);
//        }
//
//        Node tensor3(DataType data_type,
//                     SymInt shape0,
//                     SymInt shape1,
//                     SymInt shape2,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {shape0, shape1, shape2, 1}, name, g);
//        }
//
//        Node tensor3(DataType data_type,
//                     std::string name,
//                     Graph g) {
//            return tensor4(data_type, {new_sym(), new_sym(), new_sym(), 1}, name, g);
//        }
//
//        Node matrix(DataType data_type,
//                    std::array<SymInt, 2> shape,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {shape[0], shape[1], 1, 1}, name, g);
//        }
//
//        Node matrix(DataType data_type,
//                    SymInt shape0,
//                    SymInt shape1,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {shape0, shape1, 1, 1}, name, g);
//        }
//
//        Node matrix(DataType data_type,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {new_sym(), new_sym(), 1, 1}, name, g);
//        }
//
//        Node square_matrix(DataType data_type,
//                           SymInt shape,
//                           std::string name,
//                           Graph g) {
//            return tensor4(data_type, {shape, shape, 1, 1}, name, g);
//        }
//
//        Node vector(DataType data_type,
//                    SymInt shape,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {shape, 1, 1, 1}, name, g);
//        }
//
//        Node vector(DataType data_type,
//                    std::string name,
//                    Graph g) {
//            return tensor4(data_type, {new_sym(), 1, 1, 1}, name, g);
//        }
//
//        Node scalar(DataType data_type, std::string name, Graph g) {
//            return tensor4(data_type, {1, 1, 1, 1}, name, g);
//        }
//
//        Node tensor_as(Node node, std::string name) {
//            if(name == ""){
//                name = node->name + "_clone";
//            }
//            return tensor4(node->data_type, node->shape, name, node->g());
//        }
    }
    namespace core{
        Node GraphInternal::tensor4(DataType data_type,
                                    std::array<SymInt, 4> shape,
                                    std::string name) {
            auto op = std::make_shared<op::Input>(this, data_type, shape);
            auto result = std::make_shared<NodeData>(shared_from_this(), nodes.size(),
                                                     name, props.default_device,
                                                     op, 0, current_group);
            nodes.push_back(result);
            result->op->owner = result;
            return result;
        }

        Node GraphInternal::tensor4(DataType data_type,
                                    SymInt shape0,
                                    SymInt shape1,
                                    SymInt shape2,
                                    SymInt shape3,
                                    std::string name) {
            return tensor4(data_type, {shape0, shape1, shape2, shape3}, name);
        }

        Node GraphInternal::tensor4(DataType data_type,
                                    std::string name) {
            return tensor4(data_type, {new_sym(), new_sym(), new_sym(), new_sym()}, name);
        }

        Node GraphInternal::tensor3(DataType data_type,
                                    std::array<SymInt, 3> shape,
                                    std::string name) {
            return tensor4(data_type, {shape[0], shape[1], shape[2], 1}, name);
        }

        Node GraphInternal::tensor3(DataType data_type,
                                    SymInt shape0,
                                    SymInt shape1,
                                    SymInt shape2,
                                    std::string name) {
            return tensor4(data_type, {shape0, shape1, shape2, 1}, name);
        }

        Node GraphInternal::tensor3(DataType data_type,
                                    std::string name) {
            return tensor4(data_type, {new_sym(), new_sym(), new_sym(), 1}, name);
        }

        Node GraphInternal::matrix(DataType data_type,
                                   std::array<SymInt, 2> shape,
                                   std::string name) {
            return tensor4(data_type, {shape[0], shape[1], 1, 1}, name);
        }

        Node GraphInternal::matrix(DataType data_type,
                                   SymInt shape0,
                                   SymInt shape1,
                                   std::string name) {
            return tensor4(data_type, {shape0, shape1, 1, 1}, name);
        }

        Node GraphInternal::matrix(DataType data_type,
                                   std::string name) {
            return tensor4(data_type, {new_sym(), new_sym(), 1, 1}, name);
        }

        Node GraphInternal::square_matrix(DataType data_type,
                                          SymInt shape,
                                          std::string name) {
            return tensor4(data_type, {shape, shape, 1, 1}, name);
        }

        Node GraphInternal::vector(DataType data_type,
                                   SymInt shape,
                                   std::string name) {
            return tensor4(data_type, {shape, 1, 1, 1}, name);
        }

        Node GraphInternal::vector(DataType data_type,
                                   std::string name) {
            return tensor4(data_type, {new_sym(), 1, 1, 1}, name);
        }

        Node GraphInternal::scalar(DataType data_type, std::string name) {
            return tensor4(data_type, {1, 1, 1, 1}, name);
        }
    }
}