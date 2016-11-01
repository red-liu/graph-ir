//
// Created by alex on 13/12/15.
//

#ifndef METADIFF_CORE_OPERATORS_BASE_H
#define METADIFF_CORE_OPERATORS_BASE_H

namespace md {
    namespace core {
        namespace op {
            /** Addition */
            class Add : public AssociativeElementwiseOperator {
            public:
                Add(GraphInPtr graph, NodeVec parents) :
                        AbstractOperator(graph, "Add"), AssociativeOperator(parents) {}

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Add>(graph, ancestors);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return my_derivative;
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return parent_derivatives[index];
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        bool check[parents.size()];
//                        for (int i = 0; i < parents.size(); i++) {
//                            check[i] = false;
//                        }
//                        if (parents.size() != op->get_parents().size()) {
//                            return false;
//                        }
//                        for (int i = 0; i < parents.size(); i++) {
//                            Node parent = op->get_parents()[i];
//                            int j = 0;
//                            for (; j < parents.size(); j++) {
//                                if (symbolic_equals(parent, parents[j]) and not check[j]) {
//                                    check[j] = true;
//                                    break;
//                                }
//                            }
//                            if (j == parents.size()) {
//                                return false;
//                            }
//                        }
//                    }
//                    return false;
//                }
            };

            /** Negation */
            class Neg : public UnaryElementwiseOperator {
            public:
                Neg(GraphInPtr graph, Node parent) :
                AbstractOperator(graph, "Neg"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Neg>(graph, ancestors[0]);
                }

                DataType get_data_type() const {
                    // If unsigned make it signed
                    if(parent->data_type < i8){
                        return static_cast<DataType>(parent->data_type + 4);
                    }
                    return parent->data_type;
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return neg(my_derivative);
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    return neg(parent_derivatives[index]);
                }
            };

            /** Elementwise multiplication */
            class Mul : public AssociativeElementwiseOperator {
            public:
                Mul(GraphInPtr graph, NodeVec parents) :
                        AbstractOperator(graph, "Mul"), AssociativeOperator(parents) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Mul>(graph, ancestors);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    if (parents.size() == 2) {
                        return mul(my_derivative, parents[1 - index]);
                    } else {
                        return div(mul(my_derivative, owner), parents[index]);
                    }
                }

                Node forward_diff_parent(NodeVec & parent_derivatives, int index){
                    if(parents.size() == 2){
                        return mul(parent_derivatives[index], parents[1-index]);
                    } else {
                        return div(mul(parent_derivatives[index], owner), parents[index]);
                    }
                }

//                bool equals(Operator const op) const {
//                    if (name == op->name) {
//                        bool check[parents.size()];
//                        for (int i = 0; i < parents.size(); i++) {
//                            check[i] = false;
//                        }
//                        if (parents.size() != op->get_parents().size()) {
//                            return false;
//                        }
//                        for (int i = 0; i < parents.size(); i++) {
//                            Node parent = op->get_parents()[i];
//                            int j = 0;
//                            for (; j < parents.size(); j++) {
//                                if (symbolic_equals(parent, parents[j]) and not check[j]) {
//                                    check[j] = true;
//                                    break;
//                                }
//                            }
//                            if (j == parents.size()) {
//                                return false;
//                            }
//                        }
//                    }
//                    return false;
//                }
            };

            /** Elementwise inverse (division) */
            class Division : public FloatUnaryElementwiseOperator {
            public:
                Division(GraphInPtr graph, Node parent) :
                        AbstractOperator(graph, "Division"), UnaryOperator(parent) {};

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<Division>(graph, ancestors[0]);
                }

                Node backward_diff_parent(Node my_derivative, int index){
                    return - (my_derivative / square(parent));
                }
            };

            /** Integer division */
            class IntDiv : public BinaryIntegerElementwiseOperator{
            public:
                IntDiv(GraphInPtr graph, Node parent1, Node parent2) :
                AbstractOperator(graph, "IntDiv"), BinaryOperator(parent1, parent2){
                    // TODO check types of parent1 and parent2 and cast them to ints if needed
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<IntDiv>(graph, ancestors[0], ancestors[1]);
                }
            };

            /** Integer modulus (reminder) */
            class IntMod : public BinaryIntegerElementwiseOperator {
            public:
                IntMod(GraphInPtr graph, Node parent1, Node parent2) :
                        AbstractOperator(graph, "IntMod"), BinaryOperator(parent1, parent2){
                    // TODO check types of parent1 and parent2 and cast them to ints if needed
                };

                Operator copy_to(GraphInPtr graph, NodeVec ancestors) const {
                    return std::make_shared<IntMod>(graph, ancestors[0], ancestors[1]);
                }
            };
        }
    }
}

#endif //METADIFF_OPERATORS_BASE_H
