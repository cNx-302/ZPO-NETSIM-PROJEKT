//Created by Kamil
#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP

#include "storage_types.hpp"
#include "nodes.hpp"
#include <list>
#include <map>
#include <string>
#include <vector>
#include <istream>
#include <ostream>
#include <algorithm>

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

template<class Node>
class NodeCollection {
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    void add(Node&& node) { container_.emplace_back(std::move(node)); }

    void remove_by_id(ElementID id) {
        auto it = find_by_id(id);
        if (it != container_.end()) {
            container_.erase(it);
        }
    }

    iterator find_by_id(ElementID id) {
        return std::find_if(container_.begin(), container_.end(),
                            [id](const Node& elem) { return elem.get_id() == id; });
    }

    const_iterator find_by_id(ElementID id) const {
        return std::find_if(container_.cbegin(), container_.cend(),
                            [id](const Node& elem) { return elem.get_id() == id; });
    }

    iterator begin() { return container_.begin(); }
    iterator end() { return container_.end(); }
    const_iterator begin() const { return container_.cbegin(); }
    const_iterator end() const { return container_.cend(); }
    const_iterator cbegin() const { return container_.cbegin(); }
    const_iterator cend() const { return container_.cend(); }

private:
    container_t container_;
};

class Factory {
public:
    
    void add_ramp(Ramp&& r) { cont_r.add(std::move(r)); }
    void remove_ramp(ElementID id) { cont_r.remove_by_id(id); }
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return cont_r.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return cont_r.find_by_id(id); }
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return cont_r.cbegin(); }
    NodeCollection<Ramp>::const_iterator ramp_cend() const { return cont_r.cend(); }

    void add_worker(Worker&& w) { cont_w.add(std::move(w)); }
    void remove_worker(ElementID id);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return cont_w.find_by_id(id); }
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return cont_w.find_by_id(id); }
    NodeCollection<Worker>::const_iterator worker_cbegin() const { return cont_w.cbegin(); }
    NodeCollection<Worker>::const_iterator worker_cend() const { return cont_w.cend(); }

    void add_storehouse(Storehouse&& s) { cont_s.add(std::move(s)); }
    void remove_storehouse(ElementID id);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return cont_s.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return cont_s.find_by_id(id); }
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return cont_s.cbegin(); }
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return cont_s.cend(); }

    bool is_consistent() const;
    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    NodeCollection<Ramp> cont_r;
    NodeCollection<Worker> cont_w;
    NodeCollection<Storehouse> cont_s;
};

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors);

enum class ElementType { RAMP, WORKER, STOREHOUSE, LINK };

struct ParsedLineData {
    ElementType element_type;
    std::map<std::string, std::string> parameters;
};

ParsedLineData parse_line(std::string& line);

Factory load_factory_structure(std::istream& is);

void save_factory_structure(Factory& factory, std::ostream& os);

std::vector<std::string> character_split(const std::string& splittable_str, char delimiter);
PackageQueueType get_package_queue_type(std::string str);
void link(Factory& factory, const std::map<std::string, std::string>& parameters);

#endif //NETSIM_FACTORY_HPP