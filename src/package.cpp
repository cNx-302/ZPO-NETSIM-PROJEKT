//
// Created by Błażej on 19.01.2026.
//

#include "package.hpp"

std::set<ElementID> Package::assigned_IDs = {};
std::set<ElementID> Package::freed_IDs = {};

Package& Package :: operator = (Package&& package) noexcept{

    if (this->package_ID != EMPTY_ID) {
        assigned_IDs.erase(this->package_ID);
        freed_IDs.insert(this->package_ID);
    }

    this->package_ID = package.package_ID;

    package.package_ID = EMPTY_ID;

    return *this;

}

Package::Package() {
    if (freed_IDs.empty()) {
        if (assigned_IDs.empty()) {

            package_ID = 1;

        } else {

            package_ID = *assigned_IDs.rbegin() + 1;

        }
    } else {

        package_ID = *freed_IDs.begin();

        freed_IDs.erase(freed_IDs.begin());
    }

    assigned_IDs.insert(package_ID);
}

Package::~Package() {

    if (package_ID != EMPTY_ID) {

        assigned_IDs.erase(package_ID);
        freed_IDs.insert(package_ID);
    }
}