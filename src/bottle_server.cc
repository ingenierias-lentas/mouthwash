#include "bottle_server.hh"

#include <assert.h>
#include <stdlib.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>

#include <rxcpp/rx.hpp>

using namespace mouthwash;

using grpc::ServerBuilder;
using grpc::health::v1::HealthCheckResponse;

std::atomic<bool> shutdownVar(false);

class Call {
    public:
    virtual void Proceed(bool ok) = 0;
};

class HealthCheckCall final : public Call {
    public:
    explicit
    HealthCheckCall(Health::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(PROCESS)
    {
        on_done = [&](bool ok) { OnDone(ok); };
        proceed = [&](bool ok) { Proceed(ok); };
        ctx_.AsyncNotifyWhenDone(&on_done);
        service_->RequestCheck(&ctx_, &request_, &responder_, cq_, cq_, &proceed);
    }

    void
    Proceed(bool ok)
    {
        if (status_ == PROCESS) {
            if (!ok) {
                delete this;
                return;
            }
            new HealthCheckCall(service_, cq_);

            response_.set_status(
                HealthCheckResponse::SERVING
            );

            status_ = FINISH;
            responder_.Finish(response_, Status::OK, &proceed);
        } else {
            GPR_ASSERT(status_ == FINISH);
            finish_called_ = true;
            if (on_done_called_)
                delete this;
            std::cout << "Sent health check response." << std::endl;
        }
    }

    void
    OnDone(bool ok) {
        assert(ok);
        //TODO identify which health check by id
        if (ctx_.IsCancelled())
            std::cerr << "Health check cancelled" << std::endl;
        if (finish_called_)
            delete this;
        else
            status_ = FINISH;
    }

    std::function<void(bool)> proceed;
    std::function<void(bool)> on_done;

    private:
    Health::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;
    ServerAsyncResponseWriter<HealthCheckResponse> responder_;

    HealthCheckRequest request_;
    HealthCheckResponse response_;

    enum CallStatus{ PROCESS, FINISH };
    CallStatus status_;

    bool finish_called_;
    bool on_done_called_;
};

class HealthWatchCall final : public Call {
    public:
    explicit
    HealthWatchCall(
        Health::AsyncService* service,
        ServerCompletionQueue* cq
    )
        : service_(service), cq_(cq), responder_(&ctx_), status_(PROCESS)
    {
        // Make helpers for grpc completion queue and context
        on_done = [&](bool ok) { OnDone(ok); };
        proceed = [&](bool ok) { Proceed(ok); };

        // Make helpers for rxcpp
        observer_predicate = [=](int v){ return ObserverPredicate(v); };
        write_helper = [&](int v){ return WriteHelper(v); };
        finish_helper = [&](){ FinishHelper(); };

        ctx_.AsyncNotifyWhenDone(&on_done);
        service_->RequestWatch(&ctx_, &request_, &responder_, cq_, cq_, &proceed);
    }

    void
    Proceed(bool ok)
    {
        if (status_ == PROCESS) {
            if (!ok) {
                delete this;
                return;
            }
            response_.set_status(
                HealthCheckResponse::SERVING
            );
            new HealthWatchCall(service_, cq_);

            status_ = STREAM;
            auto start = std::chrono::steady_clock::now() + std::chrono::milliseconds(1);
            auto period = std::chrono::seconds(1);
            auto scheduler = rxcpp::observe_on_new_thread();
            auto values = rxcpp::observable<>::interval(start, period, scheduler);
            values.
                take_while(observer_predicate).
                subscribe(write_helper,
                          finish_helper);
        } else if (status_ == STREAM) {
            if (!ok) {
                status_ = FINISH;
            }
        } else {
            GPR_ASSERT(status_ == FINISH);
            finish_called_ = true;
            if (on_done_called_)
                delete this;
            std::cout << "Sent health watch response stream." << std::endl;
        }
    }

    void
    OnDone(bool ok) {
        assert(ok);
        //TODO identify which health check by id
        if (ctx_.IsCancelled())
            std::cerr << "Health watch cancelled" << std::endl;
        if (finish_called_)
            delete this;
        else
            status_ = FINISH;
    }

    std::function<void(bool)> proceed;
    std::function<void(bool)> on_done;
    std::function<bool(int)> observer_predicate;
    std::function<void(int)> write_helper;
    std::function<void(void)> finish_helper;

    private:

    bool
    ObserverPredicate(int v) {
        return status_ == STREAM;
    }

    void
    WriteHelper(int v) {
        response_.set_status(
            HealthCheckResponse::SERVING
        );
        responder_.Write(response_, &proceed);
    }

    void
    FinishHelper() {
        responder_.Finish(Status::OK, &proceed);
    }

    Health::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;
    ServerAsyncWriter<HealthCheckResponse> responder_;

    HealthCheckRequest request_;
    HealthCheckResponse response_;

    enum CallStatus{ PROCESS, STREAM, FINISH };
    CallStatus status_;

    bool finish_called_;
    bool on_done_called_;
};

class BottleTakeShotCall final : public Call {
    public:
    explicit
    BottleTakeShotCall(Bottle::AsyncService* service, ServerCompletionQueue* cq)
        : service_(service), cq_(cq), responder_(&ctx_), status_(PROCESS)
    {
        on_done = [&](bool ok) { OnDone(ok); };
        proceed = [&](bool ok) { Proceed(ok); };
        ctx_.AsyncNotifyWhenDone(&on_done);
        service_->RequestTakeShot(&ctx_, &request_, &responder_, cq_, cq_, &proceed);
    }

    void
    Proceed(bool ok)
    {
        if (status_ == PROCESS) {
            if (!ok) {
                delete this;
                return;
            }
            new BottleTakeShotCall(service_, cq_);

            std::string name("test");
            int length = 0;
            std::string soundbyte_format("test");
            response_.set_name(name);
            response_.set_length(length);
            response_.set_format(soundbyte_format);

            status_ = FINISH;
            responder_.Finish(response_, Status::OK, &proceed);
        } else {
            GPR_ASSERT(status_ == FINISH);
            finish_called_ = true;
            if (on_done_called_)
                delete this;
            std::cout << "Sent take shot response." << std::endl;

            GPR_ASSERT(status_ == FINISH);
            delete this;
        }
    }

    void
    OnDone(bool ok) {
        assert(ok);
        //TODO identify which health check by id
        if (ctx_.IsCancelled())
            std::cerr << "Shot call cancelled" << std::endl;
        if (finish_called_)
            delete this;
        else
            status_ = FINISH;
    }

    std::function<void(bool)> proceed;
    std::function<void(bool)> on_done;

    private:
    Bottle::AsyncService* service_;
    ServerCompletionQueue* cq_;
    ServerContext ctx_;
    ServerAsyncResponseWriter<Shot> responder_;

    ShotCall request_;
    Shot response_;

    enum CallStatus{ PROCESS, FINISH };
    CallStatus status_;

    bool finish_called_;
    bool on_done_called_;
};

BottleServiceImpl::~BottleServiceImpl() {
    server_->Shutdown(std::chrono::system_clock::now());
    for (auto cq = cqs_.begin(); cq != cqs_.end(); cq++)
        (*cq)->Shutdown();

    void *tag;
    bool ok;
    for (auto cq = cqs_.begin(); cq != cqs_.end(); cq++) {
        while ((*cq)->Next(&tag, &ok)) {
            auto proceed = static_cast<std::function<void(bool)>*>(tag);
            (*proceed)(ok);
        }
    }
}

void
BottleServiceImpl::Run(BarHelper& barHelper) {
    if (*barOk) {
        std::string server_address("0.0.0.0:50051");

        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&healthService_);
        builder.RegisterService(&bottleService_);
        cqs_.emplace_back(builder.AddCompletionQueue());
        server_ = builder.BuildAndStart();
        std::cout << "Server listening on " << server_address << std::endl;

        HandleRpcs(BarHelper& barHelper);
    }
}

void
BottleServiceImpl::HandleRpcs(BarHelper& barHelper) {
    new HealthCheckCall(&healthService_, cqs_[0].get());
    new HealthWatchCall(&healthService_, cqs_[0].get());
    new BottleTakeShotCall(&bottleService_, cqs_[0].get());
    void* tag;
    bool ok;
    while (cqs_[0]->Next(&tag, &ok) && barHelper.BarIsOk()) {
        auto proceed = static_cast<std::function<void(bool)>*>(tag);
        (*proceed)(ok);
        if (shutdownVar.load())
            break;
    }
}
