//
// Created by Błażej on 19.01.2026.
//

#include "storage_types.hpp"


Package PackageQueue::pop() {

    Package result(EMPTY_ID);

    switch (this->queue_type) {

        case PackageQueueType::FIFO:
            result = std::move(this->package_list.front());
        this->package_list.pop_front();
        break;

        case PackageQueueType::LIFO:
            result = std::move(this->package_list.back());
        this->package_list.pop_back();
        break;
    }

    return result;
}