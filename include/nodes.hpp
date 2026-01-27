//
// Created by Błażej on 19.01.2026.
//

#ifndef NODES_HPP
#define NODES_HPP


#include "types.hpp"
#include "package.hpp"
#include "helpers.hpp"
#include "storage_types.hpp"
#include <optional>
#include <memory>
#include <map>


enum class ReceiverType {
     WORKER, STOREHOUSE
};

class IPackageReceiver {
public:
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;


    virtual ReceiverType get_receiver_type() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator)
        : prob_gen_(std::move(pg)) {}

    void add_receiver(IPackageReceiver* receiver);
    void remove_receiver(IPackageReceiver* receiver);
    IPackageReceiver* choose_receiver();

    const preferences_t& get_preferences() const { return preferences_; }

    const_iterator cbegin() const { return preferences_.cbegin(); }
    const_iterator cend() const { return preferences_.cend(); }
    const_iterator begin() const { return preferences_.begin(); }
    const_iterator end() const { return preferences_.end(); }

private:
    preferences_t preferences_;
    ProbabilityGenerator prob_gen_;
};

class PackageSender {
public:
    PackageSender() = default;
    PackageSender(PackageSender&& other) = default;

    void send_package();
    const std::optional<Package>& get_sending_buffer() const { return sending_buffer_; }

    ReceiverPreferences receiver_preferences_;

protected:
    void push_package(Package&& p) { sending_buffer_.emplace(std::move(p)); }

    std::optional<Package> sending_buffer_ = std::nullopt;
};

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di) : id_(id), delivery_interval_(di) {}

    void deliver_goods(Time t);
    TimeOffset get_delivery_interval() const { return delivery_interval_; }
    ElementID get_id() const { return id_; }

private:
    ElementID id_;
    TimeOffset delivery_interval_;
};

class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO))
        : id_(id), stockpile_(std::move(d)) {}

    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }

    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }


    IPackageStockpile::const_iterator cbegin() const override { return stockpile_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return stockpile_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return stockpile_->begin(); }
    IPackageStockpile::const_iterator end() const override { return stockpile_->end(); }

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> stockpile_;
};

class Worker : public PackageSender, public IPackageReceiver {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
        : id_(id), processing_duration_(pd), queue_(std::move(q)) {}

    void do_work(Time t);
    TimeOffset get_processing_duration() const { return processing_duration_; }
    Time get_package_processing_start_time() const { return start_time_; }

    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }

    IPackageQueue* get_queue() const { return queue_.get(); }

    const std::optional<Package>& get_sending_buffer() const { return sending_buffer_; }

    const std::optional<Package>& get_processing_buffer() const { return processing_buffer_; }

    IPackageStockpile::const_iterator cbegin() const override { return queue_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return queue_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return queue_->begin(); }
    IPackageStockpile::const_iterator end() const override { return queue_->end(); }

private:
    ElementID id_;
    TimeOffset processing_duration_;
    Time start_time_ = 0;
    std::unique_ptr<IPackageQueue> queue_;
    std::optional<Package> processing_buffer_ = std::nullopt;
};

#endif //NODES_HPP
