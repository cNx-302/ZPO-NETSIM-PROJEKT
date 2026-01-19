//
// Created by Błażej on 19.01.2026.
//

#ifndef PACKAGE_HPP
#define PACKAGE_HPP

#include "types.hpp"
#include <set>

class Package {
public:
    Package();

    Package(ElementID id) : package_ID(id) {
        assigned_IDs.insert(package_ID);
    }

    Package(Package&& package) noexcept : package_ID(EMPTY_ID) {
        *this = std::move(package);
    }

    Package& operator=(Package&& package) noexcept;

    ElementID get_id() const { return package_ID; }

    ~Package();

private:

    ElementID package_ID;
    static std::set<ElementID> assigned_IDs;
    static std::set<ElementID> freed_IDs;
};



#endif //PACKAGE_HPP
