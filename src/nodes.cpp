//
// Created by Błażej on 19.01.2026.
//

#include "nodes.hpp"


void ReceiverPreferences::add_receiver(IPackageReceiver* receiver) {
    double num_receivers = static_cast<double>(preferences_.size());
    if (num_receivers == 0) {
        preferences_[receiver] = 1.0;
    } else {
        double new_prob = 1.0 / (num_receivers + 1.0);
        for (auto& pair : preferences_) {
            pair.second = new_prob;
        }
        preferences_[receiver] = new_prob;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* receiver) {
    if (preferences_.find(receiver) == preferences_.end()) {
        return;
    }

    preferences_.erase(receiver);

    if (preferences_.empty()) {
        return;
    }

    double new_prob = 1.0 / static_cast<double>(preferences_.size());
    for (auto& pair : preferences_) {
        pair.second = new_prob;
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double p = prob_gen_();
    double cumulative_sum = 0.0;

    for (auto const& [receiver, probability] : preferences_) {
        cumulative_sum += probability;
        if (p <= cumulative_sum) {
            return receiver;
        }
    }
    return nullptr;
}


void PackageSender::send_package() {
    if (sending_buffer_.has_value()) {
        IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
        if (receiver) {
            receiver->receive_package(std::move(sending_buffer_.value()));
            sending_buffer_.reset();
        }
    }
}


void Ramp::deliver_goods(Time t) {
    if ((t - 1) % delivery_interval_ == 0) {
        push_package(Package());
    }
}


void Worker::receive_package(Package&& p) {
    queue_->push(std::move(p));
}

void Worker::do_work(Time t) {
    if (!processing_buffer_.has_value() && !queue_->empty()) {
        processing_buffer_.emplace(queue_->pop());
        start_time_ = t;
    }

    if (processing_buffer_.has_value()) {
        if (t - start_time_ + 1 >= processing_duration_) {
            push_package(std::move(processing_buffer_.value()));
            processing_buffer_.reset();
        }
    }
}



void Storehouse::receive_package(Package&& p) {
    stockpile_->push(std::move(p));
}