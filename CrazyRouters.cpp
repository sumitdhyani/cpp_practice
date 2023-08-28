#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <memory>

//Acts a light weight request-response mechanism
//Making it templatized make is to be able to be used as a type safe mechanism'



//Acts a light weight pub-sub mechanism
//Making it templatized make is to be able to be used as a type safe mechanism
template<class Key, class RegistrationId, class... Data>
struct PubSubDataRouter
{
	typedef std::function<void(const Data&...)> DataCallback;

	//Produce Data with the specified key
	bool produce(const Key& key, const Data&... data)
	{
		bool retVal = false;
		if (auto it = m_routingTable.find(key); it != m_routingTable.end()) {
			retVal = true;
			for (const auto& [registrationId, callback] : it->second) {
				callback(data...);
				retVal = true;
			}
		}

		return retVal;
	}


	bool consume(const Key& key, const RegistrationId& registrationId, const DataCallback& callback)
	{
		bool retVal = true;
		if (auto it = m_routingTable.find(key); it != m_routingTable.end())
			if (auto itCallback = it->second.find(registrationId); itCallback == it->second.end()) {
				it->second[registrationId] = callback;
				m_regIdToKeys[registrationId].insert(key);
			}
			else
				retVal = false;
		else {
			m_routingTable[key] = { {registrationId, callback} };
			m_regIdToKeys[registrationId].insert(key);
		}

		return retVal;
	}

	bool consumeEveryThing(const RegistrationId& registrationId, const DataCallback& callback)
	{
		return true;
	}

	bool unregister(const Key& key, const RegistrationId& registrationId)
	{
		bool retVal = false;
		if (auto it = m_routingTable.find(key); it != m_routingTable.end()) {
			if (auto itCallback = it->second.find(registrationId); itCallback != it->second.end()) {
				it->second.erase(itCallback);

				auto itRegIdToKeys = m_regIdToKeys.find(registrationId);
				auto& [regId, keys] = *itRegIdToKeys;
				keys.erase(key);
				if (keys.empty()) {
					m_regIdToKeys.erase(itRegIdToKeys);
				}
				retVal = true;
			}
		}

		return retVal;
	}

	bool unregisterAll(const RegistrationId& registrationId)
	{
		if (auto it = m_regIdToKeys.find(registrationId); it != m_regIdToKeys.end()) {
			const auto& [regId, keys] = *it;
			for (const auto& key : keys) {
				unregister(key, registrationId);
			}
			m_regIdToKeys.erase(it);
			return true;
		} else {
			return false;
		}
	}

private:
	std::unordered_map<Key, std::unordered_map<RegistrationId, DataCallback>> m_routingTable;
	std::unordered_map<RegistrationId, std::unordered_set<Key>> m_regIdToKeys;
};

template <class Key, class... Data>
struct ProducerFunctions
{
	std::function<bool (const Key&, const Data&...)> produce;
};

template <class Key, class RegistrationId, class... Data>
struct ConsumerFunctions
{
	typedef std::function<void(const Data&...)> DataCallback;
	std::function<bool(const Key&, const RegistrationId&, const DataCallback&)> consume;
	std::function<bool(const RegistrationId&, const DataCallback&)> consumeEveryThing;
	std::function<bool(const Key&, const RegistrationId&)> unregister;
	std::function<void(const RegistrationId&)> unregisterAll;
};

template<class ResponderId, class ReqId, class ReqData, class... Response>
struct EndToEndReqRespRouter {

	typedef std::function<void(const Response&...)> ResponseHandler;
	typedef std::function<void(const ReqData&, const ResponseHandler&)> ReqListener;
	typedef PubSubDataRouter<ResponderId, size_t, ReqData, ResponseHandler> ReqRouter;
	typedef PubSubDataRouter<ReqId, size_t, Response...> RespRouter;

	bool registerAsResponder(const ResponderId& responderId, const ReqListener& reqListener) {
		return m_reqRouter.consume(responderId,
									(size_t)this,
				[this, reqListener](const ReqData& reqData,
									const ResponseHandler& responseHandler) {
					reqListener(reqData, responseHandler);
				}
		);
	}

	bool unregisterAsResponder(const ResponderId& responderId) {
		return m_reqRouter.unregister(responderId, (size_t)this);
	}

	bool request(const ResponderId& responderId,
				 const ReqId& reqId,
				 const ReqData& reqData,
				 const ResponseHandler& responseHandler) {
		if( m_respRouter.consume(reqId, (size_t)this, [this, reqId, responseHandler](const Response&... response) {
			responseHandler(response...);
			})
		) {
			m_reqRouter.produce(responderId, reqData, [this, reqId](const Response&... response) {
				m_respRouter.produce(reqId, response...);
				m_respRouter.unregister(reqId, (size_t)this);
			});
			return true;
		} else {
			return false;
		}
	}

	bool cancelRequest(const ReqId& reqId) {
		return m_respRouter.unregister(reqId, (size_t)this);
	}

	//~EndToEndReqRespRouter() {
	//	m_reqRouter.unregisterAll((size_t)this);
	//	m_respRouter.unregisterAll((size_t)this);
	//}

	private:
	ReqRouter m_reqRouter;
	RespRouter m_respRouter;
};
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>


int main() {
	auto responder = [](size_t num, const std::function<void(size_t, size_t)> callback) {
		callback(num*num, num*num*num);
	};
	//template<class ResponderId, class ReqId, class ReqData, class... Response>
	EndToEndReqRespRouter<size_t, size_t, size_t, size_t, size_t> reqRespRouter;
	reqRespRouter.registerAsResponder(1, responder);
	reqRespRouter.request(1, 1, 2, [](size_t res1, size_t res2) {
		std::cout<< "Results: " << res1 << ", " << res2 << std::endl; 
	});

	PubSubDataRouter<size_t, size_t, size_t, std::string> pubSubRouter;
	auto callback = [](const size_t& data, const std::string& str) {
		std::cout << "Received: " << data << ", " << str.c_str() << std::endl;
	};
//
	//pubSubRouter.consume(1, 1, callback);
	//pubSubRouter.consume(2, 1, callback);
//
	//pubSubRouter.produce(1, 1, "A");
	//pubSubRouter.produce(1, 2, "B");
	//pubSubRouter.produce(2, 3, "C");
	//pubSubRouter.produce(2, 4, "D");
//
	//pubSubRouter.unregister(2, 1);
	//std::cout << "Unregistered key: 2" << std::endl;
//
//
	//pubSubRouter.produce(1, 1, "A");
	//pubSubRouter.produce(1, 2, "B");
	//pubSubRouter.produce(2, 3, "C");
	//pubSubRouter.produce(2, 4, "D");
//
//
	//std::cout << "Just before end" << std::endl;
    return 0;
}