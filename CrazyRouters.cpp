#pragma once
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iostream>

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
		if (auto it = m_routingTable.find(key); it != m_routingTable.end())
			for (const auto& [registrationId, callback] : it->second)
			{
				callback(data...);
				retVal = true;
			}

		return retVal;
	}


	bool consume(const Key& key, const RegistrationId& registrationId, const DataCallback& callback)
	{
		bool retVal = true;
		if (auto it = m_routingTable.find(key); it != m_routingTable.end())
			if (auto itCallback = it->second.find(registrationId); itCallback == it->second.end())
				it->second[registrationId] = callback;
			else
				retVal = false;
		else
			m_routingTable[key] = { {registrationId, callback} };

		return retVal;
	}

	bool consumeEveryThing(const RegistrationId& registrationId, const DataCallback& callback)
	{
		return true;
	}

	bool unregister(const Key& key, const RegistrationId& registrationId)
	{
		bool retVal = false;
		if (auto it = m_routingTable.find(key); it != m_routingTable.end())
			if (auto itCallback = it->second.find(registrationId); itCallback != it->second.end())
			{
				it->second.erase(itCallback);
				retVal = true;
			}

		return retVal;
	}

	void unregisterAll(const RegistrationId& registrationId)
	{
		for (const auto& [key, callbacks] : m_routingTable)
			unregister(key, registrationId);
	}

private:
	std::unordered_map<Key, std::unordered_map<RegistrationId, DataCallback>> m_routingTable;
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

template<class AwaiterId, class ResponderId, class ReqId, class ReqData, class... Response>
struct EndToEndReqRespRouter {

	typedef std::function<void(const Response&...)> ResponseHandler;
	typedef std::function<void(const ReqData&, const ResponseHandler&)> ReqListener;

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
			m_respRouter.unregister(reqId, (size_t)this);
			})
		) {
			m_reqRouter.produce(responderId, reqData, [this, reqId](const Response&... response) {
				m_respRouter.produce(reqId, response...);
			});
			return true;
		} else {
			return false;
		}
	}

	bool cancelRequest(const ReqId& reqId) {
		return m_respRouter.unregister(reqId);
	}

	private:
	PubSubDataRouter<ResponderId, size_t, ReqData, ResponseHandler> m_reqRouter;
	PubSubDataRouter<ReqId, size_t, Response...> m_respRouter;
};
//////////////////////////////////////////////////////////////////////////////////////////////////


int main() {
    return 0;
}