#include "mixer_client.hh"

using grpc_impl::ClientAsyncResponseReader;
using grpc_impl::ClientAsyncReader;
using grpc::ClientContext;
using grpc::Status;

using namespace mouthwash;

// Subscribe mixer client to server
using mouthwash::Mixer;
using mouthwash::ShotElement;
using mouthwash::Ingredients;
using mouthwash::ShotReviewSummary;
using google::protobuf::Empty;

// Subscribe mixer health client to server
using grpc::health::v1::Health;
using grpc::health::v1::HealthCheckResponse;
using grpc::health::v1::HealthCheckRequest;

extern std::atomic<bool> shutdownVar;

struct Call {
    public:
    virtual void Proceed(bool ok) = 0;
};

struct HealthCheckCall : public Call {
    public:
    explicit
    HealthCheckCall(
            CompletionQueue* cq,
            std::unique_ptr<Health::Stub>& stub,
            HealthCheckRequest* request)
        :  cq_(cq), responseReader_(stub->PrepareAsyncCheck(&ctx_, *request, cq_))
    {
        proceed = [&](bool ok) { Proceed(ok); };
        callStatus_ = PROCESS;
    }

    void
    Proceed(bool ok) {
        if (callStatus_ == PROCESS) {
            callStatus_ = FINISH;
            responseReader_->Finish(&reply_, &status_, &proceed);
        } else {
            GPR_ASSERT(callStatus_ == FINISH);
            delete this;
        }
    }

    std::function<void(bool)> proceed;

    Status status_;
    std::unique_ptr<ClientAsyncResponseReader<HealthCheckResponse>> responseReader_;
    HealthCheckResponse reply_;

    private:
    ClientContext ctx_;
    CompletionQueue* cq_;

    enum CallStatus{ PROCESS, FINISH };
    CallStatus callStatus_;
};

struct HealthWatchCall : public Call {
    public:
    explicit
    HealthWatchCall(
            CompletionQueue* cq,
            std::unique_ptr<Health::Stub>& stub,
            HealthCheckRequest* request)
        : cq_(cq), responseReader_(stub->PrepareAsyncWatch(&ctx_, *request, cq_))
    {
        proceed = [&](bool ok) { Proceed(ok); };
        callStatus_ = PROCESS;
    }

    void
    Proceed(bool ok) {
        if (callStatus_ == PROCESS) {
            callStatus_ = FINISH;
            responseReader_->Finish(&status_, &proceed);
        } else {
            GPR_ASSERT(callStatus_ == FINISH);
            delete this;
        }
    }

    std::function<void(bool)> proceed;

    Status status_;
    std::unique_ptr<ClientAsyncReader<HealthCheckResponse>> responseReader_;
    HealthCheckResponse reply_;
    
    private:
    ClientContext ctx_;
    CompletionQueue* cq_;

    enum CallStatus{ PROCESS, FINISH };
    CallStatus callStatus_;
};

struct MixANewOneCall : public Call {
    public:
    explicit
    MixANewOneCall(
            CompletionQueue* cq,
            std::unique_ptr<Mixer::Stub>& stub,
            ShotElement* request)
        :  cq_(cq), responseReader_(stub->PrepareAsyncMixANewOne(&ctx_, *request, cq_))
    {
        proceed = [&](bool ok) { Proceed(ok); };
        callStatus_ = PROCESS;
    }

    void
    Proceed(bool ok) {
        if (callStatus_ == PROCESS) {
            callStatus_ = FINISH;
            responseReader_->Finish(&reply_, &status_, &proceed);
        } else {
            GPR_ASSERT(callStatus_ == FINISH);
            delete this;
        }
    }

    std::function<void(bool)> proceed;

    Status status_;
    std::unique_ptr<ClientAsyncResponseReader<Ingredients>> responseReader_;
    Ingredients reply_;

    private:
    ClientContext ctx_;
    CompletionQueue* cq_;

    enum CallStatus{ PROCESS, FINISH };
    CallStatus callStatus_;
};

struct ShotReviewCall : public Call {
    public:
    explicit
    ShotReviewCall(
            CompletionQueue* cq,
            std::unique_ptr<Mixer::Stub>& stub,
            ShotReviewSummary* request)
        :  cq_(cq), responseReader_(stub->PrepareAsyncShotReview(&ctx_, *request, cq_))
    {
        proceed = [&](bool ok) { Proceed(ok); };
        callStatus_ = PROCESS;
    }

    void
    Proceed(bool ok) {
        if (callStatus_ == PROCESS) {
            callStatus_ = FINISH;
            responseReader_->Finish(&reply_, &status_, &proceed);
        } else {
            GPR_ASSERT(callStatus_ == FINISH);
            delete this;
        }
    }

    std::function<void(bool)> proceed;

    Status status_;
    std::unique_ptr<ClientAsyncResponseReader<Empty>> responseReader_;
    Empty reply_;

    private:
    ClientContext ctx_;
    CompletionQueue* cq_;

    enum CallStatus{ PROCESS, FINISH };
    CallStatus callStatus_;
};

void
MixerClient::MixANewOne(const std::string& instrument){
    ShotElement request;
    request.set_instrument(instrument);

    ClientContext ctx;

    MixANewOneCall* call = new MixANewOneCall(&cq_, mixerStub_, &request);

    call->responseReader_->StartCall();

    call->responseReader_->Finish(&call->reply_, &call->status_, &call->proceed);
}

void
MixerClient::AsyncCompleteRpc() {
    void *tag;
    bool ok = false;
    while(cq_.Next(&tag, &ok)) {
        auto proceed = static_cast<std::function<void(bool)>*>(tag);
        (*proceed)(ok);
        if (shutdownVar.load())
            break;
    }
}

void
MixerClient::Run(BarHelper& barHelper) {
    /*
    while(barHelper.BarIsOk()) {
    }
    */
}
