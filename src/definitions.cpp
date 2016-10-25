//
// Created by alex on 24/10/16.
//

#include "metadiff.h"

namespace md{
    namespace core{

        bool operator==(Device const & device1, Device const & device2){
            return device1.type == device2.type and device1.id == device2.id;
        }

        bool operator!=(Device const & device1, Device const & device2){
            return device1.type != device2.type or device1.id != device2.id;
        }

        Device host(){
            static Device host(deviceType::CPU, 0);
            return host;
        }


        NodeGroup::NodeGroup(const std::string name,
                             const std::weak_ptr<NodeGroup> parent) :
                name(name),
                graph(parent.lock()->graph),
                parent(parent) {
            if(parent.lock()->is_base()){
                full_name = name;
            } else {
                full_name = parent.lock()->full_name;
                full_name += graph->props.group_delimiter;
                full_name += name;
            }
            std::replace(full_name.begin(), full_name.end(), ' ', '_');
        };

        NodeGroup::NodeGroup(const std::string name,
                             const GraphInPtr graph):
        name(name), graph(graph), full_name(name) {};

        bool NodeGroup::is_base() const{
            return parent.expired();
        };
    }
}