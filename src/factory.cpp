//Created by Kamil
#include "factory.hpp"
#include "nodes.hpp"
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <vector>


bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {
    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("Error");
    }

    bool has_path_to_storehouse = false;

    for (const auto& [receiver, probability] : sender->receiver_preferences_.get_preferences()) {
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_path_to_storehouse = true;
        } else if (receiver->get_receiver_type() == ReceiverType::WORKER) {
            PackageSender* sendrecv_ptr = dynamic_cast<PackageSender*>(dynamic_cast<Worker*>(receiver));

            if (sendrecv_ptr == sender) {
                continue;
            }

            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED) {
                if (has_reachable_storehouse(sendrecv_ptr, node_colors)) {
                    has_path_to_storehouse = true;
                }
            } else if (node_colors[sendrecv_ptr] == NodeColor::VERIFIED) {
                has_path_to_storehouse = true;
            }
        }
    }

    if (has_path_to_storehouse) {
        node_colors[sender] = NodeColor::VERIFIED;
        return true;
    }

    throw std::logic_error("Error");
}


void Factory::remove_worker(ElementID id) {
    auto it = cont_w.find_by_id(id);
    if (it != cont_w.end()) {
        IPackageReceiver* receiver_ptr = dynamic_cast<IPackageReceiver*>(&(*it));
        for (auto& ramp : cont_r) {
            ramp.receiver_preferences_.remove_receiver(receiver_ptr);
        }
        for (auto& worker : cont_w) {
            worker.receiver_preferences_.remove_receiver(receiver_ptr);
        }
    }
    cont_w.remove_by_id(id);
}

void Factory::remove_storehouse(ElementID id) {
    auto it = cont_s.find_by_id(id);
    if (it != cont_s.end()) {
        IPackageReceiver* receiver_ptr = dynamic_cast<IPackageReceiver*>(&(*it));
        for (auto& ramp : cont_r) {
            ramp.receiver_preferences_.remove_receiver(receiver_ptr);
        }
        for (auto& worker : cont_w) {
            worker.receiver_preferences_.remove_receiver(receiver_ptr);
        }
    }
    cont_s.remove_by_id(id);
}

void Factory::do_deliveries(Time t) {
    for (auto& ramp : cont_r) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_work(Time t) {
    for (auto& worker : cont_w) {
        worker.do_work(t);
    }
}

void Factory::do_package_passing() {
    for (auto& ramp : cont_r) {
        ramp.send_package();
    }
    for (auto& worker : cont_w) {
        worker.send_package();
    }
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> node_colors;

    for (const auto& worker : cont_w) {
        node_colors[dynamic_cast<const PackageSender*>(&worker)] = NodeColor::UNVISITED;
    }
    for (const auto& ramp : cont_r) {
        node_colors[dynamic_cast<const PackageSender*>(&ramp)] = NodeColor::UNVISITED;
    }

    try {
        for (const auto& ramp : cont_r) {
            has_reachable_storehouse(dynamic_cast<const PackageSender*>(&ramp), node_colors);
        }
    } catch (const std::logic_error&) {
        return false;
    }

    return true;
}

std::vector<std::string> character_split(const std::string& splittable_str, char delimiter) {
    std::stringstream parameter_stream(splittable_str);
    std::string part;
    std::vector<std::string> result;

    while (std::getline(parameter_stream, part, delimiter)) {
        if (!part.empty()) result.push_back(part);
    }
    return result;
}

ParsedLineData parse_line(std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(line);

    while (token_stream >> token) {
        tokens.push_back(token);
    }

    ParsedLineData parsed_data;
    std::map<std::string, ElementType> element_types{
            {"LOADING_RAMP", ElementType::RAMP},
            {"WORKER", ElementType::WORKER},
            {"STOREHOUSE", ElementType::STOREHOUSE},
            {"LINK", ElementType::LINK},
    };

    try {
        parsed_data.element_type = element_types.at(tokens[0]);
        for (auto it = std::next(tokens.begin()); it != tokens.end(); ++it) {
            auto key_value = character_split(*it, '=');
            if (key_value.size() == 2) {
                parsed_data.parameters[key_value[0]] = key_value[1];
            }
        }
    } catch (...) {
        throw std::invalid_argument("Invalid line format");
    }

    return parsed_data;
}

PackageQueueType get_package_queue_type(std::string str) {
    if (str == "LIFO") return PackageQueueType::LIFO;
    if (str == "FIFO") return PackageQueueType::FIFO;
    throw std::invalid_argument("Invalid queue type");
}

void link(Factory& factory, const std::map<std::string, std::string>& parameters) {
    auto src_params = character_split(parameters.at("src"), '-');
    auto dest_params = character_split(parameters.at("dest"), '-');

    IPackageReceiver* receiver = nullptr;
    ElementID dest_id = std::stoi(dest_params[1]);

    if (dest_params[0] == "worker") {
        receiver = &(*factory.find_worker_by_id(dest_id));
    } else if (dest_params[0] == "store") {
        receiver = &(*factory.find_storehouse_by_id(dest_id));
    }

    ElementID src_id = std::stoi(src_params[1]);
    if (src_params[0] == "ramp") {
        factory.find_ramp_by_id(src_id)->receiver_preferences_.add_receiver(receiver);
    } else if (src_params[0] == "worker") {
        factory.find_worker_by_id(src_id)->receiver_preferences_.add_receiver(receiver);
    }
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty() || line[0] == ';') continue;

        ParsedLineData parsed = parse_line(line);

        if (parsed.element_type == ElementType::RAMP) {
            factory.add_ramp(Ramp(std::stoi(parsed.parameters.at("id")), 
                                 std::stoi(parsed.parameters.at("delivery-interval"))));
        } else if (parsed.element_type == ElementType::WORKER) {
            factory.add_worker(Worker(std::stoi(parsed.parameters.at("id")), 
                                     std::stoi(parsed.parameters.at("processing-time")), 
                                     std::make_unique<PackageQueue>(get_package_queue_type(parsed.parameters.at("queue-type")))));
        } else if (parsed.element_type == ElementType::STOREHOUSE) {
            factory.add_storehouse(Storehouse(std::stoi(parsed.parameters.at("id"))));
        } else if (parsed.element_type == ElementType::LINK) {
            link(factory, parsed.parameters);
        }
    }
    return factory;
}

void link_stream_fill(std::stringstream& link_stream, const PackageSender& sender, ElementID id, std::string name) {
    auto prefs = sender.receiver_preferences_.get_preferences();
    for (auto const& [receiver, prob] : prefs) {
        std::string d_type = (receiver->get_receiver_type() == ReceiverType::WORKER) ? "worker" : "store";
        link_stream << "LINK src=" << name << "-" << id << " dest=" << d_type << "-" << receiver->get_id() << "\n";
    }
}

void save_factory_structure(Factory& factory, std::ostream& os) {
    std::stringstream link_stream;

    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "LOADING_RAMP id=" << it->get_id() << " delivery-interval=" << it->get_delivery_interval() << "\n";
        link_stream_fill(link_stream, *it, it->get_id(), "ramp");
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        std::string qt = (it->get_queue()->get_queue_type() == PackageQueueType::LIFO) ? "LIFO" : "FIFO";
        os << "WORKER id=" << it->get_id() << " processing-time=" << it->get_processing_duration() << " queue-type=" << qt << "\n";
        link_stream_fill(link_stream, *it, it->get_id(), "worker");
    }

    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }

    os << link_stream.str();
    os.flush();
}