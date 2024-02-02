#pragma once
#include <unordered_map>
#include <unordered_set>

template<class SubscriberId, class... Data>
struct PubSubDataSink {
	typedef std::function<void(Data...)> DataCallback;
	bool produce(const Data&... data) {
		for (auto const& [subscriberId, callback] : m_routingTable) {
			callback(data...);
		}
	}

	bool consume(const SubscriberId& subscriberId, const DataCallback& callback) {
		if (auto it = m_routingTable.find(subscriberId); it == m_routingTable.end()) {
			m_routingTable[subscriberId] = callback;
		} else {
			return false;
		}
	}

	bool unregister(const SubscriberId& subscriberId) {
		bool retVal = false;
		if (auto it = m_routingTable.find(subscriberId); it != m_routingTable.end()) {
			m_routingTable.erase(it);
			retVal = true;
		}

		return retVal;
	}

	private:
	std::unordered_map<SubscriberId, DataCallback> m_routingTable;
};

//Acts a light weight pub-sub mechanism
//Making it templatized make is to be able to be used as a type safe mechanism
template<class Key, class SubscriberId, class KeyHasher, class... Data>
struct PubSubDataRouter
{
	typedef std::function<void(const Data&...)> DataCallback;

	//Produce Data with the specified key
	bool produce(const Key& key, const Data&... data)
	{
		bool retVal = false;
		if (auto it = m_routingTable.find(key); it != m_routingTable.end()) {
			retVal = true;
			for (auto const& [registrationId, callback] : it->second) {
				callback(data...);
				retVal = true;
			}
		}

		return retVal;
	}


	bool consume(const Key& key, const SubscriberId& subscriberId, const DataCallback& callback)
	{
		bool retVal = true;
		if (auto it = m_routingTable.find(key); it != m_routingTable.end())
			if (auto itCallback = it->second.find(subscriberId); itCallback == it->second.end()) {
				it->second[subscriberId] = callback;
				m_regIdToKeys[subscriberId].insert(key);
			}
			else
				retVal = false;
		else {
			m_routingTable[key] = { {subscriberId, callback} };
			m_regIdToKeys[subscriberId].insert(key);
		}

		return retVal;
	}

	bool consumeEveryThing(const SubscriberId& subscriberId, const DataCallback& callback)
	{
		return true;
	}

	bool unregister(const Key& key, const SubscriberId& subscriberId)
	{
		return removeFromRoutingTable(key, subscriberId) &&
			   removeFromRegIdStore(key, subscriberId);
	}

	bool unregisterAll(const SubscriberId& subscriberId)
	{
		if (auto it = m_regIdToKeys.find(subscriberId); it != m_regIdToKeys.end()) {
			auto const& [regId, keys] = *it;
			for (auto const& key : keys) {
				removeFromRoutingTable(key, subscriberId);
			}

			m_regIdToKeys.erase(it);
			return true;
		} else {
			return false;
		}
	}

private:
	bool removeFromRoutingTable(const Key& key, const SubscriberId& subscriberId) {
		bool retVal = false;
		if (auto it = m_routingTable.find(key); it != m_routingTable.end()) {
			if (auto itCallback = it->second.find(subscriberId); itCallback != it->second.end()) {
				it->second.erase(itCallback);
				if (it->second.empty()) {
					m_routingTable.erase(it);
				}

				retVal = true;
			}
		}

		return retVal;
	}

	bool removeFromRegIdStore(const Key& key, const SubscriberId& subscriberId) {
		auto it = m_regIdToKeys.find(subscriberId);
		if (it == m_regIdToKeys.end()) {
			return false;
		}

		auto& [regId, keys] = *it;
		if(0 == keys.erase(key)) {
			return false;
		}

		if (keys.empty()) {
			m_regIdToKeys.erase(it);
		}

		return true;
	}

	std::unordered_map<Key, std::unordered_map<SubscriberId, DataCallback>, KeyHasher> m_routingTable;
	std::unordered_map<SubscriberId, std::unordered_set<Key, KeyHasher>> m_regIdToKeys;
};

template <class Key, class... Data>
struct ProducerFunctions
{
	std::function<bool (const Key&, const Data&...)> produce;
};

template <class Key, class SubscriberId, class... Data>
struct ConsumerFunctions
{
	typedef std::function<void(const Data&...)> DataCallback;
	std::function<bool(const Key&, const SubscriberId&, const DataCallback&)> consume;
	std::function<bool(const SubscriberId&, const DataCallback&)> consumeEveryThing;
	std::function<bool(const Key&, const SubscriberId&)> unregister;
	std::function<bool(const SubscriberId&)> unregisterAll;
};

//Acts a light weight request-response mechanism
//Making it templatized make is to be able to be used as a type safe mechanism'
template<class ResponderId, class ReqId, class ReqData, class ResponderIdHasher, class ReqIdHasher, class... Response>
struct EndToEndReqRespRouter {

	typedef std::function<void(const Response&...)> ResponseHandler;
	typedef std::function<void(const ReqData&, const ResponseHandler&)> ReqListener;
	typedef PubSubDataRouter<ResponderId, size_t, ResponderIdHasher, ReqData, ResponseHandler> ReqRouter;
	typedef PubSubDataRouter<ReqId, size_t, ReqIdHasher, Response...> RespRouter;

	EndToEndReqRespRouter() m_reqPending(false) {}

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

	void request(const ResponderId& responderId,
				 const ReqId& reqId,
				 const ReqData& reqData,
				 const ResponseHandler& responseHandler) {
		if (m_pendingreqIds.find(reqId) != m_pendingreqIds.end()) {
			throw std::runtime_error("Duplicate reqId");
		} 
		m_pendingreqIds.insert(reqId);
		
		auto func =
		[this](){
			m_reqPending = true;
			m_reqRouter.produce(responderId, reqData, [this, reqId, responseHandler](const Response&... response) {
				if (auto it = m_pendingreqIds.find(reqId); it != m_pendingreqIds.end()) {
					responseHandler(response...);
					m_pendingreqIds.erase(it);
				}
			});
			m_reqPending = false;
		};

		if (m_reqPending) {
			m_pendingReqProcessors.push_back(func);
		} else {
			func();
		}
	}

	bool cancelRequest(const ReqId& reqId) {
		bool retVal = false;
		if (auto it = m_pendingreqIds.find(reqId); it != m_pendingreqIds.end()) {
			m_pendingreqIds.erase(it);
			retVal = true;
		}

		return retVal;
	}

	private:
	bool m_reqPending;
	std::vector<std::function<void()>> m_pendingReqProcessors;
	ReqRouter m_reqRouter;
	std::unordered_set<ReqId> m_pendingreqIds;
};


template<class ResponderId, class ReqType, class ReqId, class ReqData, class ReqTypeHasher, class ReqIdHasher, class... Response>
struct GenericReqRespRouter {

	typedef std::function<void(const Response&...)> ResponseHandler;
	typedef std::function<void(const ReqData&, const ResponseHandler&)> ReqListener;
	typedef PubSubDataRouter<ReqType, size_t, ReqTypeHasher, ReqData, ResponseHandler> ReqRouter;
	typedef PubSubDataRouter<ReqId, size_t, ReqIdHasher, Response...> RespRouter;

	bool registerAsResponder(const ResponderId& responderId, const ReqType& reqType, const ReqListener& reqListener) {
		if(auto it = m_responderBook.find(reqType); it == m_responderBook.end()) {
			m_responderBook[reqType] = responderId;
			m_reqRouter.consume(reqType, (size_t)this, reqListener);
			return true;
		} else {
			return false;
		}
	}

	bool unregisterAsResponder(const ResponderId& responderId, const ReqType& reqType) {
		if( auto it = m_responderBook.find(reqType); it != m_responderBook.end()) {
			m_responderBook.erase(it);
			m_reqRouter.unregister(reqType, (size_t)this);
			return true;
		} else {
			return false;
		}
	}

	bool request(const ReqId& reqId,
				 const ReqType& reqType,
				 const ReqData& reqData,	
				 const ResponseHandler& responseHandler) {
		if (m_pendingreqIds.find(reqId) != m_pendingreqIds.end()) {
			return false;
		}

		m_pendingreqIds.insert(reqId);
		m_reqRouter.produce(reqType, reqData, [this, reqId, responseHandler](const Response&... response) {
			if (auto it = m_pendingreqIds.find(reqId); it != m_pendingreqIds.end()) {
				responseHandler(response...);
				m_pendingreqIds.erase(it);
			}
		});
		return true;
	}

	bool cancelRequest(const ReqId& reqId) {
		bool retVal = false;
		if (auto it = m_pendingreqIds.find(reqId); it != m_pendingreqIds.end()) {
			m_pendingreqIds.erase(it);
			retVal = true;
		}

		return retVal;
	}

	private:
	ReqRouter m_reqRouter;
	std::unordered_set<ReqId> m_pendingreqIds;
	std::unordered_map<ReqType, ResponderId> m_responderBook;
};


template<class ResponderId, class ReqId, class ReqData, class... Response>
struct EndToEndResponderFunctions {
	typedef std::function<void(const Response&...)> ResponseHandler;
	typedef std::function<void(const ReqData&, const ResponseHandler&)> ReqListener;
	std::function<bool(const ResponderId&, const ReqListener&)> registerAsResponder;
	std::function<bool(const ResponderId&)> unregisterAsResponder;
};

template<class ResponderId, class ReqId, class ReqData, class... Response>
struct EndToEndRequesterFunctions {
	typedef std::function<void(const Response&...)> ResponseHandler;
	std::function<bool(const ResponderId&,
				 	   const ReqId&,
				 	   const ReqData&,
				 	   const ResponseHandler&)> request;

	std::function<bool(const ReqId&)> cancelRequest;
};

template<class ResponderId, class ReqType, class ReqData, class... Response>
struct GenericResponderFunctions {
	typedef std::function<void(const Response&...)> ResponseHandler;
	typedef std::function<void(const ReqData&, const ResponseHandler&)> ReqListener;

	std::function<bool(const ResponderId&,
					   const ReqType&,
					   const ReqListener&)> registerAsResponder;

	std::function<bool(const ResponderId&,
					   const ReqType&)> unregisterAsResponder;
};

template<class ReqType, class ReqId, class ReqData, class... Response>
struct GenericRequesterFunctions {
	typedef std::function<void(const Response&...)> ResponseHandler;
	std::function<bool (const ReqId& reqId,
				  const ReqType& reqType,
				  const ReqData& reqData,	
				  const ResponseHandler& responseHandler)> request;
	
	std::function<bool(const ReqId& reqId)> cancelRequest;
};


template<class Key, class SubData, class UnsubData, class KeyHasher = std::hash<key>, class KeyEquator = std::equal_to<Key>>
struct SingleLinkSubUnsubHandler
{
	typedef std::function<void(const SubData&)> SubAction;
	typedef std::function<void(const UnsubData&)> UnsubAction;
	typedef std::function<UnsubData(const Key&)> UnsubDataGenerator;

	bool subscribe(const Key& key,
				   const SubData& subData,
				   const SubAction& subAction)
	{
		if (m_subscriptionBook.insert(key).second)
		{
			subAction(subData);
			return true;
		}
		else
			return false;
	}

	bool unsubscribe(const Key& key,
					 const UnsubData& unsubData,
					 const UnsubAction& unsubAction)
	{
		if (m_subscriptionBook.erase(key))
		{
			unsubAction(unsubData);
			return true;
		}
		else
			return false;
	}

	void unsubscribeAll(const UnsubDataGenerator&  unsubDataGenerator, 
						const UnsubAction& unsubAction) {
		for (auto const& key : m_subscriptionBook) {
			unsubAction(unsubDataGenerator(key));
		}

		m_subscriptionBook.clear();
	}

private:
	std::unordered_set<Key, KeyHasher, KeyEquator> m_subscriptionBook;
};

template<class Key, class SubData, class UnsubData, class KeyHasher = std::hash<Key>, class KeyEquator = std::equal_to<Key>>
struct AgregatedSubUnsubHandler
{
	typedef std::function<void(const SubData&)> SubAction;
	typedef std::function<void(const UnsubData&)> UnsubAction;

	bool subscribe(const Key& key, const SubData& data, const SubAction& subAction)
	{
		if (auto it = m_subscriptionBook.find(key); it != m_subscriptionBook.end())
		{
			++it->second;
			return false;
		}
		else
		{
			m_subscriptionBook[key] = 1;
			subAction(data);
			return true;
		}
	}

	bool unsubscribe(const Key& key, const UnsubData& data, const UnsubAction& unsubAction)
	{
		if (auto it = m_subscriptionBook.find(key); it != m_subscriptionBook.end())
		{
			if (!(--it->second))
			{
				unsubAction(data);
				m_subscriptionBook.erase(it);
				return true;
			}
		}

		return false;
	}

	void resubscribeAll(const std::function<SubData(Key)>& subDataGenerator, const SubAction& subAction)
	{
		for (auto const& [key, numSubscribers] : m_subscriptionBook) {
			subAction(subDataGenerator(key));
		}
	}
private:
	std::unordered_map<Key,
					   size_t,
					   KeyHasher,
					   KeyEquator> m_subscriptionBook;
};


