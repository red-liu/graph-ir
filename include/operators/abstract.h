//
// Created by alex on 04/05/16.
//

#ifndef METADIFF_CORE_OPERATORS_ABSTRACT_H
#define METADIFF_CORE_OPERATORS_ABSTRACT_H

namespace md {
    namespace core {
        namespace op {
            /** The abstract class for all operatros */
            class AbstractOperator {
            protected:
                /** This should never be called directly, it exists ONLY because of virtual inheritance */
                AbstractOperator(): name(""), graph(nullptr) {};

                /** Easy way to get logging for the corresponding operator */
                std::shared_ptr<spdlog::logger> logger() const {
                    return md::utils::logger("operator::" + name);
                }
            public:
                /** Unique name of the concrete Operator class */
                std::string const name;
                /** Pointer to the owning GraphInternal */
                GraphInPtr const graph;
                /** Pointer to the owning Node */
                Node owner;

                /** The Node owner should be set by the graph after creating the output Node */
                AbstractOperator(std::string const name,
                                 GraphInPtr const graph) :
                        name(name),  graph(graph) {};

                /** Copies the operator to a separate graph with the corresponding ancestors there */
                virtual Operator copy_to(GraphInPtr graph, NodeVec ancestors) const = 0;

                /** Calculates the dataType for the output Node based on its ancestors*/
                virtual dataType get_data_type() const = 0;

                /** Calculates the Shape for the output Node based on its ancestors*/
                virtual Shape get_shape() const = 0;

                /** Calculates if the output Node is input dependent based on its ancestors*/
                virtual bool is_input_dependent() const;

                /** Calculates if the output Node is differentiable dependent based on its ancestors*/
                virtual bool is_differentiable() const;

                /** Calculates the maximum grad level of its ancestors */
                unsigned short get_grad_level() const;

                /** Returns the parents of this operator (all ancestors influencing in differentiable way) */
                virtual NodeVec get_parents() const  = 0;

                /** Returns the arguments of this operator (all ancestors influencing in non-differentiable way) */
                virtual NodeVec get_arguments() const;

                /** Returns the union of the parents and arguments */
                virtual NodeVec get_ancestors() const;

                /** Returns the gradient with respect to the paret at index during Backward Differentiation */
                virtual Node backward_diff(Node my_grad, short index) = 0;

                /** Combines the gradients of all of the children of the operator. This is relevant only for
                 * multy-output operators, for all others this is just add */
                virtual Node backward_diff_combine(NodeVec grads) const;

                /** TODO */
                virtual Node forward_diff(Node my_grad, short index);

                /** The method generates and sends the gradients for all parents, provided the incoming gradient messages */
                void backward_diff(std::vector<NodeVec> &messages, std::vector<bool>& flow_tree);

                /**
                 * TODO this and the symbolic_equals are things which aren't yet well done
                 * Compares only if this operator is equal to the other, not the other way around.
                 * Note that although equality is symmetric, because of mathematical idenitities
                 * and the fact that the code is with each operator separately the true equality
                 * operator is `op1.equals(op2) or op2.equals(op1)`
                 */
                virtual bool equals(Operator const op) const{
                    return false;
                }
            };


            class NonDifferentiableOperator: public virtual AbstractOperator {
                bool is_differentiable() const{
                    return false;
                }
            };

            class OrphanOperator: public virtual AbstractOperator {
            public:

                NodeVec get_parents() const {
                    return NodeVec {};
                }

                Node backward_diff(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            class InputOperator: public virtual OrphanOperator {
            public:
                bool is_input_dependent() const{
                    return true;
                }

                bool is_differentiable() const{
                    return true;
                }
            };

            class ConstantOperator : public virtual OrphanOperator,
                                     public virtual NonDifferentiableOperator {
            public:
                dataType data_type;

                ConstantOperator(dataType data_type):
                        data_type(data_type) {};

                dataType get_data_type() const {
                    return data_type;
                }
            };

            class LogicalOperator: public virtual NonDifferentiableOperator{

                dataType get_data_type() const {
                    return b8;
                };

                Node backward_diff(Node my_grad, short index) {
                    auto err = std::make_shared<WrongGradient>(NodeVec{owner, my_grad}, name);
                    err->log(logger());
                    throw err;
                }
            };

            class IntegerOperator: public virtual AbstractOperator{
            public:
                dataType get_data_type() const{
                    return graph->props.max_int;
                }
            };

            class FloatOperator: public virtual AbstractOperator{
            public:
                dataType get_data_type() const{
                    return graph->props.max_float;
                }
            };

            class ElementwiseOperator: public virtual AbstractOperator{
            public:
                Shape get_shape() const {
                    return get_parents()[0]->shape;
                }
            };

            class UnaryOperator: public virtual AbstractOperator{
            protected:
                UnaryOperator(): parent(Node()) {};
            public:
                Node parent;
                UnaryOperator(Node parent) :
                        parent(parent) {};

                NodeVec get_parents() const {
                    return {parent};
                };
            };

            class BinaryOperator: public virtual AbstractOperator{
            protected:
                BinaryOperator(): parent1(Node()), parent2(Node()) {};
            public:
                Node parent1;
                Node parent2;
                BinaryOperator(Node parent1, Node parent2) :
                        parent1(parent1), parent2(parent2) {};

                NodeVec get_parents() const {
                    return {parent1, parent2};
                };
            };

            class AssociativeOperator : public virtual AbstractOperator {
            protected:
                AssociativeOperator() {};
            public:
                NodeVec parents;
                AssociativeOperator(NodeVec parents) :
                        parents(parents) {
                    if (parents.size() < 2) {
                        auto err = std::make_shared<InvalidArguments>
                                (parents, name, "All NaryOperators require at least 2 parents");
                        err->log(logger());
                        throw err;
                    }
                };

                NodeVec get_parents() const {
                    return parents;
                };

                dataType get_data_type() const {
                    return parents[0]->data_type;
                }
            };

            class MorphOperator: public virtual UnaryOperator {
            protected:
                MorphOperator(): UnaryOperator() {};
            public:
                dataType get_data_type() const{
                    return parent->data_type;
                }
            };

            class ReductionOperator : public virtual UnaryOperator {
            protected:
                ReductionOperator(): UnaryOperator() {};
            public:
                Axes axes;
                ReductionOperator(Axes axes) : axes(axes){
                    if(not validate_axes(axes)){
                        std::string axes_str;
                        for (auto i = 0; i < axes.size(); ++i) {
                            axes_str += std::to_string(axes[i]);
                            if (i < axes.size() - 1) {
                                axes_str += ", ";
                            }
                        }
                        if (axes.size() == 0) {
                            axes_str = "NULL";
                        }
                        auto err = std::make_shared<InvalidArguments>(NodeVec{parent}, name, "Invalid axes: " + axes_str);
                        err->log(logger());
                        throw err;
                    }
                    // TODO should we forbid summing over axes where shape[i] = 1?
                    for(auto i=0; i<axes.size(); ++i){
                        if(parent->shape[axes[i]] == 1){
                            axes.erase(std::remove(axes.begin(), axes.end(), i), axes.end());
                        }
                    }
                    this->axes = axes;
                };

                Shape get_shape() const {
                    Shape p_shape = parent->shape;
                    for (int i = 0; i < axes.size(); i++) {
                        p_shape[axes[i]] = 1;
                    }
                    return p_shape;
                }
            };

            class UnaryElementwiseOperator: public virtual UnaryOperator,
                                            public virtual ElementwiseOperator {};

            class LogicalUnaryElementwise: public virtual LogicalOperator,
                                           public virtual UnaryElementwiseOperator {};


            class FloatUnaryElementwiseOperator: public virtual FloatOperator,
                                                 public virtual UnaryElementwiseOperator {};

            class FloatUnaryOperator: public virtual UnaryOperator,
                                      public virtual FloatOperator {};

            class BinaryElementwiseOperator: public virtual BinaryOperator,
                                             public virtual ElementwiseOperator {};

            class LogicalBinaryElementwise: public virtual LogicalOperator,
                                            public virtual BinaryElementwiseOperator {};

            class BinaryIntegerElementwiseOperator: public virtual IntegerOperator,
                                                 public virtual BinaryElementwiseOperator{};

            class BinaryFloatElementwiseOperator: public virtual FloatOperator,
                                               public virtual BinaryElementwiseOperator{};

            class AssociativeElementwiseOperator: public virtual AssociativeOperator,
                                                  public virtual ElementwiseOperator {};

            class MorphElementwiseOperator: public virtual MorphOperator,
                                            public virtual ElementwiseOperator {};

            class LogicalReductionOperator: public virtual LogicalOperator,
                                            public virtual ReductionOperator {};

            class MorphReductionOperator: public virtual MorphOperator,
                                          public virtual ReductionOperator {};


            class FloatReductionOperator: public virtual FloatOperator,
                                          public virtual ReductionOperator {};
        }
    }
}

#endif //METADIFF_CORE_OP_ABSTRACT_H
