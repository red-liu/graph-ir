//
// Created by alex on 03/05/16.
//

#ifndef METADIFF_CORE_OPERATORS_INPUT_H
#define METADIFF_CORE_OPERATORS_INPUT_H

namespace md {
    namespace core {
        namespace op {
            /** Input variables */
            class Input : public InputOperator {
            public:
                dataType  data_type;
                Shape shape;

                Input(GraphInPtr graph, dataType data_type, Shape shape) :
                        AbstractOperator("Input", graph),
                        data_type(data_type), shape(shape) {}

                Operator copy_to(GraphInPtr graph, std::vector<Node> ancestors) const {
                    return std::make_shared<Input>(graph, data_type, shape);
                }

                dataType get_data_type() const {
                    return data_type;
                }

                Shape get_shape() const {
                    return shape;
                }
            };

            /** Shared input variables */
            class SharedInput : public InputOperator {
            public:
                SharedVar var;

                SharedInput(GraphInPtr graph, SharedVar var) :
                        AbstractOperator("Shared", graph),
                        var(var) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<SharedInput>(graph, var);
                }

                dataType get_data_type() const {
                    return var->data_type;
                }

                Shape get_shape() const {
                    return var->shape;
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        auto cast_op = std::static_pointer_cast<const SharedInput>(op);
//                        return var->id == cast_op->var->id;
//                    }
//                    return false;
//                }
            };
        }
    }
}
#endif //METADIFF_CORE_OPERATORS_H
