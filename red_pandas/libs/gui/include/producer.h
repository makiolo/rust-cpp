//
// Created by makiolo on 18-Feb-24.
//
#ifndef RED_PANDAS_PROJECT_PRODUCER_H
#define RED_PANDAS_PROJECT_PRODUCER_H

namespace zmq {
    class context_t;
}

void PublisherThread(zmq::context_t *ctx);
void SubscriberThread1(zmq::context_t *ctx);
void SubscriberThread2(zmq::context_t *ctx);

#endif //RED_PANDAS_PROJECT_PRODUCER_H
