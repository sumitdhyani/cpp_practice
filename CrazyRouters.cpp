#include "CrazyRouters.hpp"
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <array>
#include <iostream>
#include <string>

//Acts a light weight request-response mechanism
//Making it templatized make is to be able to be used as a type safe mechanism'



//Acts a light weight pub-sub mechanism
//Making it templatized make is to be able to be used as a type safe mechanism

void demoEndToEndReqRespRouter() {
	auto responder = [](const size_t& num, const std::function<void(const size_t&, const size_t&)>& callback) {
		callback(num*num, num*num*num);
	};
	//template<class ResponderId, class ReqId, class ReqData, class... Response>
	EndToEndReqRespRouter<size_t, size_t, size_t, std::hash<size_t>, std::hash<size_t>, size_t, size_t> reqRespRouter;
	reqRespRouter.registerAsResponder(0, responder);
	size_t reqId = 1;
	std::function<void(size_t,size_t)> resultHandler = [&reqId, &reqRespRouter, &resultHandler](const size_t& res1, const size_t& res2) mutable {
		std::cout << "ReqId: " << reqId << ", Results: " << res1 << ", " << res2 << std::endl;
		if (reqId++ < 100) {
			std::cout << "Sending ReqId: " << reqId << std::endl;
			reqRespRouter.request(0, reqId, reqId, resultHandler);
		}
	};

	reqRespRouter.request(0, reqId, reqId, resultHandler);
}

void demoGenericReqRespRouter() {
	using ResponseHandler = std::function<void(const size_t&, const size_t&)>;
	using RequestHandler = std::function<void(const size_t&, const ResponseHandler&)>;
	RequestHandler responder1 = [](const size_t& num, const ResponseHandler& callback) {
		callback(num*num, num*num*num);
	};

	RequestHandler responder2 = [](const size_t& num, const ResponseHandler& callback) {
		callback(num*2, num*3);
	};

	std::array<RequestHandler, 2> responders = {responder1, responder2};
	
	//template<class ResponderId, class ReqType, class ReqId, class ReqData, class ReqTypeHasher, class ReqIdHasher, class... Response>
	GenericReqRespRouter<size_t, size_t, size_t, size_t, std::hash<size_t>, std::hash<size_t>, size_t, size_t> reqRespRouter;
	reqRespRouter.registerAsResponder(0, 0, responder1);
	reqRespRouter.registerAsResponder(1, 1, responder2);
	size_t reqId = 1;
	std::function<void(size_t,size_t)> resultHandler = [&reqId, &reqRespRouter, &resultHandler](const size_t& res1, const size_t& res2) mutable {
		std::cout << "ReqId: " << reqId << ", Results: " << res1 << ", " << res2 << std::endl;
		if (reqId++ < 100) {
			std::cout << "Sending ReqId: " << reqId << std::endl;
			reqRespRouter.request(reqId, reqId%2, reqId, resultHandler);
		}
	};

	reqRespRouter.request(reqId, reqId%2, reqId, resultHandler);
}


int main() {
	demoEndToEndReqRespRouter();
	demoGenericReqRespRouter();
	return 0;
}