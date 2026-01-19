//
// Created by Błażej on 19.01.2026.
//

#ifndef STORAGE_TYPES_HPP
#define STORAGE_TYPES_HPP

#include <list>
#include "package.hpp"
#include <iostream>

enum class PackageQueueType {
    FIFO,
    LIFO
};


class IPackageStockpile {

    public:
        using const_iterator = std::list<Package>::const_iterator;

        virtual void push(Package&& moved) = 0;

        virtual bool empty() const = 0;

        virtual size_t size() const = 0;

        virtual const_iterator begin() const = 0;

        virtual const_iterator end() const = 0;

        virtual const_iterator cbegin() const = 0;

        virtual const_iterator cend() const = 0;

        virtual ~IPackageStockpile() = default;
};

class IPackageQueue : public IPackageStockpile{

    public:

       ~IPackageQueue() override = default;

       virtual Package pop() = 0;

       virtual PackageQueueType get_queue_type() const = 0;

};

class PackageQueue : public IPackageQueue {

    public:

        PackageQueue() = delete;

        explicit PackageQueue(PackageQueueType type) : queue_type(type), package_list() {}

        void push(Package&& package) override {
            this->package_list.emplace_back(std::move(package));
        }

        bool empty() const override {
            return this->package_list.empty();
        }

        size_t size() const override {
            return this->package_list.size();
        }

        const_iterator begin() const override {
            return this->package_list.cbegin();
        }

        const_iterator end() const override {
            return this->package_list.cend();
        }

        const_iterator cbegin() const override {
            return this->package_list.cbegin();
        }

        const_iterator cend() const override {
            return this->package_list.cend();
        }

        Package pop() override;

        PackageQueueType get_queue_type() const override {
            return this->queue_type;
        };

        ~PackageQueue() override = default;


    private:

        PackageQueueType queue_type;

        std::list<Package> package_list;

};

#endif //STORAGE_TYPES_HPP
